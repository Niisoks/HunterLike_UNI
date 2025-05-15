// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/MainCharacter.h"
#include "Characters/StatsComponent.h"
#include "Characters/EStat.h"
#include "Combat/LockonComponent.h"
#include "Combat/CombatComponent.h"
#include "Combat/TraceComponent.h"
#include "Combat/BlockComponent.h"
#include "Characters/PlayerActionsComponent.h"
#include "Animations/PlayerAnimInstance.h"
#include "Animation/AnimInstance.h"

// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	StatsComp = CreateDefaultSubobject<UStatsComponent>(TEXT("Stats Component"));
	LockonComp = CreateDefaultSubobject<ULockonComponent>(TEXT("Lock On Component"));
	CombatComp = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat Component"));
	TraceComp = CreateDefaultSubobject<UTraceComponent>(TEXT("Trace Component"));
	BlockComp = CreateDefaultSubobject<UBlockComponent>(TEXT("Block Component"));
	PlayerActionsComp = CreateDefaultSubobject<UPlayerActionsComponent>(TEXT("Player Actions Component"));

}

void AMainCharacter::EndLockonWithActor(AActor* ActorRef)
{
	if (LockonComp->CurrentTargetActor != ActorRef) { return;  }
	LockonComp->EndLockon();
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	PlayerAnim = Cast<UPlayerAnimInstance>(
		GetMesh()->GetAnimInstance()
	);

	
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float AMainCharacter::GetDamage()
{
	return StatsComp->Stats[EStat::Strength];
}

bool AMainCharacter::HasEnoughStamina(float Cost)
{
	return StatsComp->Stats[EStat::Stamina] >= Cost;
}

void AMainCharacter::HandleDeath()
{
	PlayAnimMontage(DeathAnimMontage);

	DisableInput(GetController<APlayerController>());
}

bool AMainCharacter::CanTakeDamage(AActor* Opponent)
{
	if (PlayerActionsComp->bIsRollActive) { return false; }
	if (PlayerAnim->bIsBlocking) { return BlockComp->Check(Opponent); }

	return true;
}

void AMainCharacter::PlayHurtAnim(TSubclassOf<class UCameraShakeBase> CameraShakeTemplate, AActor* TargetActor, float Power)
{
	if (PlayerAnim->bIsBlocking || Power < 5.0f) {
		PlayAnimMontage(HurtAnimMontage);

		if (CameraShakeTemplate) {
			GetController<APlayerController>()
				->ClientStartCameraShake(CameraShakeTemplate);
		}
	}
	else {
		if (KnockbackMontage && PlayerAnim)
		{
			bool bigHit = (Power >= 5.0f && FMath::RandRange(1, 2) == 1);
			FOnMontageEnded MontageEndDelegate;
			MontageEndDelegate.BindUObject(this, &AMainCharacter::OnKnockbackMontageEnded);

			UAnimMontage* knockbackMontage = KnockbackMontage;
			if (bigHit) { knockbackMontage = BigKnockbackMontage;}
			
			float Duration = PlayAnimMontage(knockbackMontage);
			PlayerAnim->Montage_SetEndDelegate(MontageEndDelegate, knockbackMontage);


			
			if (CameraShakeTemplate) {
				GetController<APlayerController>()->ClientStartCameraShake(CameraShakeTemplate);
			}

			DisableCharacterControl();
			if (TargetActor)
			{
				FVector DirectionToTarget = TargetActor->GetActorLocation() - GetActorLocation();
				DirectionToTarget.Z = 0; 
				FRotator NewRotation = DirectionToTarget.Rotation();
				SetActorRotation(NewRotation);
				FVector LaunchDirection = GetActorLocation() - TargetActor->GetActorLocation();
				LaunchDirection.Z = 0.f; 
				LaunchDirection.Normalize();

				float KnockbackStrength = 100.f * Power; 
				FVector LaunchVelocity = LaunchDirection * KnockbackStrength + FVector(0, 0, 200.f); 

				if (bigHit) {
					KnockbackStrength = 200.f * Power;
					LaunchVelocity = LaunchDirection * KnockbackStrength + FVector(0, 0, 400.f);
				}
				LaunchCharacter(LaunchVelocity, true, true); 
			}
		}
	}
}

void AMainCharacter::OnKnockbackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	EnableCharacterControl();
}

void AMainCharacter::DisableCharacterControl()
{
	
	APlayerController* PC = GetController<APlayerController>();
	if (PC)
	{
		bInKnockback = true;
		// Save camera control and only disable movement and actions
		PC->SetIgnoreMoveInput(true);
		PC->SetIgnoreLookInput(false); // Allow camera movement
	}
	if (PlayerActionsComp)
	{
		PlayerActionsComp->SetComponentTickEnabled(false); // Or custom logic to disable actions
	}
}

void AMainCharacter::EnableCharacterControl()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		bInKnockback = false;
		PC->SetIgnoreMoveInput(false);
		PC->SetIgnoreLookInput(false);
	}
	if (PlayerActionsComp)
	{
		PlayerActionsComp->SetComponentTickEnabled(true); // Or re-enable logic
	}
}

void AMainCharacter::SetDamage(float Damage)
{
	StatsComp->Stats[EStat::Strength] = Damage;
}

bool AMainCharacter::HandleSheath()
{
	bIsSheathed = !bIsSheathed;
	return bIsSheathed;
}

