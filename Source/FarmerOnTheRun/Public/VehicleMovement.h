// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VehicleMovement.generated.h"


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
	
	void MoveForward(float Value);
	
	void MoveRight(float Value);
};
