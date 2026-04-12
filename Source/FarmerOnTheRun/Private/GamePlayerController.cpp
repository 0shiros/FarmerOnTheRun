// Fill out your copyright notice in the Description page of Project Settings.


#include "GamePlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PlayerVehicle.h"
#include "VehicleMovement.h"

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
	if (TObjectPtr<ULocalPlayer> LocalPlayer = GetLocalPlayer())
	{
		if (TObjectPtr<UEnhancedInputLocalPlayerSubsystem> Subsystem =	LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
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
	
	if (TObjectPtr<UEnhancedInputComponent> EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		// Bind Movement Actions
		EnhancedInputComponent->BindAction(AccelerationAction, ETriggerEvent::Triggered, this, &AGamePlayerController::AccelerationTriggered);
		EnhancedInputComponent->BindAction(AccelerationAction, ETriggerEvent::Completed, this, &AGamePlayerController::AccelerationTriggered);
		EnhancedInputComponent->BindAction(ReverseAction, ETriggerEvent::Triggered, this, &AGamePlayerController::BrakeTriggered);
		EnhancedInputComponent->BindAction(ReverseAction, ETriggerEvent::Completed, this, &AGamePlayerController::BrakeTriggered);
		EnhancedInputComponent->BindAction(TurnLeftRightAction, ETriggerEvent::Triggered, this, &AGamePlayerController::SteeringTriggered);
		EnhancedInputComponent->BindAction(TurnLeftRightAction, ETriggerEvent::Completed, this, &AGamePlayerController::SteeringCompleted);
		EnhancedInputComponent->BindAction(DriftAction, ETriggerEvent::Started, this, &AGamePlayerController::OnDriftStarted);
		EnhancedInputComponent->BindAction(DriftAction, ETriggerEvent::Completed, this, &AGamePlayerController::OnDriftCompleted);
	}
}

void AGamePlayerController::AccelerationTriggered(const FInputActionValue& Value)
{
	PlayerVehicule->VehicleMovementComponent->SetTargetAcceleration(Value.Get<float>());
}

void AGamePlayerController::AccelerationCompleted(const FInputActionValue& Value)
{
	PlayerVehicule->VehicleMovementComponent->SetTargetAcceleration(0.f);
}


void AGamePlayerController::BrakeTriggered(const FInputActionValue& Value)
{
	PlayerVehicule->VehicleMovementComponent->SetTargetBrake(Value.Get<float>());
}

void AGamePlayerController::BrakeCompleted(const FInputActionValue& Value)
{
	PlayerVehicule->VehicleMovementComponent->SetTargetBrake(0.f);
}

void AGamePlayerController::SteeringTriggered(const FInputActionValue& Value)
{
	PlayerVehicule->VehicleMovementComponent->SetTargetSteering(Value.Get<float>(), true);	
}

void AGamePlayerController::SteeringCompleted(const FInputActionValue& Value)
{
	PlayerVehicule->VehicleMovementComponent->SetTargetSteering(0,false);
}

void AGamePlayerController::OnDriftStarted(const FInputActionValue& Value)
{
	
}

void AGamePlayerController::OnDriftCompleted(const FInputActionValue& Value)
{
	
}
