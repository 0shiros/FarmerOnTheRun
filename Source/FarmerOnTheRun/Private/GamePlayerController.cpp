// Fill out your copyright notice in the Description page of Project Settings.


#include "GamePlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PlayerVehicle.h"
#include "VehicleMovement.h"

void AGamePlayerController::BeginPlay()
{
	Super::BeginPlay();
		
	if (!CharacterIMC)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Character Input Mapping Context Not Set"));
		return;
	}
	
	PlayerVehicule = Cast<APlayerVehicle>(GetPawn());
	
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
	
	if (TObjectPtr<ULocalPlayer> LocalPlayer = GetLocalPlayer())
	{		
		if (TObjectPtr<UEnhancedInputLocalPlayerSubsystem> Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			Subsystem->AddMappingContext(CharacterIMC, 0);
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Local Player Not Valid"));
	}
	
	PauseAction->bTriggerWhenPaused = true;
}

void AGamePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	if (TObjectPtr<UEnhancedInputComponent> EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		// Bind Movement Actions
		EnhancedInputComponent->BindAction(AccelerationAction, ETriggerEvent::Triggered, this, &AGamePlayerController::AccelerationTriggered);
		EnhancedInputComponent->BindAction(AccelerationAction, ETriggerEvent::Completed, this, &AGamePlayerController::AccelerationCompleted);
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Started, this, &AGamePlayerController::BrakeTriggered);
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Completed, this, &AGamePlayerController::BrakeCompleted);
		EnhancedInputComponent->BindAction(TurnLeftRightAction, ETriggerEvent::Triggered, this, &AGamePlayerController::SteeringTriggered);
		EnhancedInputComponent->BindAction(TurnLeftRightAction, ETriggerEvent::Completed, this, &AGamePlayerController::SteeringCompleted);
		
		// Bind Pause Action
		EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Started, this, &AGamePlayerController::PauseTriggered);
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
	PlayerVehicule->VehicleMovementComponent->SetTargetBrake(Value.Get<float>(), true);
}

void AGamePlayerController::BrakeCompleted(const FInputActionValue& Value)
{
	PlayerVehicule->VehicleMovementComponent->SetTargetBrake(0.f, false);
}

void AGamePlayerController::SteeringTriggered(const FInputActionValue& Value)
{
	PlayerVehicule->VehicleMovementComponent->SetTargetSteering(Value.Get<float>(), true);	
}

void AGamePlayerController::SteeringCompleted(const FInputActionValue& Value)
{
	PlayerVehicule->VehicleMovementComponent->SetTargetSteering(0,false);
}

void AGamePlayerController::PauseTriggered(const FInputActionValue& Value)
{
	OnPauseDelegate.Broadcast();
}