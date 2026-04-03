// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerVehicle.h"
#include "SuspensionComponent.h"
#include "VehicleMovement.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/ArrowComponent.h"

// Sets default values
APlayerVehicle::APlayerVehicle()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;	
	
	BoxCollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollisionComponent"));
	RootComponent = BoxCollisionComponent;
	
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	SkeletalMeshComponent->SetupAttachment(BoxCollisionComponent);
	
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	ArrowComponent->SetupAttachment(BoxCollisionComponent);
	
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(BoxCollisionComponent);
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);		
	
	SkeletalMeshComponent->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	BoxCollisionComponent->SetSimulatePhysics(true);
	
	TObjectPtr<USuspensionComponent> FrontLeftWheel = CreateDefaultSubobject<USuspensionComponent>(TEXT("FrontLeftWheel"));
	FrontLeftWheel->SetupAttachment(BoxCollisionComponent);
	Wheels.Add(FrontLeftWheel);
	
	TObjectPtr<USuspensionComponent> FrontRightWheel = CreateDefaultSubobject<USuspensionComponent>(TEXT("FrontRightWheel"));
	FrontRightWheel->SetupAttachment(BoxCollisionComponent);
	Wheels.Add(FrontRightWheel);
	
	TObjectPtr<USuspensionComponent> RearLeftWheel = CreateDefaultSubobject<USuspensionComponent>(TEXT("RearLeftWheel"));
	RearLeftWheel->SetupAttachment(BoxCollisionComponent);
	Wheels.Add(RearLeftWheel);
	
	TObjectPtr<USuspensionComponent> RearRightWheel = CreateDefaultSubobject<USuspensionComponent>(TEXT("RearRightWheel"));
	RearRightWheel->SetupAttachment(BoxCollisionComponent);
	Wheels.Add(RearRightWheel);    
	
	VehicleMovementComponent = CreateDefaultSubobject<UVehicleMovement>(TEXT("VehicleMovementComponent"));
}

// Called when the game starts or when spawned
void APlayerVehicle::BeginPlay()
{
	Super::BeginPlay();
	
	for (TObjectPtr Wheel : Wheels)
	{
		Wheel->Initialize(BoxCollisionComponent, VehicleStats.SuspensionAmplitudes, VehicleStats.SuspensionStrength, VehicleStats.SuspensionResponseCurve);
	}
}

// Called every frame
void APlayerVehicle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetRepulsionForce();
}

void APlayerVehicle::SetRepulsionForce()
{
	for (TObjectPtr Wheel : Wheels)
	{
		FVector Force = Wheel->GetResults().RepulsionForce;
		FVector Location = Wheel->GetComponentLocation();
		BoxCollisionComponent->AddForceAtLocation(Force, Location, NAME_None);
	}
}

