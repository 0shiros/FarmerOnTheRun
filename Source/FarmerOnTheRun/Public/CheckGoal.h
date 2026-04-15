// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCheckPoint.h"
#include "CheckGoal.generated.h"

DECLARE_DELEGATE(FOnGoalReached);

UCLASS(Abstract)
class FARMERONTHERUN_API ACheckGoal : public ABaseCheckPoint
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACheckGoal();
	
	FOnGoalReached OnGoalReached;

protected:
	
	virtual void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;

};
