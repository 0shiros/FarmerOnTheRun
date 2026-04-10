// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SuspensionStats.h"
#include "Components/SceneComponent.h"
#include "SuspensionComponent.generated.h"

class APlayerVehicle;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FARMERONTHERUN_API USuspensionComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USuspensionComponent();
			
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSuspensionStats SuspensionStats;
	
	FHitResult OutHit;
	FCollisionQueryParams Query;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void Initialize( float NewRestDist, float NewSpringForce, float NewSpringDamping);
		
	const FHitResult& GetResults() const;
	
	void PerformTrace();
	
	void OnTraceCompleted();
	
	FVector CalculateSuspension(APlayerVehicle* VehiclePlayer, float OutDistance);
	
	bool GetIsGrounded() const;
};
