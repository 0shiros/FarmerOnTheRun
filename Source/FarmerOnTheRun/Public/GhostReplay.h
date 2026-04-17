// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GhostReplay.generated.h"

class AGhostVehicle;

UCLASS()
class FARMERONTHERUN_API AGhostReplay : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGhostReplay();	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Player")
	TObjectPtr<class APlayerVehicle> PlayerVehicle;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Ghost")
	TObjectPtr<AGhostVehicle> GhostVehicle;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ghost")
	TSubclassOf<AGhostVehicle> GhostVehicleClass;
		
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GameMode")
	TObjectPtr<class UMyGameInstance> GameInstance;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Timer")
	TObjectPtr<class ATimer> Timer;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Replay")
	TArray<FTransform> RecordedTransforms;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Replay")
	TArray<FTransform> GhostTransforms;
	
	bool bHasBeginRace = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	void Init();
		
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
	void RecordReplay(const FTransform& PlayerTransform);
	
	void ReadReplay();
	
	void LoadGhostReplay();
	
	UFUNCTION()
	void HasRaceBegun();
	
	UFUNCTION()
	void HasRaceEnded();
};
