// Fill out your copyright notice in the Description page of Project Settings.


#include "Arrow.h"
#include "Components/BoxComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

// Sets default values
AArrow::AArrow()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionComp);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(CollisionComp);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CollisionComp);

	Trail = CreateDefaultSubobject<USceneComponent>(TEXT("Trail"));
	Trail->SetupAttachment(Mesh);
}

// Called when the game starts or when spawned
void AArrow::BeginPlay()
{
	Super::BeginPlay();

	CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AArrow::ArrowBeginOverlap);
}

void AArrow::DestroySpawnedVFX()
{
	if (SpawnedVFX)
	{
		SpawnedVFX->DestroyComponent();
	}
}

void AArrow::ArrowBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (GetInstigator() != OtherActor)
	{
		if (ProjectileComp)
		{
			ProjectileComp->DestroyComponent();
		}
		CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		AttachToComponent(SweepResult.GetComponent(), FAttachmentTransformRules::KeepWorldTransform, TEXT("None"));
		if (HitSound)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, GetActorLocation());
		}
		if (SweepResult.GetComponent()->IsSimulatingPhysics())
		{
			SweepResult.GetComponent()->AddImpulse(ArrowVelocity, TEXT("None"), true);
		}
		if (HitVFX)
		{
			FTransform Transform;
			Transform.SetLocation(SweepResult.Location);
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitVFX, Transform);
		}
		GetWorld()->GetTimerManager().SetTimer(TimerHamdle_DestroyVFX, this, &AArrow::DestroySpawnedVFX, 1.0f);
	}
}

// Called every frame
void AArrow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AArrow::Fire(FVector Direction, float Strength)
{
	const FVector Velocity = FMath::Lerp(MinSpeed, MaxSpeed, Strength) * Direction;
	ProjectileComp = NewObject<UProjectileMovementComponent>(this);
	ProjectileComp->RegisterComponent();
	ProjectileComp->bRotationFollowsVelocity = true;
	ProjectileComp->ProjectileGravityScale = 0.5f;
	ProjectileComp->Velocity = Velocity;
	ArrowVelocity = ProjectileComp->Velocity;
	ProjectileComp->ProjectileGravityScale = FMath::Lerp(MaxGravity, MinGravity, Strength);
	if (FlySound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), FlySound, GetActorLocation());
	}
	if (FlyVFX)
	{
		const FVector Location (0.0, 0.0, 0.0);
		const FRotator Rotation (0.0, 0.0, 0.0);
		SpawnedVFX = UNiagaraFunctionLibrary::SpawnSystemAttached(FlyVFX, Trail, "None", Location, Rotation, EAttachLocation::KeepRelativeOffset, false);
	}
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

