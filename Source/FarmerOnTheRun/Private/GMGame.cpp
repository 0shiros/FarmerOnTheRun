// Fill out your copyright notice in the Description page of Project Settings.


#include "GMGame.h"
#include "GamePlayerController.h"

AGMGame::AGMGame()
{
	PlayerControllerClass = AGamePlayerController::StaticClass();
	DefaultPawnClass = nullptr;
}

void AGMGame::BeginPlay()
{
	Super::BeginPlay();
	
	UWorld* World = GetWorld();
	
	AGamePlayerController* MyPlayerController = World->GetFirstPlayerController<AGamePlayerController>();
	
	if (!MyPlayerController)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Failed to get MyPlayerController!"));
		return;
	}
	
	if (IsValid(World))
	{
		if (!PlayerSpawnPoint)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("PlayerSpawnPoint is not set!"));
			return;
		}
		
		FVector Location = PlayerSpawnPoint->GetActorLocation();
		FRotator Rotation = PlayerSpawnPoint->GetActorRotation();
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		SpawnParams.Owner = MyPlayerController;
		
		APlayerVehicule* PlayerVehicule = World->SpawnActor<APlayerVehicule>(PlayerClass, Location, Rotation, SpawnParams);
		
		if (IsValid(PlayerVehicule))
		{	
			MyPlayerController->Possess(PlayerVehicule);
			MyPlayerController->PlayerVehicule = PlayerVehicule;
			MyPlayerController->Start();
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("MyCharacter spawned and possessed successfully!"));
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Failed to spawn MyCharacter!"));
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("World is not valid!"));
	}
}
