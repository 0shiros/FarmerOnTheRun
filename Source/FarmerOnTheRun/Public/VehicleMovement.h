// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VehicleMovement.generated.h"

class UBoxComponent;
class UVehicleData;
class USuspensionComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FARMERONTHERUN_API UVehicleMovement : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UVehicleMovement();		

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	
	FVector CalculateSteering(const FVector& WheelPosition, const FVector& WheelRightVector, UVehicleData* VehicleData, UBoxComponent* BoxComponent, float DeltaTime);	

	FVector CalculateAcceleration(FVector WheelForwardVector, FVector VehicleVelocity, UVehicleData* VehicleData, FVector VehicleForwardVector, float ForwardInput);
};
