// Fill out your copyright notice in the Description page of Project Settings.

#include "VehicleMovement.h"
#include "PlayerVehicle.h"
#include "SuspensionComponent.h"
#include "VehicleData.h"
#include "WheelSetup.h"
#include "Components/ArrowComponent.h"

UVehicleMovement::UVehicleMovement()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UVehicleMovement::BeginPlay()
{
	Super::BeginPlay();

	PrimaryComponentTick.TickGroup = TG_PostPhysics;
	VehicleOwner = Cast<APlayerVehicle>(GetOwner());

	if (!IsValid(VehicleOwner))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,TEXT("VehicleMovementComponent is not attached to a valid APlayerVehicle!"));
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
}

void UVehicleMovement::SetVariablesToFrame(float DeltaTime)
{
	CurrentSpeed = FMath::Abs(Velocity.Size());
	CalculateNormalizedSpeed();
	CalculateSteering();
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

void UVehicleMovement::CalculateSteering()
{
	float SteeringSpeed = VehicleOwner->VehicleData->SteeringSpeed;
	
	if (!bIsSteering)
	{
		SteeringSpeed *= 2.f;
	}

	Steering = FMath::FInterpTo(Steering, TargetSteering, GetWorld()->GetDeltaSeconds(), SteeringSpeed);
}

void UVehicleMovement::Turning()
{
	if (!IsValid(VehicleOwner->VehicleData->TurnCurve))
	{
		return;
	}
	
	if (VehicleOwner->Suspension_FL->GetIsGrounded())
	{
		const float TurnTorque = VehicleOwner->VehicleData->TurnCurve->GetFloatValue(NormalizedSpeed) * Steering * VehicleOwner->VehicleData->SteeringTorque;
			
		const FVector TurnForce = FVector(0.f, 0.f, TurnTorque * TargetAcceleration);

		VehicleOwner->BoxCollider->AddTorqueInDegrees(TurnForce, NAME_None, true);
		
		DrawDebugDirectionalArrow(GetWorld(), VehicleOwner->GetActorLocation(), VehicleOwner->GetActorLocation() + TurnForce * 0.01f, 10.f, FColor::Yellow, false, 0.1f);
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
	
	const float SpinFactor = (Acceleration > 0.8f && NormalizedSpeed < 0.3f) ? 0.2f : 1.f;	
	const float CurveDamping = VehicleOwner->VehicleData->AngularDampingCurve->GetFloatValue(NormalizedSpeed);
	const float NewDamping = CurveDamping * SpinFactor;
	const float Damping = VehicleOwner->Suspension_RR->GetIsGrounded() ? NewDamping : 0.05f;
	
	VehicleOwner->BoxCollider->SetAngularDamping(Damping);
	DrawDebugDirectionalArrow(GetWorld(), VehicleOwner->GetActorLocation(), VehicleOwner->GetActorLocation() + VehicleOwner->BoxCollider->GetPhysicsAngularVelocityInDegrees() * 0.01f, 10.f, FColor::Purple, false, 0.1f);
}

void UVehicleMovement::ApplyBrake()
{	
	for (FWheelSetup& Wheel : VehicleOwner->Wheels)
	{
		if (!Wheel.SuspensionComponent || !Wheel.SuspensionComponent->GetIsGrounded())
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
		
		UE_LOG(LogTemp, Warning, TEXT("Brake Force: %s "), *Force.ToString());
		
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
		
		if (!IsValid(Suspension) || !Suspension->GetIsGrounded() || !IsValid(VehicleOwner->VehicleData->AccelerationCurve) || Wheel.WheelPosition == EWheelPosition::FrontLeft || Wheel.WheelPosition == EWheelPosition::FrontRight)
		{
			continue;
		}
	
		const FVector WheelForward = Suspension->GetForwardVector();
		const float AvailableTorque = VehicleOwner->VehicleData->AccelerationCurve->GetFloatValue(NormalizedSpeed);
		
		const FVector AccelForce = WheelForward * AvailableTorque * Acceleration * VehicleOwner->VehicleData->EngineForce;	
		//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Cyan, FString::Printf(TEXT("AccelForce: %s | AvailableTorque: %f | Acceleration: %f | EngineForce: %f"), *AccelForce.ToString(), AvailableTorque, Acceleration, VehicleOwner->VehicleData->EngineForce));
		const FVector WheelLocation = Suspension->GetComponentLocation();
		
		VehicleOwner->BoxCollider->AddForceAtLocation(AccelForce, WheelLocation);
		DrawDebugDirectionalArrow(GetWorld(), WheelLocation, WheelLocation + AccelForce * 0.01f, 10.f, FColor::Green, false, 0.1f);
	}
}

void UVehicleMovement::SetTargetAcceleration(float Value)
{
	TargetAcceleration = Value;
}

void UVehicleMovement::SetTargetSteering(float Value, bool bIsSteeringInput)
{
	bIsSteering = bIsSteeringInput;	
	TargetSteering = Value;
}

void UVehicleMovement::SetTargetBrake(float Value)
{
	TargetBrake = Value;
}

void UVehicleMovement::CustomGravity()
{
	const FVector GravityForce = FVector::DownVector * 981.f * VehicleOwner->BoxCollider->GetMass() * VehicleOwner->VehicleData->GravityScale;

	VehicleOwner->BoxCollider->AddForce(GravityForce);
	DrawDebugDirectionalArrow(GetWorld(), VehicleOwner->GetActorLocation(), VehicleOwner->GetActorLocation() + GravityForce * 0.01f, 10.f, FColor::Blue, false, 0.1f);
}

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
		
		GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Yellow, FString::Printf(TEXT("Steering: %f"), Steering));
		const FRotator TargetRotation = FRotator(0.f,  Steering * VehicleOwner->VehicleData->MaxSteeringAngle, 0.f);
		GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Yellow, FString::Printf(TEXT("TargetRotation: %s"), *TargetRotation.ToString()));
		Wheel.SuspensionComponent->SetRelativeRotation(TargetRotation);
	}
}
