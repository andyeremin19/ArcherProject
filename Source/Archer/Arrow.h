// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Arrow.generated.h"

class UBoxComponent;
class UCameraComponent;
class UNiagaraSystem;
class UNiagaraComponent;
class UProjectileMovementComponent;

UCLASS()
class ARCHER_API AArrow : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AArrow();

	//Timers

	FTimerHandle TimerHamdle_DestroyVFX;

	//Components

protected:

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* CollisionComp = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Mesh = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* Trail = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UCameraComponent* Camera = nullptr;

	//Variables

protected:

	UPROPERTY(EditAnywhere, Category = "ArrowSettings")
	UNiagaraSystem* FlyVFX = nullptr;

	UPROPERTY(EditAnywhere, Category = "ArrowSettings")
	UParticleSystem* HitVFX = nullptr;

	UPROPERTY(EditAnywhere, Category = "ArrowSettings")
	USoundBase* FlySound = nullptr;

	UPROPERTY(EditAnywhere, Category = "ArrowSettings")
	USoundBase* HitSound = nullptr;

	UPROPERTY(EditAnywhere, Category = "ArrowSettings")
	float MaxSpeed = 0.0f;

	UPROPERTY(EditAnywhere, Category = "ArrowSettings")
	float MinSpeed = 0.0f;

	UPROPERTY(EditAnywhere, Category = "ArrowSettings")
	float MaxGravity = 0.0f;

	UPROPERTY(EditAnywhere, Category = "ArrowSettings")
	float MinGravity = 0.0f;

	UPROPERTY(EditAnywhere, Category = "ArrowSettings")
	float Lifespan = 0.0f;

private:

	UPROPERTY()
	UProjectileMovementComponent* ProjectileComp = nullptr;

	FVector ArrowVelocity;

	UPROPERTY()
	UNiagaraComponent* SpawnedVFX = nullptr;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void DestroySpawnedVFX();

	UFUNCTION()
	void ArrowBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Fire(FVector Direction, float Strength);

};
