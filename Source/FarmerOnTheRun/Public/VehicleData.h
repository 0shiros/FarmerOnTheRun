// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "VehicleData.generated.h"

/**
 * 
 */
UCLASS()
class FARMERONTHERUN_API UVehicleData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category="Engine")
	float EngineForce = 8000.f;
	
	UPROPERTY(EditAnywhere, Category="Suspension")
	float SpringStrength = 25000.f;

	UPROPERTY(EditAnywhere, Category="Suspension")
	float SpringDamping = 6000.f;

	UPROPERTY(EditAnywhere, Category="Suspension")
	float RestDist = 40.f;
	
	UPROPERTY(EditAnywhere, Category="Suspension")
	float WheelRadius = 30.f;
};
