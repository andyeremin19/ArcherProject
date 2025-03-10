// Fill out your copyright notice in the Description page of Project Settings.


#include "BowMechanicsComponent.h"
#include "Bow.h"
#include "Arrow.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

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
	if (ArrowType && !DrawnArrow)
	{
		FActorSpawnParameters Params;
		Params.Instigator = Owner;
		DrawnArrow = GetWorld()->SpawnActor<AArrow>(ArrowType, FVector(0.0, 0.0, 0.0), FRotator(0.0, 0.0, 0.0), Params);
		DrawnArrow->AttachToComponent(Owner->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("ArrowSocket"));
	}
}

void UBowMechanicsComponent::DestroyArrow()
{
	if (DrawnArrow)
	{
		DrawnArrow->Destroy();
		DrawnArrow = nullptr;
	}
}

bool UBowMechanicsComponent::GetIsAiming()
{
	return bIsAiming;
}

bool UBowMechanicsComponent::GetIsDrawing()
{
	return bIsDrawing;
}

void UBowMechanicsComponent::AimBegin()
{
	bIsAiming = true;
	Owner->GetCharacterMovement()->bOrientRotationToMovement = false;
	Owner->GetCharacterMovement()->bUseControllerDesiredRotation = true;
	Owner->GetCharacterMovement()->RotationRate = CustomRotationRate;
	CurrentBow->SetBowState(EBowState::Aim);
	SpawnArrow();
	OnAimBegin.Broadcast();
}

void UBowMechanicsComponent::AimEnd()
{
	bIsAiming = false;
	DrawEnd();
	Owner->GetCharacterMovement()->bOrientRotationToMovement = true;
	Owner->GetCharacterMovement()->bUseControllerDesiredRotation = false;
	Owner->GetCharacterMovement()->RotationRate = InitialRotationRate;
	CurrentBow->SetBowState(EBowState::Idle);
	DestroyArrow();
	OnAimEnd.Broadcast();
}

void UBowMechanicsComponent::FireArrowBegin(const FVector& Direction)
{
	if (DrawnArrow && bIsDrawing)
	{
		bIsFiring = true;
		FireAimedArrow(Direction);
		DrawEnd();
		if (FireBowMontage)
		{
			Owner->GetMesh()->GetAnimInstance()->Montage_Play(FireBowMontage);
			OnFireMontageEnd.BindUObject(this, &UBowMechanicsComponent::FireArrowEnd);
			Owner->GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(OnFireMontageEnd, FireBowMontage);
		}
	}
}

void UBowMechanicsComponent::FireArrowEnd(UAnimMontage* Montage, bool IsIterrupted)
{
	bIsFiring = false;
	if (bIsAiming)
	{
		AimBegin();
		if (bIsWaitingToDrawBow)
		{
			DrawBegin();
		}
	}
}

void UBowMechanicsComponent::DrawBegin_Implementation()
{
	if (!bIsDrawing && bIsAiming && !bIsFiring)
	{
		bIsWaitingToDrawBow = false;
		bIsDrawing = true;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_Drawing, this, &UBowMechanicsComponent::IncrementDrawTime, DrawIncrementTime, true);
		CurrentBow->DrawBegin();
	}
	else
	{
		bIsWaitingToDrawBow = true;
	}
}

void UBowMechanicsComponent::Server_DrawBegin_Implementation()
{
	DrawBegin();
}

void UBowMechanicsComponent::DrawEnd()
{
	bIsDrawing = false;
	bIsWaitingToDrawBow = false;
	DrawTime = 0.0f;
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Drawing);
	CurrentBow->DrawEnd();
	OnDrawEnd.Broadcast();
}

void UBowMechanicsComponent::IncrementDrawTime()
{
	DrawTime += DrawIncrementTime;
	OnDrawOngoing.Broadcast(CurrentBow->GetMaxDrawTime(), DrawTime);
}

void UBowMechanicsComponent::FireAimedArrow(const FVector& Direction)
{
	DrawnArrow->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	const float Strength = FMath::Clamp(UKismetMathLibrary::NormalizeToRange(DrawTime, 0.0, CurrentBow->GetMaxDrawTime()), 0.0, 1.0);
	DrawnArrow->Fire(Direction, Strength);
	FiredArrow = DrawnArrow;
	DrawnArrow = nullptr;
}

AArrow* UBowMechanicsComponent::GetDrawnArrow()
{
	return DrawnArrow;
}

AArrow* UBowMechanicsComponent::GetFiredArrow()
{
	return FiredArrow;
}

                                                   