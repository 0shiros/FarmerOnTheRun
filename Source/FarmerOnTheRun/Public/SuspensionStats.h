// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SuspensionStats.generated.h"

USTRUCT(BlueprintType)
struct FSuspensionStats 
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsGrounded;
		
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float RestDist;	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float SpringForce;
		
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float SpringDamping;	
};
