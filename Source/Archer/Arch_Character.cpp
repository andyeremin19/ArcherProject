// Fill out your copyright notice in the Description page of Project Settings.

#include "Arch_Character.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "BowMechanicsComponent.h"

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
	InitValues();
	SetAimTimeline();
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

void AArch_Character::InitValues()
{
	InitialFOV = Camera->FieldOfView;
	InitialCameraOffset = SpringArm->SocketOffset;
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

