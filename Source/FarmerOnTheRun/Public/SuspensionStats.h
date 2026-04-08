// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SuspensionStats.generated.h"

USTRUCT(BlueprintType)
struct FSuspensionStats 
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsGrounded;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RestDist;	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpringForce;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpringDamping;
};
