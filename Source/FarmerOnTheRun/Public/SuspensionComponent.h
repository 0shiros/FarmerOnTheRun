// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SuspensionResult.h"
#include "Components/SceneComponent.h"
#include "SuspensionComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FARMERONTHERUN_API USuspensionComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USuspensionComponent();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UPrimitiveComponent> UpdatedComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D Amplitudes;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Strength;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UCurveFloat> ResponseCurve;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSuspensionResult Results;
	
protected:
	
	FTraceDelegate* Handle;
	FCollisionQueryParams Query;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void Initialize(UPrimitiveComponent* InUpdatedComponent, const FVector2D& InAmplitudes, const float InStrength, UCurveFloat* InResponseCurve);
	
	FSuspensionResult GetResults();
	
	void PerformTrace();
	
	void OnTraceCompleted(const FTraceHandle& CurrentHandle, FTraceDatum& Data);
};
