// Fill out your copyright notice in the Description page of Project Settings.

#include "VehicleMovement.h"

#include "NiagaraComponent.h"
#include "PlayerVehicle.h"
#include "SuspensionComponent.h"
#include "VehicleData.h"
#include "WheelSetup.h"
#include "Components/ArrowComponent.h"
#include "Components/AudioComponent.h"
#include "Components/PointLightComponent.h"

UVehicleMovement::UVehicleMovement()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UVehicleMovement::BeginPlay()
{
	Super::BeginPlay();

	VehicleOwner = Cast<APlayerVehicle>(GetOwner());

	if (!IsValid(VehicleOwner))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,TEXT("VehicleMovementComponent is not attached to a valid APlayerVehicle!"));
	}
}

void UVehicleMovement::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	if (OnSpeedUpdate.IsBound())
	{
		OnSpeedUpdate.Clear();
	}
}

void UVehicleMovement::TickComponent(float DeltaTime,ELevelTick TickType,FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!IsValid(VehicleOwner) || !IsValid(VehicleOwner->VehicleData) || !IsValid(VehicleOwner->BoxCollider))
	{
		return;
	}
	
	SetVariablesToFrame(DeltaTime);
	UpdateSuspension();
	CustomGravity();
	Turning();
	SteeringWheel();
	RollingWheel();
	ApplyFrictionForce();
	SetAngularDamping();
	ApplyBrake();
	ApplyAcceleration();
	SoundMotorPitch();
}

#pragma region CalculateVariables
void UVehicleMovement::SetVariablesToFrame(float DeltaTime)
{
	CurrentSpeed = FMath::Abs(Velocity.Size());
	OnSpeedUpdate.Broadcast(CurrentSpeed);
	CalculateNormalizedSpeed();
	CalculateSteering(DeltaTime);
	LinearVelocity = VehicleOwner->BoxCollider->GetPhysicsLinearVelocity();
	Velocity = VehicleOwner->BoxCollider->GetComponentVelocity();
	Acceleration = FMath::Lerp(Acceleration, TargetAcceleration, DeltaTime * VehicleOwner->VehicleData->AccelerationSpeed);
}

void UVehicleMovement::CalculateNormalizedSpeed()
{	
	const FVector Forward = VehicleOwner->ArrowComponent->GetForwardVector();
	const float ForwardSpeed = FVector::DotProduct(Forward, Velocity);
	
	float CarSpeed = CurrentSpeed > 0 ? ForwardSpeed : 0.01f; 

	NormalizedSpeed = FMath::Clamp(FMath::Abs(CarSpeed) / VehicleOwner->VehicleData->MaxSpeed, 0.f,1.f);
}

void UVehicleMovement::CalculateSteering(float DelaTime)
{
	float SteeringSpeed = VehicleOwner->VehicleData->SteeringSpeed;
	
	if (!bIsSteering)
	{
		SteeringSpeed *= 2.f;
	}

	Steering = FMath::FInterpTo(Steering, TargetSteering, DelaTime, SteeringSpeed);
}
#pragma endregion

#pragma region ApplyVariousForces
void UVehicleMovement::UpdateSuspension() const
{
	for (FWheelSetup& Wheel : VehicleOwner->Wheels)
	{
		USuspensionComponent* Suspension = Wheel.SuspensionComponent;
		
		if (!IsValid(Suspension))
		{
			continue;
		}
		
		const FVector WheelLocation = Suspension->GetComponentLocation();
		const FVector WheelUpVector = Suspension->GetUpVector();

		const FVector SuspensionForce = Suspension->CalculateSuspensionForce(VehicleOwner->BoxCollider,VehicleOwner->VehicleData, WheelLocation, WheelUpVector);
		VehicleOwner->BoxCollider->AddForceAtLocation(SuspensionForce, WheelLocation);
		DrawDebugDirectionalArrow(GetWorld(), WheelLocation, WheelLocation + SuspensionForce * 0.01f, 10.f, FColor::Cyan, false, 0.1f);
	}
}

