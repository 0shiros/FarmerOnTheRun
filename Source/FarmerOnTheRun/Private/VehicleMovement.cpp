// Fill out your copyright notice in the Description page of Project Settings.


#include "VehicleMovement.h"
#include "PlayerVehicle.h"

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

void UVehicleMovement::MoveForward(float Value)
{
}

void UVehicleMovement::MoveRight(float Value)
{
}

