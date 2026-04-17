// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VehicleMovement.generated.h"

class UBoxComponent;
class UVehicleData;
class USuspensionComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpeedUpdate, float, CurrentSpeed);
DECLARE_DELEGATE_OneParam(FOnTransformUpdate, const FTransform&);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FARMERONTHERUN_API UVehicleMovement : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UVehicleMovement();		
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="VehicleOwner")
	TObjectPtr<class APlayerVehicle> VehicleOwner;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="CheckStart")	
	TObjectPtr<class ACheckStart> CheckStart;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="CheckGoal")	
	TObjectPtr<class ACheckGoal> CheckGoal;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="VehicleOwner")
	bool bIsBraking;
	
	FOnTransformUpdate OnTransformUpdate;	
	
	bool bBeginSendingInput = false;
	
private : 
	
	UPROPERTY(VisibleAnywhere, BlueprintAssignable, Category = "Speed")
	FOnSpeedUpdate OnSpeedUpdate;
	
	
	// Input variables
	float TargetAcceleration;
	float TargetSteering;
	bool bIsSteering;
	float TargetBrake;	
	
	
	// Calculated variables
	float Acceleration;
	float Steering;
	float NormalizedSpeed;
	FVector Velocity;
	FVector LinearVelocity;
	float CurrentSpeed;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void SetBeginSendingInput();
	
	void SetEndSendingInput();
	
	void SetVariablesToFrame(float DeltaTime);
	
	void CalculateNormalizedSpeed();
	
	void UpdateSuspension() const;
	
	void ApplyFrictionForce();	
	
	void ApplyBrake();
		
	void ApplyAcceleration();
	
	void Turning();
		
	void SetAngularDamping();	
	
	void CalculateSteering(float DelaTime);	
	
	void SetTargetAcceleration(float Value);
	
	void SetTargetSteering(float Value, bool bIsSteeringInput);	
	
	void SetTargetBrake(float Value, bool bIsBrakingInput);
	
	void CustomGravity();
	
	UFUNCTION(BlueprintCallable)
	float GetCurrentSpeed() const { return CurrentSpeed; }
	
	void RollingWheel();
	
	void SteeringWheel();
	
	void AddSkidMark(bool bActivate);
	
	void SoundMotorPitch();
	
	void BrakeLights(bool bActivate);
};
