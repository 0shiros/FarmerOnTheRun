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

void USuspensionComponent::Initialize(APlayerVehicle* NewOwningVehicle, float NewRestDist, float NewSpringForce, float NewSpringDamping)
{
	OwningVehicle = NewOwningVehicle;
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
	if (!OwningVehicle) return;
	
	const FVector Start = GetComponentLocation();
	const FVector End = Start - GetUpVector() * SuspensionStats.RestDist;
	FCollisionObjectQueryParams ObjectQueryParams = FCollisionObjectQueryParams(ECC_WorldStatic | ECC_WorldDynamic);
	
	GetWorld()->LineTraceSingleByObjectType(
		OutHit, 
		Start, 
		End, 
		ObjectQueryParams, 
		Query);
	
	OnTraceCompleted();
	
	#if ENABLE_DRAW_DEBUG
	DrawDebugLine(
		GetWorld(),
		Start, 
		End, 
		OutHit.bBlockingHit ? FColor::Green : FColor::Red,
		false, 
		-1.f, 
		0, 
		2.f
		);
	#endif	
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

FVector USuspensionComponent::CalculateSuspension(float OutDistance)
{
	if (!OwningVehicle || !SuspensionStats.IsGrounded) return FVector::ZeroVector;
	
	FVector VelocityAtPoint = OwningVehicle->BoxCollisionComponent->GetPhysicsLinearVelocityAtPoint(GetComponentLocation());	
	float Damping = FVector::DotProduct(VelocityAtPoint, OutHit.ImpactNormal) * SuspensionStats.SpringDamping;	
	float SuspensionForce = (SuspensionStats.RestDist - OutDistance)* SuspensionStats.SpringForce;		
	FVector ForwardSpeed = OutHit.ImpactNormal * (SuspensionForce - Damping);
		
	return ForwardSpeed ;
}

bool USuspensionComponent::GetIsGrounded() const
{
	return SuspensionStats.IsGrounded;
}



