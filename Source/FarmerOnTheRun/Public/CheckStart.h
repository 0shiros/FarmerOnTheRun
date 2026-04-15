// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCheckPoint.h"
#include "CheckStart.generated.h"

DECLARE_DELEGATE(FOnStartReached);

UCLASS(Blueprintable)
class FARMERONTHERUN_API ACheckStart : public ABaseCheckPoint
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACheckStart();
	
	FOnStartReached OnStartReached;
	
protected:
	
	virtual void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;

};
