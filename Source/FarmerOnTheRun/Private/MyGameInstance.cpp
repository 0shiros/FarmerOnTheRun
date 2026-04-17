// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"
#include "MySaveGame.h"
#include "Kismet/GameplayStatics.h"

void UMyGameInstance::Init()
{
	Super::Init();
	
	SaveGame = LoadOrCreateSaveGame();	
}

UMySaveGame* UMyGameInstance::LoadOrCreateSaveGame()
{
	UMySaveGame* LoadedSaveGame = nullptr;
	
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
	{
		LoadedSaveGame = Cast<UMySaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
	}
	else
	{
		LoadedSaveGame = Cast<UMySaveGame>(UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));
	}
	
	return LoadedSaveGame;
}

void UMyGameInstance::SaveGameToSlot()
{
	UGameplayStatics::SaveGameToSlot(SaveGame, SaveSlotName, 0);
}
