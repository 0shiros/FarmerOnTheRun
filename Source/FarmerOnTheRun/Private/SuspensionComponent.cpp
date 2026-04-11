// Fill out your copyright notice in the Description page of Project Settings.


#include "SuspensionComponent.h"
#include "CollisionQueryParams.h"
#include "Components/BoxComponent.h"
#include "VehicleData.h"

// Sets default values for this component's properties
USuspensionComponent::USuspensionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USuspensionComponent::BeginPlay()
{
	Super::BeginPlay();
	
	SuspensionQueryParams = FCollisionQueryParams(TEXT("SuspensionTrace"), false, GetOwner());
}

void USuspensionComponent::PerformSuspensionTrace(const UVehicleData* VehicleData, const FVector& WheelLocation, const FVector& WheelUpVector)
{
	if (!IsValid(VehicleData))
	{
		return;
	}
	
	FVector Start = WheelLocation;
	FVector End = Start - WheelUpVector * VehicleData->RestDist;

	GetWorld()->LineTraceSingleByChannel(SuspensionHit, Start, End, ECC_Visibility, SuspensionQueryParams);
	
	bIsGrounded = SuspensionHit.bBlockingHit;
	
	DrawDebugLine(GetWorld(), Start, End, SuspensionHit.bBlockingHit ? FColor::Green : FColor::Red, false, 0.1f);
}

FVector USuspensionComponent::CalculateSuspensionForce(UBoxComponent* BoxComponent,	const UVehicleData* VehicleData, const FVector& WheelLocation, const FVector& WheelUpVector)
{		
	if (!IsValid(BoxComponent) || !IsValid(VehicleData) || !bIsGrounded)
	{
		return FVector::ZeroVector;
	}
	
	FVector WheelVelocity = BoxComponent->GetPhysicsLinearVelocityAtPoint(WheelLocation);
	
	float OffSet = VehicleData->RestDist - SuspensionHit.Distance;
	UE_LOG(LogTemp, Warning, TEXT("Offset: %f"), OffSet);
	float Velocity = FVector::DotProduct(WheelUpVector, WheelVelocity);
	
	float Force = FMath::Clamp(OffSet * VehicleData->SpringStrength - Velocity * VehicleData->SpringDamping, 0.f, VehicleData->SpringStrength * 50.f);
	
	FVector SuspensionForce = WheelUpVector * Force;
	
	return SuspensionForce;
	
}


	



