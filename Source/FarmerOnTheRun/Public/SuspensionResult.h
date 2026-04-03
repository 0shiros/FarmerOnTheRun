#pragma once

#include "CoreMinimal.h"
#include "SuspensionResult.generated.h"

USTRUCT(BlueprintType)
struct FSuspensionResult
{
	GENERATED_BODY()
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsGrounded;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CompressionRate;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AirTime;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector RepulsionForce;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector GroundNormal;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DriftRatio;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UPhysicalMaterial> PhysicalMaterial;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UPrimitiveComponent> GroundComponent;
};
