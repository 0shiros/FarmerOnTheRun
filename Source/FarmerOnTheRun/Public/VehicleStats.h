// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VehicleStats.generated.h"

USTRUCT(BlueprintType)
struct FVehicleStats
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Suspension")
	float SuspensionRestDistance = 8.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Suspension")
	float SuspensionSpringForce = 1000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Suspension")
	float SuspensionSpringDamping = 500.f;
			
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float AccelerationForce = 3000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float DecelerationForce = 1500.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	TObjectPtr<UCurveFloat> AccelerationTorqueCurve;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float TopSpeed = 6000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float ReverseTopSpeed = 3000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float TurnTorque = 7.5f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float DefaultAngularDamping = 1.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float DriftingAngularDamping = 0.2f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    bool IsDrifting;
};
