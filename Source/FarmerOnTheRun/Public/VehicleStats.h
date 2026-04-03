// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VehicleStats.generated.h"

USTRUCT(BlueprintType)
struct FVehicleStats
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	FVector2D SuspensionAmplitudes;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	float SuspensionStrength;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	TObjectPtr<UCurveFloat> SuspensionResponseCurve;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	float AccelerationForce;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	float TuningForce;
};
