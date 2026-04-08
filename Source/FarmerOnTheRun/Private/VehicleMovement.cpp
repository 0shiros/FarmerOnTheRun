// Fill out your copyright notice in the Description page of Project Settings.


#include "VehicleMovement.h"
#include "PlayerVehicle.h"
#include "SuspensionComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"

// Sets default values for this component's properties
UVehicleMovement::UVehicleMovement()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UVehicleMovement::BeginPlay()
{
	Super::BeginPlay();

	VehicleOwner = Cast<APlayerVehicle>(GetOwner());
	
	if (!VehicleOwner)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("UVehicleMovement::BeginPlay failed. Owner is not APlayerVehicle."));
	}	
}


// Called every frame
void UVehicleMovement::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	LateralSlipping();
	SetRepulsionForce();
	ShowVehicleSpeed();
}

void UVehicleMovement::ApplyDriveForce(float Value, float TopSpeed, float DriveForce, UCurveFloat* TorqueCurve)
{
	if (!IsGrounded() || TorqueCurve == nullptr) return;
    
	float ForwardSpeed = FVector::DotProduct(VehicleOwner->BoxCollisionComponent->GetPhysicsLinearVelocity(),VehicleOwner->ArrowComponent->GetForwardVector());
	
	if (FMath::Abs(ForwardSpeed) <= TopSpeed)
	{
		float SpeedRatio = FMath::Clamp(FMath::Abs(ForwardSpeed) / TopSpeed, 0.f, 1.f);
		SpeedRatio = TorqueCurve->GetFloatValue(SpeedRatio);
		FVector Force = SpeedRatio * Value * DriveForce * VehicleOwner->ArrowComponent->GetForwardVector();
		
		VehicleOwner->BoxCollisionComponent->AddForce(Force, NAME_None, true);
	}
}

void UVehicleMovement::Accelerate(float Value) 
{ 
	if (!IsGrounded() || !VehicleOwner) return;
	
	ApplyDriveForce(Value, VehicleOwner->VehicleStats.TopSpeed, VehicleOwner->VehicleStats.AccelerationForce, VehicleOwner->VehicleStats.AccelerationTorqueCurve); 
}
void UVehicleMovement::Reverse(float Value)    
{ 
	if (!IsGrounded() || !VehicleOwner) return;

	ApplyDriveForce(Value, VehicleOwner->VehicleStats.ReverseTopSpeed, VehicleOwner->VehicleStats.DecelerationForce, VehicleOwner->VehicleStats.ReverseTorqueCurve); 
}

void UVehicleMovement::TurnLeftRight(float Value)
{
	if (!IsGrounded() || !VehicleOwner || VehicleOwner->CurrentSpeed == 0) return;
		
	FVector Torque = FVector(0.f, 0.f, Value * VehicleOwner->VehicleStats.TurnTorque);
	
	VehicleOwner->BoxCollisionComponent->AddTorqueInRadians(Torque, NAME_None, true);
}

void UVehicleMovement::LateralSlipping()
{
	if (!IsGrounded() || !VehicleOwner) return;	
	
	for (TObjectPtr Wheel : VehicleOwner->Wheels)
	{
		if (!Wheel->GetIsGrounded()) continue;
		
		float VelocityDot = FVector::DotProduct(Wheel->GetRightVector(), VehicleOwner->BoxCollisionComponent->GetPhysicsLinearVelocityAtPoint(Wheel->GetComponentLocation()));
		float CurrentGripFactor = VehicleOwner->VehicleStats.IsDrifting ? VehicleOwner->VehicleStats.DriftingGripFactor : VehicleOwner->VehicleStats.DefaultGripFactor;
		FVector LateralForce = -(VelocityDot * Wheel->GetRightVector()) * CurrentGripFactor;

		VehicleOwner->BoxCollisionComponent->AddForceAtLocation(LateralForce, Wheel->GetComponentLocation(), NAME_None);
	}
}

void UVehicleMovement::SetRepulsionForce()
{	
	if (!VehicleOwner) return;
	
	for (TObjectPtr Wheel : VehicleOwner->Wheels)
	{
		FVector Force = Wheel->CalculateSuspension(Wheel->GetResults().Distance);
		FVector Location = Wheel->GetComponentLocation();	
		
		VehicleOwner->BoxCollisionComponent->AddForceAtLocation(Force, Location);
	}
}

void UVehicleMovement::Drift(bool bIsDrifting)
{
	if (!IsGrounded() || !VehicleOwner) return;
	
	VehicleOwner->VehicleStats.IsDrifting = bIsDrifting;	
}

bool UVehicleMovement::IsGrounded() const
{
	if (!VehicleOwner) return false;
	
	for (TObjectPtr Wheel : VehicleOwner->Wheels)
		if (Wheel->GetIsGrounded()) return true;
	return false;
}

void UVehicleMovement::ShowVehicleSpeed() const
{	
	if (!VehicleOwner) return;
	
	VehicleOwner->CurrentSpeed = FMath::Abs(FMath::RoundToInt(FVector::DotProduct(VehicleOwner->BoxCollisionComponent->GetPhysicsLinearVelocity(), VehicleOwner->ArrowComponent->GetForwardVector()) * 0.036f));
}
