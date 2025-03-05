// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BowMechanicsComponent.generated.h"

class ABow;
class AArrow;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARCHER_API UBowMechanicsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBowMechanicsComponent();

	//Variables

protected:

	UPROPERTY(EditAnywhere, Category = "Bow")
	TSubclassOf<ABow> BowType;

	UPROPERTY(EditAnywhere, Category = "Arrow")
	TSubclassOf<AArrow> ArrowType;

	UPROPERTY(EditAnywhere, Category = "CameraSettings")
	FRotator CustomRotationRate;

private:

	UPROPERTY()
	AArrow* DrawnArrow = nullptr;

	UPROPERTY()
	AArrow* FiredArrow = nullptr;

	UPROPERTY()
	ABow* CurrentBow = nullptr;

	UPROPERTY()
	ACharacter* Owner = nullptr;

	bool bIsAiming = false;

	FRotator InitialRotationRate;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	void EquipBow();

	UFUNCTION()
	void SpawnArrow();

	UFUNCTION()
	void DestroyArrow();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintPure)
	bool GetIsAiming();

	UFUNCTION()
	void AimBegin();

	UFUNCTION()
	void AimEnd();

	UFUNCTION()
	void FireArrowBegin();
};
