// Fill out your copyright notice in the Description page of Project Settings.


#include "Arch_HUD.h"
#include "BowMechanicsComponent.h"

void AArch_HUD::BeginPlay()
{
	Super::BeginPlay();

	BindDelegates();
}

void AArch_HUD::BindDelegates()
{
	if (GetOwningPawn()->GetComponentByClass<UBowMechanicsComponent>())
	{
		BowMechComp = GetOwningPawn()->GetComponentByClass<UBowMechanicsComponent>();
		BowMechComp->OnAimBegin.AddDynamic(this, &AArch_HUD::ShowCrosshair);
		BowMechComp->OnAimEnd.AddDynamic(this, &AArch_HUD::HideCrosshair);
	}
}

void AArch_HUD::HideCrosshair_Implementation()
{
}

void AArch_HUD::ShowCrosshair_Implementation()
{
}
