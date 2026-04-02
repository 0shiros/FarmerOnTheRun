// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VehicleStats.h"
#include "GameFramework/Pawn.h"
#include "PlayerVehicle.generated.h"

UCLASS(Abstract)
class FARMERONTHERUN_API APlayerVehicle : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayerVehicle();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	TObjectPtr<class UCameraComponent> CameraComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SpringArm)
	TObjectPtr<class USpringArmComponent> SpringArmComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BoxCollision)
	TObjectPtr<class UBoxComponent> BoxCollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SkeletalMesh)
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Arrow)
	TObjectPtr<class UArrowComponent> ArrowComponent;		
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Wheels)
	TArray<TObjectPtr<class USuspensionComponent>> Wheels;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	FVehicleStats VehicleStats;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	void SetRepulsionForce();
	
	void Accelerate(float Value);
};
