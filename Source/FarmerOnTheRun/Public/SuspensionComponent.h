// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "SuspensionComponent.generated.h"

class UVehicleData;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FARMERONTHERUN_API USuspensionComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USuspensionComponent();
			
protected:
	
	FHitResult SuspensionHit;	
	FCollisionQueryParams SuspensionQueryParams;	
	
	bool bIsGrounded = false;
		
protected:

	virtual void BeginPlay() override;
	
public :
		
	void PerformSuspensionTrace(const UVehicleData* VehicleData, const FVector& WheelLocation, const FVector& WheelUpVector);
	
	FVector CalculateSuspensionForce(UBoxComponent* BoxComponent, const UVehicleData* VehicleData, const FVector& WheelLocation, const FVector& WheelUpVector);	
};

