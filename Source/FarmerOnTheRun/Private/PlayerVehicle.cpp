// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerVehicle.h"

#include "CheckGoal.h"
#include "NiagaraComponent.h"
#include "SuspensionComponent.h"
#include "VehicleMovement.h"
#include "WheelSetup.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "Components/PointLightComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APlayerVehicle::APlayerVehicle()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
	RootComponent = BoxCollider;

	BoxCollider->SetSimulatePhysics(true);
	BoxCollider->SetMassOverrideInKg(NAME_None, 1200.f);
	BoxCollider->BodyInstance.bUseCCD = true;
	BoxCollider->SetEnableGravity(false);
	BoxCollider->SetCollisionProfileName(TEXT("Vehicle"));
	BoxCollider->SetLinearDamping(0.f);
	BoxCollider->SetAngularDamping(0.f);
	BoxCollider->SetCenterOfMass(FVector(7.1283f, 0.f, -50.f));
	BoxCollider->SetBoxExtent(FVector(106.f, 54.f, 32.f));
	
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	ArrowComponent->SetupAttachment(RootComponent);
	ArrowComponent->ArrowLength = 200.f;	

	CarBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	CarBody->SetupAttachment(RootComponent);
	CarBody->SetCollisionProfileName(TEXT("NoCollision"));
	CarBody->SetComponentTickEnabled(false);
	CarBody->SetRelativeLocation(FVector(-15.f, 0.f, -57.f));
	CarBody->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
	
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 300.f;
	SpringArmComponent->SetRelativeLocation(FVector(0.f, 0.f, 110.f));
	SpringArmComponent->SetRelativeRotation(FRotator(-10.f, 0.f, 0.f));

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->FieldOfView = 110.f;
	
	TObjectPtr<UNiagaraComponent> SkidMarkLeftEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SkidMarkLeftEffect"));
	SkidMarkLeftEffect->SetupAttachment(RootComponent);
	SkidMarkEffects.Add(SkidMarkLeftEffect);
	
	TObjectPtr<UNiagaraComponent> SkidMarkRightEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SkidMarkRightEffect"));
	SkidMarkRightEffect->SetupAttachment(RootComponent);
	SkidMarkEffects.Add(SkidMarkRightEffect);
	
	EngineSoundEffect = CreateDefaultSubobject<UAudioComponent>(TEXT("EngineSoundEffect"));
	EngineSoundEffect->SetupAttachment(RootComponent);
	
	TObjectPtr<UPointLightComponent> EngineLightRightEffect = CreateDefaultSubobject<UPointLightComponent>(TEXT("EngineLightRightEffect"));
	EngineLightRightEffect->SetupAttachment(RootComponent);
	RearLights.Add(EngineLightRightEffect);
	
	TObjectPtr<UPointLightComponent> EngineLightLeftEffect = CreateDefaultSubobject<UPointLightComponent>(TEXT("EngineLightLeftEffect"));
	EngineLightLeftEffect->SetupAttachment(RootComponent);
	RearLights.Add(EngineLightLeftEffect);
	

	Wheels.SetNum(4);
	
	auto SetupWheel = [this](EWheelPosition Position,
					  TObjectPtr<USuspensionComponent>& SuspensionComponent,
					  TObjectPtr<UStaticMeshComponent>& WheelMeshComponent,
					  TObjectPtr<USceneComponent>& PivotComponent,
					  const TCHAR* SuspensionName)
	{
		FWheelSetup& Wheel = Wheels[static_cast<uint8>(Position)];
		
		Wheel.WheelPosition = Position;

		SuspensionComponent = CreateDefaultSubobject<USuspensionComponent>(SuspensionName);
		SuspensionComponent->SetupAttachment(RootComponent);
		Wheel.SuspensionComponent = SuspensionComponent;

		const FName PivotName = *FString::Printf(TEXT("Pivot_%s"), SuspensionName);
		PivotComponent = CreateDefaultSubobject<USceneComponent>(PivotName);
		PivotComponent->SetupAttachment(SuspensionComponent);
		Wheel.PivotComponent = PivotComponent;

		const FName MeshName = *FString::Printf(TEXT("WheelMesh_%s"), SuspensionName);
		WheelMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(MeshName);
		WheelMeshComponent->SetupAttachment(PivotComponent);
		WheelMeshComponent->SetCollisionProfileName(TEXT("NoCollision"));
		WheelMeshComponent->SetComponentTickEnabled(false);
		Wheel.WheelMesh = WheelMeshComponent;		
	};

	SetupWheel(EWheelPosition::FrontLeft,  Suspension_FL, WheelMesh_FL, Pivot_FL, TEXT("Suspension_FL"));
	Suspension_FL->SetRelativeLocation(FVector(70.f, -47.f, -32.f));
	WheelMesh_FL->SetRelativeLocation(FVector(-85.f, 46.f, -23.f));
	WheelMesh_FL->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
	SetupWheel(EWheelPosition::FrontRight, Suspension_FR, WheelMesh_FR, Pivot_FR, TEXT("Suspension_FR"));
	Suspension_FR->SetRelativeLocation(FVector(70.f, 47.f, -32.f));
	WheelMesh_FR->SetRelativeLocation(FVector(-85.f, -46.f, -23.f));
	WheelMesh_FR->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
	SetupWheel(EWheelPosition::RearLeft,   Suspension_RL, WheelMesh_RL, Pivot_RL, TEXT("Suspension_RL"));
	Suspension_RL->SetRelativeLocation(FVector(-60.f, -47.f, -32.f));
	WheelMesh_RL->SetRelativeLocation(FVector(45.f, 46.f, -23.f));
	WheelMesh_RL->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
	SetupWheel(EWheelPosition::RearRight,  Suspension_RR, WheelMesh_RR, Pivot_RR, TEXT("Suspension_RR"));
	Suspension_RR->SetRelativeLocation(FVector(-60.f, 47.f, -32.f));
	WheelMesh_RR->SetRelativeLocation(FVector(45.f, -46.f, -23.f));
	WheelMesh_RR->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));

	VehicleMovementComponent = CreateDefaultSubobject<UVehicleMovement>(TEXT("VehicleMovementComponent"));	
}

void APlayerVehicle::BeginPlay()
{
	Super::BeginPlay();
	
	CheckGoal = Cast<ACheckGoal>(UGameplayStatics::GetActorOfClass(GetWorld(), ACheckGoal::StaticClass()));
	
	if (CheckGoal)
	{
		CheckGoal->OnGoalReached.BindUObject(this, &APlayerVehicle::DetachFromComponent);
	}
}

void APlayerVehicle::DetachFromComponent()
{
	SpringArmComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
}



