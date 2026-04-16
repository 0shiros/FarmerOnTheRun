// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "MySaveGame.generated.h"

/**
 * 
 */
UCLASS()
class FARMERONTHERUN_API UMySaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:		
	
	UPROPERTY(VisibleAnywhere, Category = "Timer")
	TArray<float> LeaderboardTimes;
};
