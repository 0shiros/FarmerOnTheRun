// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheelSetup.generated.h"

UENUM(BlueprintType)
enum class EWheelPosition : uint8
{
	FrontLeft  = 0,
	FrontRight = 1,
	RearLeft   = 2,
	RearRight  = 3
};

USTRUCT(BlueprintType)
struct FWheelSetup 
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)	
	EWheelPosition WheelPosition;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> WheelMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class USuspensionComponent> SuspensionComponent;	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> PivotComponent;		
	
	FVector PreviousPivotWheelLocation;
};

