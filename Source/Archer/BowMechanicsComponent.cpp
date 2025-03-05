// Fill out your copyright notice in the Description page of Project Settings.


#include "BowMechanicsComponent.h"
#include "Bow.h"
#include "Arrow.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
UBowMechanicsComponent::UBowMechanicsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UBowMechanicsComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner = Cast<ACharacter>(GetOwner());
	EquipBow();
	InitialRotationRate = Owner->GetCharacterMovement()->RotationRate;
}


// Called every frame
void UBowMechanicsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UBowMechanicsComponent::EquipBow()
{
	if (BowType)
	{
		FActorSpawnParameters Params;
		CurrentBow = GetWorld()->SpawnActor<ABow>(BowType, FVector(0.0, 0.0, 0.0), FRotator(0.0, 0.0, 0.0), Params);
		CurrentBow->AttachToComponent(Owner->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("BowSocket"));
	}
}

void UBowMechanicsComponent::SpawnArrow()
{
	if (ArrowType)
	{
		FActorSpawnParameters Params;
		DrawnArrow = GetWorld()->SpawnActor<AArrow>(ArrowType, FVector(0.0, 0.0, 0.0), FRotator(0.0, 0.0, 0.0), Params);
		DrawnArrow->AttachToComponent(Owner->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("ArrowSocket"));
	}
}

void UBowMechanicsComponent::DestroyArrow()
{
	if (DrawnArrow)
	{
		DrawnArrow->Destroy();
	}
}

bool UBowMechanicsComponent::GetIsAiming()
{
	return bIsAiming;
}

void UBowMechanicsComponent::AimBegin()
{
	bIsAiming = true;
	Owner->GetCharacterMovement()->bOrientRotationToMovement = false;
	Owner->GetCharacterMovement()->bUseControllerDesiredRotation = true;
	Owner->GetCharacterMovement()->RotationRate = CustomRotationRate;
	CurrentBow->SetBowState(EBowState::Aim);
	SpawnArrow();
}

void UBowMechanicsComponent::AimEnd()
{
	bIsAiming = false;
	Owner->GetCharacterMovement()->bOrientRotationToMovement = true;
	Owner->GetCharacterMovement()->bUseControllerDesiredRotation = false;
	Owner->GetCharacterMovement()->RotationRate = InitialRotationRate;
	CurrentBow->SetBowState(EBowState::Idle);
	DestroyArrow();
}

void UBowMechanicsComponent::FireArrowBegin()
{
	if (DrawnArrow)
	{
		DrawnArrow->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		//DrawnArrow->Fire();
	}
}

                                                   