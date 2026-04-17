// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

UCLASS()
class FARMERONTHERUN_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, Category = "SaveSlot")
	FString SaveSlotName = "PlayerSave";
	
	UPROPERTY(VisibleAnywhere, Category = "SaveGame")
	TObjectPtr<class UMySaveGame> SaveGame;
	
protected:
	virtual void Init() override;

	UMySaveGame* LoadOrCreateSaveGame();	
	
public :
	
	void SaveGameToSlot();
	
	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	UMySaveGame* GetSaveGame() const { return SaveGame; }
};
