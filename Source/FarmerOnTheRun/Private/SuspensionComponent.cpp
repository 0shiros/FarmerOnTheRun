// Fill out your copyright notice in the Description page of Project Settings.


#include "SuspensionComponent.h"
#include "PlayerVehicle.h"
#include "CollisionQueryParams.h"
#include "Components/BoxComponent.h"

// Sets default values for this component's properties
USuspensionComponent::USuspensionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void USuspensionComponent::BeginPlay()
{
	Super::BeginPlay();	
	Query = FCollisionQueryParams(FName(TEXT("")), false, GetOwner());
}


// Called every frame
void USuspensionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	PerformTrace();
}

void USuspensionComponent::Initialize(float NewRestDist, float NewSpringForce, float NewSpringDamping)
{
	SuspensionStats.SpringForce = NewSpringForce;
	SuspensionStats.SpringDamping = NewSpringDamping;
	SuspensionStats.RestDist = NewRestDist;
}

const FHitResult& USuspensionComponent::GetResults() const
{
	return OutHit;
}

void USuspensionComponent::PerformTrace()
{	
	const FVector Start = GetComponentLocation();
	const FVector End = Start - GetUpVector() *  SuspensionStats.RestDist;
	FCollisionObjectQueryParams ObjectQueryParams = FCollisionObjectQueryParams(ECC_WorldStatic | ECC_WorldDynamic);
	
	GetWorld()->LineTraceSingleByObjectType(
		OutHit, 
		Start, 
		End, 
		ObjectQueryParams, 
		Query);
	
	OnTraceCompleted();
	
	DrawDebugDirectionalArrow(
		GetWorld(),
		Start,
		Start - GetUpVector() *  SuspensionStats.RestDist * 2.f,
		10.f,
		FColor::Yellow,
		false,
		-1.f,
		0,
		2.f);
	
}


void USuspensionComponent::OnTraceCompleted()
{
	if (OutHit.bBlockingHit)
	{
		SuspensionStats.IsGrounded = true;
	}
	else
	{
		SuspensionStats.IsGrounded = false;
	}
}

FVector USuspensionComponent::CalculateSuspension(APlayerVehicle* VehiclePlayer, float OutDistance)
{	
	FVector springDir = OutHit.ImpactNormal;
	FVector tireWorldVel = VehiclePlayer->BoxCollisionComponent->GetPhysicsLinearVelocityAtPoint(GetComponentLocation());
	float offSet = SuspensionStats.RestDist - OutDistance;
	float vel = FVector::DotProduct(springDir, tireWorldVel);
	float force = (offSet * SuspensionStats.SpringForce) - (vel * SuspensionStats.SpringDamping);
	FVector suspensionForce = springDir * force;
	return suspensionForce;
}

bool USuspensionComponent::GetIsGrounded() const
{
	return SuspensionStats.IsGrounded;
}



