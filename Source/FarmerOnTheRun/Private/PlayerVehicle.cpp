// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerVehicle.h"

#include "SuspensionComponent.h"
#include "VehicleMovement.h"
#include "WheelSetup.h"
#include "VehicleData.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
APlayerVehicle::APlayerVehicle()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
	RootComponent = BoxCollider;

	BoxCollider->SetSimulatePhysics(true);
	BoxCollider->SetMassOverrideInKg(NAME_None, 1200.f);
	BoxCollider->BodyInstance.bUseCCD = true;
	BoxCollider->SetCollisionProfileName(TEXT("Vehicle"));
	BoxCollider->SetLinearDamping(0.1f);
	BoxCollider->SetAngularDamping(1.5f);
	
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	ArrowComponent->SetupAttachment(RootComponent);

	CarBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	CarBody->SetupAttachment(RootComponent);
	
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);

	Wheels.SetNum(4);
	
	auto Setup = [&](int32 i, EWheelPosition Pos,
	                 TObjectPtr<USuspensionComponent>& SuspComp,
	                 TObjectPtr<UStaticMeshComponent>& MeshComp,
	                 TObjectPtr<USceneComponent>& PivotComp,
	                 const TCHAR* Name)
	{
		FWheelSetup& W = Wheels[(uint8)Pos];

		SuspComp = CreateDefaultSubobject<USuspensionComponent>(Name);
		SuspComp->SetupAttachment(RootComponent);
		W.SuspensionComponent = SuspComp;
		
		PivotComp = CreateDefaultSubobject<USceneComponent>(*FString::Printf(TEXT("Pivot_%s"), Name));
		PivotComp->SetupAttachment(SuspComp);
		W.PivotComponent = PivotComp;

		FName MeshName = *FString::Printf(TEXT("WheelMesh_%s"), Name);
		MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(MeshName);
		MeshComp->SetupAttachment(PivotComp);
		MeshComp->SetCollisionProfileName(TEXT("NoCollision"));
		MeshComp->SetComponentTickEnabled(false);
		W.WheelMesh = MeshComp;
	};

	Setup(0, EWheelPosition::FrontLeft, Suspension_FL, WheelMesh_FL,
		Pivot_FL,TEXT("Suspension_FL"));
	Setup(1, EWheelPosition::FrontRight, Suspension_FR, WheelMesh_FR,
		Pivot_FR,TEXT("Suspension_FR"));
	Setup(2, EWheelPosition::RearLeft, Suspension_RL, WheelMesh_RL,
		Pivot_RL,TEXT("Suspension_RL"));
	Setup(3, EWheelPosition::RearRight, Suspension_RR, WheelMesh_RR,
		Pivot_RR,TEXT("Suspension_RR"));

	VehicleMovementComponent = CreateDefaultSubobject<UVehicleMovement>(TEXT("VehicleMovementComponent"));
}

void APlayerVehicle::BeginPlay()
{
	Super::BeginPlay();
	
	PrimaryActorTick.TickGroup = TG_PostPhysics;
}

void APlayerVehicle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);	
	
	if (!VehicleData)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("VehicleData is not set!"));
		return;
	}	
	
	UpdateSuspension();
	ApplySteeringToWheels(DeltaTime);
}

void APlayerVehicle::UpdateSuspension()
{
	for (FWheelSetup& Wheel : Wheels)
	{
		if (!IsValid(Wheel.SuspensionComponent))
			continue;

		FVector WheelLocation = Wheel.SuspensionComponent->GetComponentLocation();
		FVector WheelUpVector = Wheel.SuspensionComponent->GetUpVector();

		Wheel.SuspensionComponent->PerformSuspensionTrace(VehicleData, WheelLocation, WheelUpVector);
		FVector Force = Wheel.SuspensionComponent->CalculateSuspensionForce(BoxCollider, VehicleData, WheelLocation, WheelUpVector);
		BoxCollider->AddForceAtLocation(Force, WheelLocation);
		DrawDebugLine(GetWorld(), WheelLocation, WheelLocation + Force * 0.01f, FColor::Blue, false, 0.1f);
	}
}

void APlayerVehicle::ApplySteeringToWheels(float DeltaTime)
{
	for (FWheelSetup& Wheel : Wheels)
	{
		if (!IsValid(Wheel.SuspensionComponent) || !Wheel.SuspensionComponent->GetIsGrounded())
			continue;
	
		FVector WheelLocation = Wheel.SuspensionComponent->GetComponentLocation();
		FVector WheelRightVector = Wheel.SuspensionComponent->GetRightVector();
		FVector Force = VehicleMovementComponent->CalculateSteering(WheelLocation, WheelRightVector, VehicleData, BoxCollider, DeltaTime);
		
		BoxCollider->AddForceAtLocation(Force, WheelLocation);
		DrawDebugLine(GetWorld(), WheelLocation, WheelLocation + Force * 0.01f, FColor::Red, false, 0.1f);
		UE_LOG(LogTemp, Warning, TEXT("Steering Force: %s of Wheel at Location: %s"), *Force.ToString(), *WheelLocation.ToString());
	}
}

void APlayerVehicle::ApplyAcceleration(float Value)
{	
	if (GetCalculateCarSpeed() > VehicleData->MaxSpeed)
	{
		return;
	}	
	
	for (FWheelSetup& Wheel : Wheels)
	{
		if (!IsValid(Wheel.SuspensionComponent) || !Wheel.SuspensionComponent->GetIsGrounded())
		{
			continue;
		}
		
		FVector AccelDir = Wheel.SuspensionComponent->GetForwardVector(); 
		FVector VehicleVelocity = BoxCollider->GetComponentVelocity(); 
		FVector VehicleForwardVector = ArrowComponent->GetForwardVector();
		
		FVector Force = VehicleMovementComponent->CalculateAcceleration(AccelDir, VehicleVelocity, VehicleData, VehicleForwardVector, Value * VehicleData->EngineForce); 
		BoxCollider->AddForceAtLocation(Force, Wheel.SuspensionComponent->GetComponentLocation()); 
		
		DrawDebugLine(GetWorld(), Wheel.SuspensionComponent->GetComponentLocation(), Wheel.SuspensionComponent->GetComponentLocation() + Force * 0.01f, FColor::Green, false, 0.1f);
		//UE_LOG(LogTemp, Warning, TEXT("Acceleration Force: %s of Wheel at Location: %s"), *Force.ToString(), *Wheel.SuspensionComponent->GetComponentLocation().ToString());
	}
}

float APlayerVehicle::GetCalculateCarSpeed()
{
	return BoxCollider->GetComponentVelocity().Size();
}



