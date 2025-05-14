// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/CombatComponent.h"
#include "Characters/MainCharacter.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "Interfaces/MainPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	CharacterRef = GetOwner<ACharacter>();

	// ...
	
}


// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(bWasCharging){ UE_LOG(LogTemp, Warning, TEXT("B WAS"));  }

	if (bIsCharging) { UE_LOG(LogTemp, Warning, TEXT("B IS")); }
	

	if (bWasCharging)
	{
		if (CharacterRef && CharacterRef->IsA(AMainCharacter::StaticClass()))
		{
			AMainCharacter* MyChar = Cast<AMainCharacter>(CharacterRef);
			if (MyChar)
			{
				MyChar->DrainStaminaWhileCharging(DeltaTime);

				if (CharacterRef->Implements<UMainPlayer>())
				{
					IMainPlayer* IPlayerRef = Cast<IMainPlayer>(CharacterRef);
					if (IPlayerRef && !IPlayerRef->HasEnoughStamina(1.0f))
					{
						UE_LOG(LogTemp, Warning, TEXT("Cancelling charge due to low stamina"));

						if (CharacterRef->GetMesh()->GetAnimInstance()->Montage_IsPlaying(nullptr))
						{
							CharacterRef->StopAnimMontage();
						}

						CharacterRef->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
						KillCharge();
					}
				}
			}
		}
	}
}

void UCombatComponent::ComboAttack()
{
	if (CharacterRef->Implements<UMainPlayer>()) {
		IMainPlayer* IPlayerRef{ Cast<IMainPlayer>(CharacterRef) };
		if (IPlayerRef && !IPlayerRef->HasEnoughStamina(StaminaCost)) {
			return;
		}
	}

	if (!bCanAttack) { return; }
	bCanAttack = false;
	KillCharge();

	CharacterRef->PlayAnimMontage(AttackAnimations[ComboCounter]);

	ComboCounter++;

	int MaxCombo{ AttackAnimations.Num() };

	ComboCounter = UKismetMathLibrary::Wrap(
		ComboCounter,
		-1,
		(MaxCombo - 1)
	);

	OnAttackPerformedDelegate.Broadcast(StaminaCost);
}

void UCombatComponent::HandleResetAttack()
{
	bCanAttack = true;
	CharacterRef->GetCharacterMovement()->SetMovementMode(MOVE_Walking);

}

void UCombatComponent::HandleResetCombo()
{
	bCanAttack = true;
	ComboCounter = 0;
}

void UCombatComponent::RandomAttack()
{
	int RandomIndex{
		FMath::RandRange(0, AttackAnimations.Num() - 1)
	};

	AnimDuration = CharacterRef->PlayAnimMontage(AttackAnimations[RandomIndex]);
}

float UCombatComponent::GetChargeDuration() const
{
	if (!bIsCharging) return 0.f;

	return GetWorld()->GetTimeSeconds() - ChargeStartTime;
}

void UCombatComponent::StartChargeAttack()
{
	if (!bCanAttack || bIsCharging || !CharacterRef)
	{
		KillCharge();
		return;
	}

	bWasCharging = true;
	bIsCharging = true;
	ChargeStartTime = GetWorld()->GetTimeSeconds();

	CharacterRef->GetCharacterMovement()->DisableMovement();
	UAnimInstance* AnimInstance = CharacterRef->GetMesh()->GetAnimInstance();
	if (AnimInstance && ChargeStartMontage) // Replace with your animation
	{
		float FrameTime = 67.0f / 30.0f;
		// This freezes the character at the first frame of the animation
		AnimInstance->Montage_Play(ChargeStartMontage, 0.0f);
		AnimInstance->Montage_SetPosition(ChargeStartMontage, FrameTime); // Set to specific frame
	}
}

float UCombatComponent::StopChargeAttack()
{
	if (!CharacterRef || !bIsCharging) return 0.f;
	bWasCharging = false;
	bCanAttack = false;
	float Elapsed = GetWorld()->GetTimeSeconds() - ChargeStartTime;

	if (ChargeReleaseMontage)
	{
		float MontageDuration = CharacterRef->PlayAnimMontage(ChargeReleaseMontage);

		// Broadcast after the montage has finished playing
		if (MontageDuration > 0.f)
		{
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(
				TimerHandle,
				[this]()
				{
					OnChargeAttackFinished.Broadcast();
					bIsCharging = false;
					bCanAttack = true;
				},
				MontageDuration,
				false
			);
		}
		else
		{
			OnChargeAttackFinished.Broadcast();
			bIsCharging = false;
			bCanAttack = true;
		}
	}
	else
	{
		OnChargeAttackFinished.Broadcast();
		bIsCharging = false;
		bCanAttack = true;
	}

	return Elapsed;
}

bool UCombatComponent::GetIsCharging()
{
	return bIsCharging;
}

bool UCombatComponent::GetCanAttack()
{
	return bCanAttack;
}

void UCombatComponent::KillCharge()
{
	bIsCharging = false;
	bWasCharging = false;
}
