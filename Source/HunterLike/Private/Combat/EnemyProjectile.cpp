// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/EnemyProjectile.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/DamageEvents.h"
#include "EngineUtils.h"

// Sets default values
AEnemyProjectile::AEnemyProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemyProjectile::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("ScoreManager spawned and BeginPlay called."));

	for (TActorIterator<AAScoreManager> It(GetWorld()); It; ++It)
	{
		ScoreManager = *It;
		break;
	}
	
}

// Called every frame
void AEnemyProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemyProjectile::HandleBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!OtherActor || !OtherComp)
		return;

	APawn* PawnRef{
		Cast<APawn>(OtherActor)
	};

	if (!PawnRef->IsPlayerControlled()) { return; }


	if (OtherComp->ComponentHasTag("Score"))
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit Score Hitbox! Gaining points."));
		if (ScoreManager)
		{
			ScoreManager->AddScore(100);
		}
		return;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit normal hitbox. Deal damage."));
		// Deal damage
	}
	
	FindComponentByClass<UParticleSystemComponent>()
		->SetTemplate(HitTemplate);

	FindComponentByClass<UProjectileMovementComponent>()
		->StopMovementImmediately();

	FTimerHandle DeathTimerHandle{
	
	};

	GetWorldTimerManager().SetTimer(
		DeathTimerHandle,
		this,
		&AEnemyProjectile::DestroyProjectile,
		0.5f
	);

	FindComponentByClass<USphereComponent>()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	FDamageEvent ProjectileAttackEvent{};

	PawnRef->TakeDamage(
		Damage,
		ProjectileAttackEvent,
		PawnRef->GetController(),
		this
	);
}

void AEnemyProjectile::DestroyProjectile()
{
	Destroy();
}

