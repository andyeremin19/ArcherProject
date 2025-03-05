// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Arch_Types.generated.h"

/**
 * 
 */
UENUM()
enum class EBowState : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	Aim UMETA(DisplayName = "Aim"),
	Draw UMETA(DisplayName = "Draw")
};



UCLASS()
class ARCHER_API UArch_Types : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
};
