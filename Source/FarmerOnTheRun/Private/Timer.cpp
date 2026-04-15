// Fill out your copyright notice in the Description page of Project Settings.


#include "Timer.h"
#include "CheckGoal.h"
#include "CheckStart.h"
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
	
	CheckStart = Cast<ACheckStart>(UGameplayStatics::GetActorOfClass(GetWorld(), ACheckStart::StaticClass()));
	CheckGoal = Cast<ACheckGoal>(UGameplayStatics::GetActorOfClass(GetWorld(), ACheckGoal::StaticClass()));
	
	if (CheckStart && CheckGoal)
	{
		CheckStart->OnStartReached.BindUObject(this, &ATimer::StartTimer);
		CheckGoal->OnGoalReached.AddUObject(this, &ATimer::StopTimer);
	}
}

void ATimer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	if (CheckStart && CheckGoal)
	{
		CheckStart->OnStartReached.Unbind();
		CheckGoal->OnGoalReached.RemoveAll(this);
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
	
	if (CheckStart)
	{
		CheckStart->OnStartReached.Unbind();
	}	
}

void ATimer::StopTimer()
{
	bIsTimerRunning = false;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Final Time: %.2f seconds"), CurrentTime));
}
