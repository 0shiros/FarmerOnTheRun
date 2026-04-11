// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerVehicle.h"

#include "SuspensionComponent.h"
#include "VehicleMovement.h"
#include "WheelSetup.h"
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
}

void APlayerVehicle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	PrimaryActorTick.TickGroup = TG_PostPhysics;
	
	if (!VehicleData)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("VehicleData is not set!"));
		return;
	}
	
	FVector CoM = BoxCollider->GetBodyInstance()->GetCOMPosition();

	FVector AvgFront = FVector::ZeroVector;
	FVector AvgRear = FVector::ZeroVector;

	for (int32 i = 0; i < Wheels.Num(); i++)
	{
		if (!IsValid(Wheels[i].SuspensionComponent))
			continue;

		FVector Loc = Wheels[i].SuspensionComponent->GetComponentLocation();

		if (i < Wheels.Num() / 2)
			AvgFront += Loc;
		else
			AvgRear += Loc;
	}

	AvgFront /= (Wheels.Num() / 2);
	AvgRear /= (Wheels.Num() / 2);

	FVector AxleCenter = (AvgFront + AvgRear) / 2.f;

	UE_LOG(LogTemp, Warning, TEXT("CoM World    : %s"), *CoM.ToString());
	UE_LOG(LogTemp, Warning, TEXT("Axle Center  : %s"), *AxleCenter.ToString());
	UE_LOG(LogTemp, Warning, TEXT("CoM Offset X : %.4f"), CoM.X - AxleCenter.X);
	UE_LOG(LogTemp, Warning, TEXT("CoM Offset Y : %.4f"), CoM.Y - AxleCenter.Y);
	UE_LOG(LogTemp, Warning, TEXT("CoM Offset Z : %.4f"), CoM.Z - AxleCenter.Z);
	UE_LOG(LogTemp, Warning, TEXT("Wheelbase    : %.4f"), FVector::Dist(AvgFront, AvgRear));
	
	UpdateSuspension();
}

void APlayerVehicle::UpdateSuspension()
{
	TArray<FVector> Forces;
	TArray<FVector> Locations;

	for (FWheelSetup& Wheel : Wheels)
	{
		if (!IsValid(Wheel.SuspensionComponent))
			continue;

		FVector WheelLocation = Wheel.SuspensionComponent->GetComponentLocation();
		FVector WheelUpVector = FVector::UpVector; 

		Wheel.SuspensionComponent->PerformSuspensionTrace(VehicleData, WheelLocation, WheelUpVector);
		Forces.Add(Wheel.SuspensionComponent->CalculateSuspensionForce(BoxCollider, VehicleData, WheelLocation, WheelUpVector));
		Locations.Add(WheelLocation);
	}

	for (int32 i = 0; i < Forces.Num(); i++)
	{
		BoxCollider->AddForceAtLocation(Forces[i], Locations[i]);
		UE_LOG(LogTemp, Warning, TEXT("Suspension Force: %s at Location: %s"), *Forces[i].ToString(), *Locations[i].ToString());
	}
	
	
}



