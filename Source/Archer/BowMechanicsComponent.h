// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BowMechanicsComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAimBegin);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAimEnd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDrawEnd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDrawOngoing, float, MaxDrawTime, float, DrawTime);


class ABow;
class AArrow;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARCHER_API UBowMechanicsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBowMechanicsComponent();

	//Timers

	FTimerHandle TimerHandle_Drawing;

	//Delegates

	FOnMontageEnded OnFireMontageEnd;
	FOnAimBegin OnAimBegin;
	FOnAimEnd OnAimEnd;
	FOnDrawOngoing OnDrawOngoing;
	FOnAimEnd OnDrawEnd;

	//Variables

protected:

	UPROPERTY(EditAnywhere, Category = "Bow")
	TSubclassOf<ABow> BowType;

	UPROPERTY(EditAnywhere, Category = "Bow")
	UAnimMontage* FireBowMontage = nullptr;

	UPROPERTY(EditAnywhere, Category = "Arrow")
	TSubclassOf<AArrow> ArrowType;

	UPROPERTY(EditAnywhere, Category = "CameraSettings")
	FRotator CustomRotationRate;

	UPROPERTY(EditAnywhere, Category = "TimerSettings")
	float DrawIncrementTime = 0.033f;

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

	bool bIsDrawing = false;

	bool bIsFiring = false;

	FRotator InitialRotationRate;

	float DrawTime = 0.0f;

	bool bIsWaitingToDrawBow = false;

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

	UFUNCTION(BlueprintPure)
	bool GetIsDrawing();

	UFUNCTION()
	void AimBegin();

	UFUNCTION()
	void AimEnd();

	UFUNCTION(NetMulticast, Unreliable)
	void FireArrowBegin(const FVector& Direction);

	UFUNCTION()
	void FireArrowEnd(UAnimMontage* Montage, bool IsIterrupted);

	UFUNCTION(Server, Unreliable)
	void Server_DrawBegin();

	UFUNCTION(NetMulticast, Unreliable)
	void DrawBegin();

	void DrawEnd();

	void IncrementDrawTime();

	void FireAimedArrow(const FVector& Direction);

	UFUNCTION()
	AArrow* GetDrawnArrow();
};
