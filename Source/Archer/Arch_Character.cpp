// Fill out your copyright notice in the Description page of Project Settings.

#include "Arch_Character.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "BowMechanicsComponent.h"
#include "Arrow.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AArch_Character::AArch_Character()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, TEXT("None"));

	BowMechComp = CreateDefaultSubobject<UBowMechanicsComponent>(TEXT("BowMechanicsComp"));
	BowMechComp->SetIsReplicated(true);

	AimTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("AimTimeLine"));
}

// Called when the game starts or when spawned
void AArch_Character::BeginPlay()
{
	Super::BeginPlay();
	InitVariables();
	SetAimTimeline();
}

void AArch_Character::SwitchToArrow()
{
	//UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.3f);  Skipped it for the multiplayer
	if (BowMechComp->GetFiredArrow() && !BowMechComp->GetFiredArrow()->GetbIsStuck())
	{
		OnCameraSwitchBegin.Broadcast();
		APlayerController* MyController = Cast<APlayerController>(GetController());
		MyController->SetViewTargetWithBlend(BowMechComp->GetFiredArrow());
	}
}

void AArch_Character::SwitchToChar()
{
	//UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.3f); Skipped it for the multiplayer
	OnCameraSwitchEnd.Broadcast();
	APlayerController* MyController = Cast<APlayerController>(GetController());
	MyController->SetViewTargetWithBlend(this);
}

// Called every frame
void AArch_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AArch_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	SetMappingContext();
	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	Input->BindAction(LookAroundAction, ETriggerEvent::Triggered, this, &AArch_Character::LookAround);
	Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AArch_Character::Move);
	Input->BindAction(AimAction, ETriggerEvent::Started, this, &AArch_Character::Server_StartAiming);
	Input->BindAction(AimAction, ETriggerEvent::Triggered, this, &AArch_Character::Server_EndAiming);
	Input->BindAction(FireAction, ETriggerEvent::Started, BowMechComp, &UBowMechanicsComponent::Server_DrawBegin);
	Input->BindAction(FireAction, ETriggerEvent::Triggered, this, &AArch_Character::Server_FireArrow);
	Input->BindAction(SwitchCameraAction, ETriggerEvent::Started, this, &AArch_Character::SwitchToArrow);
	Input->BindAction(SwitchCameraAction, ETriggerEvent::Triggered, this, &AArch_Character::SwitchToChar);
}

void AArch_Character::SetMappingContext()
{
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (InputMappingContext)
			{
				InputSystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
}

void AArch_Character::Move(const FInputActionInstance& Instance)
{
	const FVector2D Vector2D = Instance.GetValue().Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.0, Rotation.Yaw, 0.0);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X); // need to think about it
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(ForwardDirection, Vector2D.Y);
	AddMovementInput(RightDirection, Vector2D.X);
}

void AArch_Character::LookAround(const FInputActionInstance& Instance)
{
	const FVector2D Vector2D = Instance.GetValue().Get<FVector2D>();
	AddControllerYawInput(Vector2D.X);
	AddControllerPitchInput(Vector2D.Y);
}

void AArch_Character::FireArrow_Implementation()
{
	BowMechComp->FireArrowBegin(CalculateAimDirection());
}

void AArch_Character::Server_FireArrow_Implementation()
{
	FTransform Transform = Camera->GetComponentTransform();
	UpdateCameraTransform(Transform);
	FireArrow();
}

void AArch_Character::Server_StartAiming_Implementation()
{
	StartAiming();
}

void AArch_Character::Server_EndAiming_Implementation()
{
	EndAiming();
}

void AArch_Character::StartAiming_Implementation()
{
	AimTimeline->Play();
	BowMechComp->AimBegin();
}

void AArch_Character::EndAiming_Implementation()
{
	AimTimeline->Reverse();
	BowMechComp->AimEnd();
}

void AArch_Character::InitVariables()
{
	InitialFOV = Camera->FieldOfView;
	InitialCameraOffset = SpringArm->SocketOffset;
}

void AArch_Character::UpdateCameraTransform_Implementation(FTransform Transform)
{
	if (!IsLocallyControlled())
	{
		Camera->SetWorldTransform(Transform);
	}
}

void AArch_Character::SetCameraByTimeline(float Alpha)
{
	Camera->FieldOfView = FMath::Lerp(InitialFOV, CustomFOV, Alpha);
	SpringArm->SocketOffset = FMath::Lerp(InitialCameraOffset, CustomCameraOffset, Alpha);
}

void AArch_Character::SetAimTimeline()
{
	OnAimTimeline.BindUFunction(this, TEXT("SetCameraByTimeline"));
	if (AimingCurve)
	{
		AimTimeline->AddInterpFloat(AimingCurve, OnAimTimeline);
		AimTimeline->SetIgnoreTimeDilation(false);
	}
}

FVector AArch_Character::CalculateAimDirection()
{
	FVector Direction;
	const FVector Start = Camera->GetComponentLocation();
	const FVector End = (Camera->GetForwardVector() * 10000.f) + Camera->GetComponentLocation();
	FHitResult Hit;
	GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECollisionChannel::ECC_Visibility);
	if (BowMechComp->GetDrawnArrow())
	{
		if (Hit.bBlockingHit)
		{
			Direction = (Hit.Location - BowMechComp->GetDrawnArrow()->GetActorLocation()).GetSafeNormal();
		}
		else
		{
			Direction = (Hit.TraceEnd - BowMechComp->GetDrawnArrow()->GetActorLocation()).GetSafeNormal();
		}
	}
	return Direction;
}

