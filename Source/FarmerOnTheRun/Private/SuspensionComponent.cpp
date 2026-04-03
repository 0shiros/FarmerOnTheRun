// Fill out your copyright notice in the Description page of Project Settings.


#include "SuspensionComponent.h"

#include "CollisionQueryParams.h"

// Sets default values for this component's properties
USuspensionComponent::USuspensionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}


// Called when the game starts
void USuspensionComponent::BeginPlay()
{
	Super::BeginPlay();
	
	Query = FCollisionQueryParams(FName(TEXT("")), false, GetOwner());
	Handle = new FTraceDelegate();
	Handle->BindUObject(this, &USuspensionComponent::OnTraceCompleted);
}


// Called every frame
void USuspensionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	PerformTrace();
}

void USuspensionComponent::Initialize(UPrimitiveComponent* InUpdatedComponent, const FVector2D& InAmplitudes, const float InStrength,
	UCurveFloat* InResponseCurve)
{
	this->UpdatedComponent = InUpdatedComponent;
	this->Amplitudes = InAmplitudes;
	this->Strength = InStrength;
	this->ResponseCurve = InResponseCurve;
	
	if (this->UpdatedComponent && this->ResponseCurve) 
	{
		SetComponentTickEnabled(true);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("USuspensionComponent::Init failed. UpdatedComponent or ResponseCurve is null."));
		SetComponentTickEnabled(false);
	}
}

FSuspensionResult USuspensionComponent::GetResults()
{
	return Results;
}

void USuspensionComponent::PerformTrace()
{
	const FVector Start = GetComponentLocation() + GetUpVector() * Amplitudes.X;
	const FVector End = GetComponentLocation() - GetUpVector() * Amplitudes.Y;
	FCollisionObjectQueryParams ObjectQueryParams = FCollisionObjectQueryParams(ECC_WorldStatic | ECC_WorldDynamic);
	
	GetWorld()->AsyncLineTraceByObjectType(
		EAsyncTraceType::Single,
		Start,
		End,
		ObjectQueryParams,
		Query,
		Handle);
}

void USuspensionComponent::OnTraceCompleted(const FTraceHandle& CurrentHandle, FTraceDatum& Data)
{
	if (Data.OutHits.Num())
	{
		Results.IsGrounded = true;
		Results.CompressionRate = 1.f - Data.OutHits[0].Time;
		Results.AirTime = 0.f;
		Results.RepulsionForce = (1.f - Data.OutHits[0].Time) * Strength * GetUpVector();
		Results.GroundNormal = Data.OutHits[0].ImpactNormal;
		Results.DriftRatio = FVector::DotProduct(GetOwner()->GetVelocity(), GetRightVector());
		Results.PhysicalMaterial = Data.OutHits[0].PhysMaterial.Get();
		Results.GroundComponent = Data.OutHits[0].GetComponent();
	}
	else
	{
		Results.IsGrounded = false;
		Results.CompressionRate = 0.f;
		Results.AirTime += GetWorld()->GetDeltaSeconds();
		Results.RepulsionForce = FVector::ZeroVector;
		Results.GroundNormal = FVector::ZeroVector;
		Results.DriftRatio = 0.f;
		Results.PhysicalMaterial = nullptr;
		Results.GroundComponent = nullptr;
	}
}



