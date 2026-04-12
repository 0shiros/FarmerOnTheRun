// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VehicleMovement.h"
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
	float AccelerationSpeed = 4.5f;
	
	UPROPERTY(EditAnywhere, Category="Engine")
	float EngineForce = 900000.f;
		
	UPROPERTY(EditAnywhere, Category="Engine")
	float MaxSpeed = 9000.f;
	
	UPROPERTY(EditAnywhere, Category="Engine")
	TObjectPtr<UCurveFloat> AccelerationCurve;
	
	UPROPERTY(EditAnywhere, Category="Engine")
	float MaxReverseSpeed = 3000.f;
	
	UPROPERTY(EditAnywhere, Category="Brake")	
	float BrakeMultiplierFrontWheel = 0.3f;
	
	UPROPERTY(EditAnywhere, Category="Brake")
	float BrakeMultiplierRearWheel = 2.0f;
	
	UPROPERTY(EditAnywhere, Category="Brake")
	float EngineBrakeForce = 200000.f;
	
	UPROPERTY(EditAnywhere, Category="Grip")
	float GripFactor = 1.f;
	
	UPROPERTY(EditAnywhere, Category="Grip")
	TObjectPtr<UCurveFloat> FrictionCurve;
		
	UPROPERTY(EditAnywhere, Category="Grip")
	float GripMultiplier = -1.5f;
	
	UPROPERTY(EditAnywhere, Category="Suspension")
	float SpringStrength = 40000.f;

	UPROPERTY(EditAnywhere, Category="Suspension")
	float SpringDamping = 6000.f;

	UPROPERTY(EditAnywhere, Category="Suspension")
	float RestDist = 36.f;
	
	UPROPERTY(EditAnywhere, Category="Suspension")
	float MaxSuspensionForce = 200000.f;
	
	UPROPERTY(EditAnywhere, Category="Steering")
	TObjectPtr<UCurveFloat> TurnCurve;
	
	UPROPERTY(EditAnywhere, Category="Steering")
    float SteeringTorque = 300.f;
	
	UPROPERTY(EditAnywhere, Category="Steering")
	float SteeringSpeed = 6.f;
	
	UPROPERTY(EditAnywhere, Category="AngularDamping")
	TObjectPtr<UCurveFloat> AngularDampingCurve;
	
	UPROPERTY(EditAnywhere, Category="Gravity")
	float GravityScale = 2.f;
	
	UPROPERTY(EditAnywhere, Category="Wheel")
	float WheelRadius = 30.f;
};