void UVehicleMovement::ApplyFrictionForce()
{
	if (!IsValid(VehicleOwner->VehicleData->FrictionCurve))
	{
		return;
	}	
	
	if (VehicleOwner->Suspension_RL->GetIsGrounded())
	{
		const FVector RightVector = VehicleOwner->BoxCollider->GetRightVector();
		const float LateralSpeed = FVector::DotProduct(Velocity, RightVector);
		const float FrictionForce = VehicleOwner->VehicleData->FrictionCurve->GetFloatValue(NormalizedSpeed);
		
		const FVector Force = - RightVector * LateralSpeed * FrictionForce * VehicleOwner->VehicleData->GripFactor * VehicleOwner->VehicleData->GripMultiplier;
	
		VehicleOwner->BoxCollider->AddForce(Force, NAME_None, true);
		DrawDebugDirectionalArrow(GetWorld(), VehicleOwner->GetActorLocation(), VehicleOwner->GetActorLocation() + Force * 0.01f, 10.f, FColor::Red, false, 0.1f);
	}		
}


void UVehicleMovement::SetAngularDamping()
{
	if (!IsValid(VehicleOwner->VehicleData->AngularDampingCurve))
	{
		return;
	}
	
	float NewDamping = TargetBrake < 0.f ? 20.f : 50.f;	
	VehicleOwner->BoxCollider->SetAngularDamping(NewDamping);	
}
#pragma endregion

#pragma region ApplyInputForces
void UVehicleMovement::Turning()
{
	if (!IsValid(VehicleOwner->VehicleData->TurnCurve))
	{
		return;
	}
	
	if (VehicleOwner->Suspension_FL->GetIsGrounded())
	{
		const float Dir = TargetAcceleration >= 0.f ? 1.f : -1.f;
		const float TurnForce = VehicleOwner->VehicleData->TurnCurve->GetFloatValue(NormalizedSpeed) * Steering * VehicleOwner->VehicleData->SteeringTorque * Dir;
		VehicleOwner->BoxCollider->AddTorqueInRadians(FVector::UpVector * TurnForce, NAME_None, true);
		
		DrawDebugDirectionalArrow(GetWorld(), VehicleOwner->GetActorLocation(), VehicleOwner->GetActorLocation() + TurnForce, 10.f, FColor::Yellow, false, 0.1f);
	}
}

void UVehicleMovement::ApplyBrake()
{		
	for (FWheelSetup& Wheel : VehicleOwner->Wheels)
	{
		if (!Wheel.SuspensionComponent || !Wheel.SuspensionComponent->GetIsGrounded() || Steering != 0.f)
		{
			continue;
		}
		
		float Multiplier = VehicleOwner->VehicleData->BrakeMultiplierFrontWheel;
		
		if (Wheel.WheelPosition == EWheelPosition::RearLeft || Wheel.WheelPosition == EWheelPosition::RearRight)
		{
			Multiplier = VehicleOwner->VehicleData->BrakeMultiplierRearWheel;
		}
		
		FVector NormalizedLinearVelocity = LinearVelocity.GetSafeNormal();
		FVector Force = Multiplier * TargetBrake * NormalizedLinearVelocity * VehicleOwner->VehicleData->EngineBrakeForce;
				
		VehicleOwner->BoxCollider->AddForceAtLocation(Force, Wheel.SuspensionComponent->GetComponentLocation());
		DrawDebugDirectionalArrow(GetWorld(), Wheel.SuspensionComponent->GetComponentLocation(), Wheel.SuspensionComponent->GetComponentLocation() + Force * 0.01f, 10.f, FColor::Magenta, false, 0.1f);
	}
}

void UVehicleMovement::ApplyAcceleration()
{	
	if ((CurrentSpeed > VehicleOwner->VehicleData->MaxSpeed && TargetAcceleration > 0.f) || (CurrentSpeed > VehicleOwner->VehicleData->MaxReverseSpeed && TargetAcceleration < 0.f))
	{
		return;
	}
	
	for (FWheelSetup& Wheel : VehicleOwner->Wheels)
	{
		USuspensionComponent* Suspension = Wheel.SuspensionComponent;
		
		if (!IsValid(Suspension) || !Suspension->GetIsGrounded() || !IsValid(VehicleOwner->VehicleData->AccelerationCurve) 
			|| Wheel.WheelPosition == EWheelPosition::FrontLeft || Wheel.WheelPosition == EWheelPosition::FrontRight)
		{
			continue;
		}
	
		const FVector WheelForward = Suspension->GetForwardVector();
		const float AvailableTorque = VehicleOwner->VehicleData->AccelerationCurve->GetFloatValue(NormalizedSpeed);
		
		const FVector AccelForce = WheelForward * AvailableTorque * Acceleration * VehicleOwner->VehicleData->EngineForce;	
		const FVector WheelLocation = Suspension->GetComponentLocation();
		
		VehicleOwner->BoxCollider->AddForceAtLocation(AccelForce, WheelLocation);
		DrawDebugDirectionalArrow(GetWorld(), WheelLocation, WheelLocation + AccelForce * 0.01f, 10.f, FColor::Green, false, 0.1f);
	}
}
#pragma endregion

