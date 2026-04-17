// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GhostVehicle.generated.h"

UCLASS(Abstract)
class FARMERONTHERUN_API AGhostVehicle : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGhostVehicle();
};
