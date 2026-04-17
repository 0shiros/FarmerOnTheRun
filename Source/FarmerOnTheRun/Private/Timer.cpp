// Fill out your copyright notice in the Description page of Project Settings.


#include "Timer.h"
#include "CheckGoal.h"
#include "CheckStart.h"
#include "MyGameInstance.h"
#include "MySaveGame.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ATimer::ATimer()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ATimer::BeginPlay()
{
	Super::BeginPlay();
	
	GameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	
	if (IsValid(GameInstance))
	{
		Init();
	}
}

void ATimer::Init()
{	
	CheckStart = Cast<ACheckStart>(UGameplayStatics::GetActorOfClass(GetWorld(), ACheckStart::StaticClass()));
	CheckGoal = Cast<ACheckGoal>(UGameplayStatics::GetActorOfClass(GetWorld(), ACheckGoal::StaticClass()));
	
	if (IsValid(CheckStart))
	{
		CheckStart->OnStartReached.AddUObject(this, &ATimer::StartTimer);
	}
	
	if (IsValid(CheckGoal))
	{
		CheckGoal->OnGoalReached.AddUObject(this, &ATimer::StopTimer);
	}
}

void ATimer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	if (CheckStart)
	{
		CheckStart->OnStartReached.RemoveAll(this);
	}
	
	if (CheckGoal)
	{
		CheckGoal->OnGoalReached.RemoveAll(this);
	}
	
	if (OnLeaderboardUpdate.IsBound())
	{
		OnLeaderboardUpdate.Clear();
	}
	
	if (OnTimerUpdated.IsBound())
	{
		OnTimerUpdated.Clear();
	}
	
	if (OnTimerBegin.IsBound())
	{
		OnTimerBegin.Clear();
	}
}

void ATimer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);	
	
	if (bIsTimerRunning)
	{
		CurrentTime += DeltaTime;
		OnTimerUpdated.Broadcast(CurrentTime);
	}	
}

void ATimer::StartTimer()
{
	bIsTimerRunning = true;		
	OnTimerBegin.Broadcast();
	
	if (CheckStart)
	{
		CheckStart->OnStartReached.RemoveAll(this);
	}	
}

void ATimer::StopTimer()
{		
	bIsTimerRunning = false;
	UpdateLeaderboard();
	OnLeaderboardUpdate.Broadcast(LeaderboardTimes, CurrentTime);	
}

void ATimer::UpdateLeaderboard()
{	
	LeaderboardTimes.Sort();
	
	if (LeaderboardTimes.Num() < 10)
	{
		LeaderboardTimes.Add(CurrentTime);
		LeaderboardTimes.Sort();
		SaveLeaderboardTimes();
		return;
	}
	
	if (CurrentTime < LeaderboardTimes.Last())
	{
		LeaderboardTimes.Last() = CurrentTime;
		LeaderboardTimes.Sort();
		SaveLeaderboardTimes();
	}
}

void ATimer::LoadLeaderboardTimes()
{		
	LeaderboardTimes = GameInstance->GetSaveGame()->LeaderboardTimes;	
	OnTimerSetup.Broadcast();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Leaderboard times loaded"));
}

void ATimer::SaveLeaderboardTimes()
{	
	GameInstance->GetSaveGame()->LeaderboardTimes = LeaderboardTimes;
	GameInstance->SaveGameToSlot();
	OnLeaderSave.Broadcast();
}
