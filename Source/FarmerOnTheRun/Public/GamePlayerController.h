// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GamePlayerController.generated.h"

struct FInputActionValue;
class UInputAction;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPause);

UCLASS(Abstract)
class FARMERONTHERUN_API AGamePlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	TObjectPtr<class APlayerVehicle> PlayerVehicule;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = IMC)
	TObjectPtr<class UInputMappingContext> CharacterIMC;
	
protected: 
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> AccelerationAction;
		
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> BrakeAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> TurnLeftRightAction;
		
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> PauseAction;
	
	UPROPERTY(VisibleAnywhere, BlueprintAssignable, Category = "Pause")
	FOnPause OnPauseDelegate;
	
private:
	
	virtual void BeginPlay() override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual void SetupInputComponent() override;
	
public:
	
	void AccelerationTriggered(const FInputActionValue& Value);	
	
	void AccelerationCompleted(const FInputActionValue& Value);
	
	void BrakeTriggered(const FInputActionValue& Value);
	
	void BrakeCompleted(const FInputActionValue& Value);
	
	void SteeringTriggered(const FInputActionValue& Value);
	
	void SteeringCompleted(const FInputActionValue& Value);
	
	void PauseTriggered(const FInputActionValue& Value);
};
