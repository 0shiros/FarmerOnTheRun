// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GamePlayerController.generated.h"

UCLASS(Abstract)
class FARMERONTHERUN_API AGamePlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	TObjectPtr<class APlayerVehicule> PlayerVehicule;
	
protected: 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = IMC)
	TObjectPtr<class UInputMappingContext> CharacterIMC;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<class UInputAction> AccelerationAction;
		
private:
	virtual void SetupInputComponent() override;
	
public:
	void Start();
	
	UFUNCTION()
	void OnAcceleration(const FInputActionValue& Value);	
};
