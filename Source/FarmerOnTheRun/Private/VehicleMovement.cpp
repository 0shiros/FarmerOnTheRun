// Fill out your copyright notice in the Description page of Project Settings.


#include "VehicleMovement.h"
#include "PlayerVehicle.h"
#include "SuspensionComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"

// Sets default values for this component's properties
UVehicleMovement::UVehicleMovement()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UVehicleMovement::BeginPlay()
{
	Super::BeginPlay();

	VehicleOwner = Cast<APlayerVehicle>(GetOwner());
	
	if (!VehicleOwner)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("UVehicleMovement::BeginPlay failed. Owner is not APlayerVehicle."));
	}	
	
	VehicleOwner->BoxCollisionComponent->SetAngularDamping(VehicleOwner->GetVehicleStats().DefaultAngularDamping);
}


// Called every frame
void UVehicleMovement::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	SetRepulsionForce();	
}

float UVehicleMovement::GetNormalizeSpeed(float MaxSpeed)
{
	FVector linearVelocity = VehicleOwner->BoxCollisionComponent->GetPhysicsLinearVelocity();
	float vehicleSpeed = FVector::DotProduct(VehicleOwner->ArrowComponent->GetForwardVector(), linearVelocity);
	vehicleSpeed = FMath::Abs(linearVelocity.Length()) > 0 ? vehicleSpeed : 0.01f;
	NormalizeSpeed = FMath::Abs(vehicleSpeed) / MaxSpeed;
	NormalizeSpeed = FMath::Clamp(NormalizeSpeed, 0.f, 1.f);
	return NormalizeSpeed;
}

int UVehicleMovement::GetSpeedInKmh()
{
	return FMath::Abs(FMath::RoundToInt(VehicleOwner->BoxCollisionComponent->GetPhysicsLinearVelocity().Size() * 0.036f));
}

void UVehicleMovement::ApplyDriveForce(float Value, float MaxSpeed, float ForceMultiplier)
{
	if (GetSpeedInKmh() >= MaxSpeed) return;
	
	for (TObjectPtr Wheel : VehicleOwner->Wheels)
	{
		if (!Wheel->GetIsGrounded()) continue;
				
		FVector accelDir = Wheel->GetForwardVector();
		float accelCurve = VehicleOwner->GetVehicleStats().AccelerationTorqueCurve->GetFloatValue(FMath::Abs(GetNormalizeSpeed(MaxSpeed)));
		FVector accelForce = accelCurve * Value * accelDir * ForceMultiplier * VehicleOwner->BoxCollisionComponent->GetMass();
			VehicleOwner->BoxCollisionComponent->AddForceAtLocation(accelForce, Wheel->GetComponentLocation());
		
		DrawDebugDirectionalArrow(
			GetWorld(),
			Wheel->GetComponentLocation(),
			Wheel->GetComponentLocation() + accelForce * 0.1f,
			10.f,
			FColor::Green,
			false,
			-1.f,
			0,
			2.f);
		
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Velocity: %s"), *VehicleOwner->BoxCollisionComponent->GetPhysicsLinearVelocity().ToString()));
		
	}
}


void UVehicleMovement::Accelerate(float Value) 
{ 
	if (!IsGrounded() || !VehicleOwner) return;
	
	ApplyDriveForce(Value, VehicleOwner->GetVehicleStats().TopSpeed, VehicleOwner->GetVehicleStats().AccelerationForce);
	
}
void UVehicleMovement::Reverse(float Value)    
{ 
	if (!IsGrounded() || !VehicleOwner) return;

	ApplyDriveForce(Value, VehicleOwner->GetVehicleStats().ReverseTopSpeed, VehicleOwner->GetVehicleStats().DecelerationForce);
}

void UVehicleMovement::TurnLeftRight(float Value)
{	
	if (!IsGrounded() || !VehicleOwner || GetSpeedInKmh() < 1) return;
		
	float turnTorque = VehicleOwner->GetVehicleStats().TurnTorque * Value * NormalizeSpeed;
	FVector turnForce = FVector(0.f, 0.f, turnTorque) * VehicleOwner->BoxCollisionComponent->GetMass();
	VehicleOwner->BoxCollisionComponent->AddTorqueInRadians(turnForce);	
}

void UVehicleMovement::SetRepulsionForce()
{	
	if (!VehicleOwner) return;
	
	for (TObjectPtr Wheel : VehicleOwner->Wheels)
	{
		if (!Wheel->GetIsGrounded()) continue;
		
		FVector Force = Wheel->CalculateSuspension(VehicleOwner, Wheel->GetResults().Distance);
		FVector Location = Wheel->GetComponentLocation();	
		
		VehicleOwner->BoxCollisionComponent->AddForceAtLocation(Force, Location);
		
		DrawDebugDirectionalArrow(
			GetWorld(),
			Location,
			Location + Force * 0.1f,
			10.f,
			FColor::Blue,
			false,
			-1.f,
			0,
			2.f);
	}
}

void UVehicleMovement::Drift(bool bIsDrifting)
{
	if (!VehicleOwner) return;
	
	VehicleOwner->SetDrifting(bIsDrifting);
	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Drifting: %s"), bIsDrifting ? TEXT("True") : TEXT("False")));
	
	if (bIsDrifting)
	{
		VehicleOwner->BoxCollisionComponent->SetAngularDamping(VehicleOwner->GetVehicleStats().DriftingAngularDamping);
	}
	else
	{
		VehicleOwner->BoxCollisionComponent->SetAngularDamping(VehicleOwner->GetVehicleStats().DefaultAngularDamping);
	}
}

bool UVehicleMovement::IsGrounded() const
{
	if (!VehicleOwner) return false;
	
	for (TObjectPtr Wheel : VehicleOwner->Wheels)
		if (Wheel->GetIsGrounded()) return true;
	return false;
}
