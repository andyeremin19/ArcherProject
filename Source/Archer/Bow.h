// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Arch_Types.h"
#include "Bow.generated.h"

UCLASS()
class ARCHER_API ABow : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABow();

	//Components
protected:

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USkeletalMeshComponent* BowMesh = nullptr;

private:

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* SceneComponent = nullptr;

	EBowState BowState = EBowState::Idle;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintPure)
	EBowState GetBowState();

	void SetBowState(EBowState NewState);
};
