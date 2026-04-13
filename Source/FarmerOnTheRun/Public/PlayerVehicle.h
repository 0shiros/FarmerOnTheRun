// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SuspensionComponent.h"
#include "GameFramework/Pawn.h"
#include "PlayerVehicle.generated.h"

class UNiagaraComponent;

UCLASS(Abstract)
class FARMERONTHERUN_API APlayerVehicle : public APawn
{
	GENERATED_BODY()

public:
	
	APlayerVehicle();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<class UCameraComponent> CameraComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SpringArm")
	TObjectPtr<class USpringArmComponent> SpringArmComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ArrowComponent")
	TObjectPtr<class UArrowComponent> ArrowComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BoxCollider")
	TObjectPtr<UBoxComponent> BoxCollider;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	TObjectPtr<UStaticMeshComponent> CarBody;	
		
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	TObjectPtr<class UVehicleMovement> VehicleMovementComponent;
	
	UPROPERTY(VisibleAnywhere, Category = "Wheels|Suspension")
	TObjectPtr<USuspensionComponent> Suspension_FL;
	
	UPROPERTY(VisibleAnywhere, Category = "Wheels|Suspension")
	TObjectPtr<USuspensionComponent> Suspension_FR;
	
	UPROPERTY(VisibleAnywhere, Category = "Wheels|Suspension")
	TObjectPtr<USuspensionComponent> Suspension_RL;
	
	UPROPERTY(VisibleAnywhere, Category = "Wheels|Suspension")
	TObjectPtr<USuspensionComponent> Suspension_RR;
	
	UPROPERTY(VisibleAnywhere, Category = "Wheels|Pivot")
	TObjectPtr<USceneComponent> Pivot_FL;
	
	UPROPERTY(VisibleAnywhere, Category = "Wheels|Pivot")
	TObjectPtr<USceneComponent> Pivot_FR;
	
	UPROPERTY(VisibleAnywhere, Category = "Wheels|Pivot")
	TObjectPtr<USceneComponent> Pivot_RL;
	
	UPROPERTY(VisibleAnywhere, Category = "Wheels|Pivot")
	TObjectPtr<USceneComponent> Pivot_RR;

	UPROPERTY(VisibleAnywhere, Category = "Wheels|Mesh")
	TObjectPtr<UStaticMeshComponent> WheelMesh_FL;
	
	UPROPERTY(VisibleAnywhere, Category = "Wheels|Mesh")
	TObjectPtr<UStaticMeshComponent> WheelMesh_FR;
	
	UPROPERTY(VisibleAnywhere, Category = "Wheels|Mesh")
	TObjectPtr<UStaticMeshComponent> WheelMesh_RL;
	
	UPROPERTY(VisibleAnywhere, Category = "Wheels|Mesh")
	TObjectPtr<UStaticMeshComponent> WheelMesh_RR;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wheels")
	TArray<struct FWheelSetup> Wheels;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkidMark")
	TObjectPtr<UNiagaraComponent> SkidMarkRightEffect;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkidMark")
	TObjectPtr<UNiagaraComponent> SkidMarkLeftEffect;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
	TObjectPtr<UVehicleData> VehicleData;						
};
