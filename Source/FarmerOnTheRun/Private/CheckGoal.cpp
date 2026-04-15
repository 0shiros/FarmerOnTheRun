// Fill out your copyright notice in the Description page of Project Settings.
#include "CheckGoal.h"

// Sets default values
ACheckGoal::ACheckGoal()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

void ACheckGoal::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor,
	class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	if (IsValid(OtherActor))
	{
		OnGoalReached.Broadcast();
	}
}
