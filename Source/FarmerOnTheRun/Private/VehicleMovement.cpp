// Fill out your copyright notice in the Description page of Project Settings.


#include "VehicleMovement.h"
#include "PlayerVehicle.h"
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

	// ...
}

void UVehicleMovement::Accelerate(float Value)
{		
	FVector Force = VehicleOwner->ArrowComponent->GetForwardVector() * Value * VehicleOwner->VehicleStats.AccelerationForce;
	VehicleOwner->BoxCollisionComponent->AddForce(Force, NAME_None, true);
}

void UVehicleMovement::TurnLeftRight(float Value)
{
	FVector Torque = VehicleOwner->ArrowComponent->GetUpVector() * Value * VehicleOwner->VehicleStats.TuningForce; // Adjust the multiplier as needed for turning sensitivity
	VehicleOwner->BoxCollisionComponent->AddTorqueInRadians(Torque, NAME_None, true);
}
