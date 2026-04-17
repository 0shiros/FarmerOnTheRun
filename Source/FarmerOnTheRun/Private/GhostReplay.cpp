// Fill out your copyright notice in the Description page of Project Settings.


#include "GhostReplay.h"

#include "CheckGoal.h"
#include "CheckStart.h"
#include "GhostVehicle.h"
#include "MyGameInstance.h"
#include "MySaveGame.h"
#include "PlayerVehicle.h"
#include "Timer.h"
#include "VehicleMovement.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AGhostReplay::AGhostReplay()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AGhostReplay::BeginPlay()
{
	Super::BeginPlay();	
	
	PlayerVehicle = Cast<APlayerVehicle>(UGameplayStatics::GetActorOfClass(GetWorld(),APlayerVehicle::StaticClass()));
	CheckGoal = Cast<ACheckGoal>(UGameplayStatics::GetActorOfClass(GetWorld(), ACheckGoal::StaticClass()));
	CheckStart = Cast<ACheckStart>(UGameplayStatics::GetActorOfClass(GetWorld(), ACheckStart::StaticClass()));
	Timer = Cast<ATimer>(UGameplayStatics::GetActorOfClass(GetWorld(), ATimer::StaticClass()));
		
	if (IsValid(PlayerVehicle))
	{
		PlayerVehicle->VehicleMovementComponent->OnTransformUpdate.BindUObject(this, &AGhostReplay::RecordReplay);				
	}
	
	if (IsValid(CheckStart))
	{
		CheckStart->OnStartReached.AddUObject(this, &AGhostReplay::HasRaceBegun);
	}

	if (IsValid(CheckGoal))
	{
		CheckGoal->OnGoalReached.AddUObject(this, &AGhostReplay::HasRaceEnded);
	}
}

void AGhostReplay::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	if (IsValid(PlayerVehicle))
	{
		PlayerVehicle->VehicleMovementComponent->OnTransformUpdate.Unbind();
	}
	
	if (IsValid(CheckStart))
	{
		CheckStart->OnStartReached.RemoveAll(this);
	}
	
	if (IsValid(CheckGoal))
	{
		CheckGoal->OnGoalReached.RemoveAll(this);
	}	
}

// Called every frame
void AGhostReplay::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bHasBeginRace)
	{
		ReadReplay();
	}
}

void AGhostReplay::RecordReplay(const FTransform& PlayerTransform)
{	
	RecordedTransforms.Add(PlayerTransform);	
}

void AGhostReplay::ReadReplay()
{
	if (!IsValid(GhostVehicle) || GhostTransforms.Num() == 0)	
	{		
		return;
	}	

	const FTransform& GhostTransform = GhostTransforms[0];
	GhostVehicle->SetActorTransform(GhostTransform);	
	GhostTransforms.RemoveAt(0);
	
	if (GhostTransforms.Num() == 0)
	{
		GhostVehicle->Destroy();
	}
}

void AGhostReplay::LoadGhostReplay()
{		
	GameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));	
	
	if (!IsValid(GameInstance))
	{
		return;
	}

	if (!IsValid(GameInstance->SaveGame))	
	{
		return;
	}
	
	if (GameInstance->SaveGame->PlayerTransforms.Num() == 0)
	{
		return;
	}
		
	 GhostVehicle = GetWorld()->SpawnActor<AGhostVehicle>(
	 	GhostVehicleClass, 
	 	GameInstance->SaveGame->PlayerTransforms[0].GetLocation(), 
	 	GameInstance->SaveGame->PlayerTransforms[0].GetRotation().Rotator());
	
	if (!IsValid(GhostVehicle))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn ghost vehicle"));
		return;
	}
	
	GhostTransforms = GameInstance->SaveGame->PlayerTransforms;

	CheckStart->OnStartReached.RemoveAll(this);
}

void AGhostReplay::HasRaceBegun()
{
	LoadGhostReplay();
	bHasBeginRace = true;
}

void AGhostReplay::HasRaceEnded()
{
	bHasBeginRace = false;
	
	if (GameInstance->SaveGame->LeaderboardTimes.Num() == 0 || Timer->CurrentTime < GameInstance->SaveGame->LeaderboardTimes[0])
	{
		GameInstance->SaveGame->PlayerTransforms = RecordedTransforms;
	    UGameplayStatics::SaveGameToSlot(GameInstance->SaveGame, GameInstance->SaveSlotName, 0);
	}		
}


