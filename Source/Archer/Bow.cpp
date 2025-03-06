// Fill out your copyright notice in the Description page of Project Settings.


#include "Bow.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

// Sets default values
ABow::ABow()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = SceneComponent;

	BowMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BowMesh"));
	BowMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ABow::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

EBowState ABow::GetBowState()
{
	return BowState;
}

void ABow::SetBowState(EBowState NewState)
{
	BowState = NewState;
}

float ABow::GetMaxDrawTime()
{
	return MaxDrawTime;
}

void ABow::DrawBegin()
{
	BowState = EBowState::Draw;
	if (DrawSound)
	{
		SpawnedDrawSound = UGameplayStatics::SpawnSoundAtLocation(GetWorld(), DrawSound, GetActorLocation());
	}
}

void ABow::DrawEnd()
{
	BowState = EBowState::Idle;
	if (SpawnedDrawSound)
	{
		SpawnedDrawSound->DestroyComponent();
	}
}