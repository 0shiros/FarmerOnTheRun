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
	
	GameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	
	if (IsValid(GameInstance))
	{
		Init();
	}
}

void AGhostReplay::Init()
{
	PlayerVehicle = Cast<APlayerVehicle>(UGameplayStatics::GetActorOfClass(GetWorld(),APlayerVehicle::StaticClass()));
	Timer = Cast<ATimer>(UGameplayStatics::GetActorOfClass(GetWorld(), ATimer::StaticClass()));
		
	if (IsValid(PlayerVehicle) && IsValid(PlayerVehicle->VehicleMovementComponent))
	{
		PlayerVehicle->VehicleMovementComponent->OnTransformUpdate.BindUObject(this, &AGhostReplay::RecordReplay);				
	}
	
	if (IsValid(Timer))
	{
		Timer->OnTimerBegin.AddDynamic(this, &AGhostReplay::HasRaceBegun);
		Timer->OnLeaderSave.AddDynamic(this, &AGhostReplay::HasRaceEnded);
	}
}

void AGhostReplay::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	if (IsValid(PlayerVehicle) && IsValid(PlayerVehicle->VehicleMovementComponent))
	{
		PlayerVehicle->VehicleMovementComponent->OnTransformUpdate.Unbind();
	}
	
	if (IsValid(Timer))
	{
		Timer->OnTimerBegin.RemoveDynamic(this, &AGhostReplay::HasRaceBegun);
		Timer->OnLeaderSave.RemoveDynamic(this, &AGhostReplay::HasRaceEnded);
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
	if (!IsValid(GameInstance) || !IsValid(GameInstance->GetSaveGame()) || GameInstance->GetSaveGame()->PlayerTransforms.Num() == 0)
	{
		return;
	}
		
	GhostVehicle = GetWorld()->SpawnActor<AGhostVehicle>(
		GhostVehicleClass, 
		GameInstance->GetSaveGame()->PlayerTransforms[0].GetLocation(), 
		GameInstance->GetSaveGame()->PlayerTransforms[0].GetRotation().Rotator());
	
	if (!IsValid(GhostVehicle))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn ghost vehicle"));
		return;
	}
	
	GhostTransforms = GameInstance->GetSaveGame()->PlayerTransforms;

	if (IsValid(Timer))
	{
		Timer->OnTimerBegin.RemoveAll(this);
	}
}

void AGhostReplay::HasRaceBegun()
{	
	LoadGhostReplay();
	bHasBeginRace = true;
}

void AGhostReplay::HasRaceEnded()
{
	bHasBeginRace = false;	
	
	if (!IsValid(GameInstance) || !IsValid(GameInstance->GetSaveGame()) || !IsValid(Timer))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Invalid GameInstance, SaveGame or Timer. Cannot save replay.") );
		return;
	}	
	
	if ( GameInstance->GetSaveGame()->LeaderboardTimes.Num() == 0 ||Timer->CurrentTime <= GameInstance->GetSaveGame()->LeaderboardTimes[0])
	{
		GameInstance->GetSaveGame()->PlayerTransforms = RecordedTransforms;
		GameInstance->SaveGameToSlot();
	}		
}


