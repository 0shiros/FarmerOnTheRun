// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Timer.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimerUpdated, float, CurrentTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLeaderboardUpdate, const TArray<float>&, LeaderboardTimes, float, CurrentTime);

UCLASS(Abstract)
class FARMERONTHERUN_API ATimer : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATimer();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GameMode")
	TObjectPtr<class UMyGameInstance> GameInstance;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CheckStart")
	TObjectPtr<class ACheckStart> CheckStart;	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CheckStart")
	TObjectPtr<class ACheckGoal> CheckGoal;	
		
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Timer")
	float CurrentTime = 0.0f;	
	
	UPROPERTY(VisibleAnywhere, BlueprintAssignable, Category = "Timer")
	FOnTimerUpdated OnTimerUpdated;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Timer")
	TArray<float> LeaderboardTimes;
	
	UPROPERTY(VisibleAnywhere, BlueprintAssignable, Category = "Timer")
	FOnLeaderboardUpdate OnLeaderboardUpdate;
			
private :	
	bool bIsTimerRunning = false;
	
protected:
	
	virtual void BeginPlay() override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual void Tick(float DeltaTime) override;
	
	void StartTimer();
	
	void StopTimer();
	
	void UpdateLeaderboard();
	
	void LoadLeaderboardTimes();
	
	void SaveLeaderboardTimes();
};
