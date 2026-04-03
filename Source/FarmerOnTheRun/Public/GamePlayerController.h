// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GamePlayerController.generated.h"

struct FInputActionValue;
class UInputAction;

UCLASS(Abstract)
class FARMERONTHERUN_API AGamePlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	TObjectPtr<class APlayerVehicle> PlayerVehicule;
	
protected: 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = IMC)
	TObjectPtr<class UInputMappingContext> CharacterIMC;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> AccelerationAction;
		
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> TurnLeftRightAction;
	
private:
	virtual void SetupInputComponent() override;
	
public:
	void Start();
	
	void OnAcceleration(const FInputActionValue& Value);	
	
	void OnTurnLeftRight(const FInputActionValue& Value);
};
