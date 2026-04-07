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

	ApplyLateralFriction(DeltaTime);

}

void UVehicleMovement::Accelerate(float Value)
{			
	if (!IsGrounded()) return; 
    
	FVector Force = VehicleOwner->ArrowComponent->GetForwardVector()
				  * Value
				  * VehicleOwner->VehicleStats.AccelerationForce;
	VehicleOwner->BoxCollisionComponent->AddForce(Force, NAME_None, true);
}

void UVehicleMovement::Reverse(float Value)
{
	if (!IsGrounded()) return;
	
	FVector Force = VehicleOwner->ArrowComponent->GetForwardVector()
				  * Value
				  * VehicleOwner->VehicleStats.AccelerationForce;
	VehicleOwner->BoxCollisionComponent->AddForce(Force, NAME_None, true);
}

void UVehicleMovement::TurnLeftRight(float Value)
{
	if (!IsGrounded()) return;
    
	FVector Torque = VehicleOwner->ArrowComponent->GetUpVector()
				   * Value
				   * VehicleOwner->VehicleStats.TuningForce;
	VehicleOwner->BoxCollisionComponent->AddTorqueInRadians(Torque, NAME_None, true);
}

void UVehicleMovement::ApplyLateralFriction(float DeltaTime)
{
	int32 GroundedCount = 0;
	
	for (TObjectPtr Wheel : VehicleOwner->Wheels)
		if (Wheel->GetResults().IsGrounded) GroundedCount++;

	if (GroundedCount == 0) return;

	FVector RightVec  = VehicleOwner->ArrowComponent->GetRightVector();
	FVector Velocity  = VehicleOwner->BoxCollisionComponent->GetPhysicsLinearVelocity();
	float   LateralSpeed = FVector::DotProduct(Velocity, RightVec);

	FVector FrictionForce = -RightVec * LateralSpeed * VehicleOwner->VehicleStats.LateralFriction;
	VehicleOwner->BoxCollisionComponent->AddForce(FrictionForce, NAME_None, false);
}

bool UVehicleMovement::IsGrounded() const
{
	for (TObjectPtr Wheel : VehicleOwner->Wheels)
		if (Wheel->GetResults().IsGrounded) return true;
	return false;
}
