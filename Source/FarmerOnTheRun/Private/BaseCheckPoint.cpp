// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCheckPoint.h"
#include "Components/BoxComponent.h"

// Sets default values
ABaseCheckPoint::ABaseCheckPoint()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
	BoxCollider->SetupAttachment(RootComponent);
	BoxCollider->SetCollisionProfileName("Trigger");
	BoxCollider->SetBoxExtent(FVector(4500.f, 200.0f, 500.f));
	
	BoxCollider->OnComponentBeginOverlap.AddDynamic(this, &ABaseCheckPoint::OnOverlapBegin);
}

