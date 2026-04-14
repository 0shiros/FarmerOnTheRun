// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseCheckPoint.generated.h"

UCLASS(Abstract)
class FARMERONTHERUN_API ABaseCheckPoint : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABaseCheckPoint();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BoxCollider")
	TObjectPtr<class UBoxComponent> BoxCollider;

protected:	
	UFUNCTION()
	virtual void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {};
};
