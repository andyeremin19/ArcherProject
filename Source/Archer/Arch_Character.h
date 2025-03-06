// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "GameFramework/Character.h"
#include "Components/TimelineComponent.h"
#include "Arch_Character.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UInputMappingContext;
class UBowMechanicsComponent;
class UCurveFloat;

UCLASS()
class ARCHER_API AArch_Character : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AArch_Character();

	//Delegates

	FOnTimelineFloat OnAimTimeline;

	//Components

protected:
	
	UPROPERTY(EditAnywhere, Category = "Archer|Input")
	UInputMappingContext* InputMappingContext = nullptr;

	UPROPERTY(EditAnywhere, Category = "Archer|Input")
	UInputAction* MoveAction = nullptr;

	UPROPERTY(EditAnywhere, Category = "Archer|Input")
	UInputAction* LookAroundAction = nullptr;

	UPROPERTY(EditAnywhere, Category = "Archer|Input")
	UInputAction* AimAction = nullptr;

	UPROPERTY(EditAnywhere, Category = "Archer|Input")
	UInputAction* FireAction = nullptr;

private:

	UPROPERTY()
	UTimelineComponent* AimTimeline = nullptr;

	UPROPERTY(EditAnywhere, Category = "Components")
	UBowMechanicsComponent* BowMechComp = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UCameraComponent* Camera = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USpringArmComponent* SpringArm = nullptr;

	//Variables

protected:

	UPROPERTY(EditAnywhere, Category = "Archer|CameraSettings")
	UCurveFloat* AimingCurve = nullptr;

	UPROPERTY(EditAnywhere, Category = "Archer|CameraSettings")
	float CustomFOV = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Archer|CameraSettings")
	FVector CustomCameraOffset;

private:

	float InitialFOV = 0.0f;

	FVector InitialCameraOffset;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FVector CalculateAimDirection();

private:

	void SetMappingContext();

	void Move(const FInputActionInstance& Instance);

	void LookAround(const FInputActionInstance& Instance);

	UFUNCTION(Server, Unreliable)
	void Server_FireArrow();

	UFUNCTION(NetMulticast, Unreliable)
	void FireArrow();

	UFUNCTION(Server, Unreliable)
	void Server_StartAiming();

	UFUNCTION(Server, Unreliable)
	void Server_EndAiming();
	
	UFUNCTION(NetMulticast, Unreliable)
	void StartAiming();
	
	UFUNCTION(NetMulticast, Unreliable)
	void EndAiming();

	void InitVariables();

	UFUNCTION(NetMulticast, Unreliable)
	void UpdateCameraTransform(FTransform Transform);

	//Changes camera loc&fov 
	UFUNCTION()
	void SetCameraByTimeline(float Alpha);

	//Adjusts AimTimeline
	void SetAimTimeline();

};
