// Fill out your copyright notice in the Description page of Project Settings.


#include "VehicleMovement.h"

#include "SuspensionComponent.h"
#include "VehicleData.h"

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
}

FVector UVehicleMovement::CalculateSteering(const FVector& WheelPosition, const FVector& WheelRightVector, UVehicleData* VehicleData, UBoxComponent* BoxComponent, float DeltaTime)
{
	if (!IsValid(VehicleData) || !IsValid(BoxComponent))
	{
		return FVector::ZeroVector;
	}
	
	FVector SteeringDirection = WheelRightVector;
	FVector TireWorldVelocity = BoxComponent->GetPhysicsLinearVelocityAtPoint(WheelPosition);
	
	float SteeringVelocity = FVector::DotProduct(SteeringDirection, TireWorldVelocity);
	float DesiredVelChange = - SteeringVelocity * VehicleData->GripFactor;
	
	FVector ForceToApply = SteeringDirection * DesiredVelChange * BoxComponent->GetMass();
	
	return ForceToApply;
}

int LastForwardSpeed;

FVector UVehicleMovement::CalculateAcceleration(FVector WheelForwardVector, FVector VehicleVelocity, UVehicleData* VehicleData, FVector VehicleForwardVector, float ForwardInput)
{
	if (!IsValid(VehicleData))
	{
		return FVector::ZeroVector;
	} 
	
	float CurrentSpeed = FVector::DotProduct(VehicleForwardVector, VehicleVelocity); 
	float NormalizeSpeed = FMath::Clamp(FMath::Abs(CurrentSpeed) / VehicleData->MaxSpeed, 0.f, 1.f); 
	float AvailableTorque = VehicleData->AccelerationCurve->GetFloatValue(NormalizeSpeed) * ForwardInput; 
	FVector ForceToApply = WheelForwardVector * AvailableTorque; 
	return ForceToApply;
}

