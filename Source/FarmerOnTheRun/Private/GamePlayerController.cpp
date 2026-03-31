// Fill out your copyright notice in the Description page of Project Settings.


#include "GamePlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

void AGamePlayerController::Start()
{
	if (!CharacterIMC)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Character Input Mapping Context Not Set"));
		return;
	}
	
	// Set the input mode to Game Only to ensure the player can control the character
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
	
	// Add the input mapping context to the local player subsystem
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =	LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			Subsystem->AddMappingContext(CharacterIMC, 0);
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Local Player Not Valid"));
	}
}

void AGamePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		// Bind Movement Actions
		EnhancedInputComponent->BindAction(AccelerationAction, ETriggerEvent::Triggered, this, &AGamePlayerController::OnAcceleration);
	}
}

void AGamePlayerController::OnAcceleration(const FInputActionValue& Value)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Acceleration: %f"), Value.Get<float>()));
}
