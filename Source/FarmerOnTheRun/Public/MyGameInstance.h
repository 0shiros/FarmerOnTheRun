// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"


UCLASS()
class FARMERONTHERUN_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(VisibleAnywhere, Category = "SaveSlot")
	FString SaveSlotName = "PlayerSave";
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SaveGame")
	TObjectPtr<class UMySaveGame> SaveGame;
	
protected:
	virtual void Init() override;
};
