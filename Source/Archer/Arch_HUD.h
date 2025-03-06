// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Arch_HUD.generated.h"

class UBowMechanicsComponent;

/**
 * 
 */
UCLASS()
class ARCHER_API AArch_HUD : public AHUD
{
	GENERATED_BODY()

	//Variables

private:

	UPROPERTY()
	UBowMechanicsComponent* BowMechComp = nullptr;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintNativeEvent)
	void HideCrosshair();

	UFUNCTION(BlueprintNativeEvent)
	void ShowCrosshair();

	void BindDelegates();

	UFUNCTION(BlueprintNativeEvent)
	void ResizeCrosshair( float MaxDrawTime, float DrawTime);

	UFUNCTION(BlueprintNativeEvent)
	void ResizeCrosshairToOriginal();
	
};