#pragma region InputSetters
void UVehicleMovement::SetTargetAcceleration(float Value)
{
	TargetAcceleration = Value;
}

void UVehicleMovement::SetTargetSteering(float Value, bool bIsSteeringInput)
{
	bIsSteering = bIsSteeringInput;	
	TargetSteering = Value;
}

void UVehicleMovement::SetTargetBrake(float Value, bool bIsBrakingInput)
{
	TargetBrake = Value;
	bIsBraking = bIsBrakingInput;
	AddSkidMark(bIsBraking);
	BrakeLights(bIsBrakingInput);
}
#pragma endregion

#pragma region Gravity
void UVehicleMovement::CustomGravity()
{
	const FVector GravityForce = FVector::DownVector * 981.f * VehicleOwner->BoxCollider->GetMass() * VehicleOwner->VehicleData->GravityScale;

	VehicleOwner->BoxCollider->AddForce(GravityForce);
	DrawDebugDirectionalArrow(GetWorld(), VehicleOwner->GetActorLocation(), VehicleOwner->GetActorLocation() + GravityForce * 0.01f, 10.f, FColor::Blue, false, 0.1f);
}
#pragma endregion


#pragma region WheelAnimation
void UVehicleMovement::RollingWheel()
{
	for (FWheelSetup& Wheel : VehicleOwner->Wheels)
	{
		if (!Wheel.PivotComponent || !Wheel.SuspensionComponent->GetIsGrounded())
		{
			continue;
		}
		
		const FVector WheelPivotLocation = Wheel.SuspensionComponent->GetComponentLocation();
		const FVector WheelPivotForward = Wheel.SuspensionComponent->GetForwardVector();
		const float RotationAngle = FVector::DotProduct(WheelPivotLocation - Wheel.PreviousPivotWheelLocation, WheelPivotForward);
		const FRotator DeltaRotation = FRotator(RotationAngle, 0.f, 0.f).GetInverse();
		Wheel.PivotComponent->AddLocalRotation(DeltaRotation);
		Wheel.PreviousPivotWheelLocation = WheelPivotLocation;		
	}		
}

void UVehicleMovement::SteeringWheel()
{
	for (FWheelSetup& Wheel : VehicleOwner->Wheels)
	{
		if (!Wheel.PivotComponent || !Wheel.SuspensionComponent->GetIsGrounded() || Wheel.WheelPosition == EWheelPosition::RearLeft|| Wheel.WheelPosition == EWheelPosition::RearRight)
		{
			continue;
		}
		
		const FRotator TargetRotation = FRotator(0.f,  Steering * VehicleOwner->VehicleData->MaxSteeringAngle, 0.f);
		Wheel.SuspensionComponent->SetRelativeRotation(TargetRotation);
	}
}
#pragma endregion

#pragma region SkidMarks
void UVehicleMovement::AddSkidMark(bool bActivate)
{
	for (TObjectPtr SkidMarkEffect : VehicleOwner->SkidMarkEffects)
	{
		if (IsValid(SkidMarkEffect))
		{
			bActivate ? SkidMarkEffect->Activate() : SkidMarkEffect->Deactivate();
		}
	}
}
#pragma endregion

#pragma region Sound
void UVehicleMovement::SoundMotorPitch()
{
	VehicleOwner->EngineSoundEffect->SetPitchMultiplier(1.f + 2.f * NormalizedSpeed);
}

#pragma endregion

#pragma region Light
void UVehicleMovement::BrakeLights(bool bActivate)
{
	for (TObjectPtr RearLight : VehicleOwner->RearLights)
	{
		if (IsValid(RearLight))
		{
			RearLight->SetVisibility(bActivate);
		}
	}
	
}
#pragma endregion

