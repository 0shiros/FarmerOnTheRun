// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerVehicle.h"
#include "GameFramework/GameModeBase.h"
#include "GMGame.generated.h"


UCLASS(Abstract)
class FARMERONTHERUN_API AGMGame : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AGMGame();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= PlayerVehiculeClass)
	TSubclassOf<APlayerVehicle> PlayerClass;
		
private:
	virtual void BeginPlay() override;	
};
