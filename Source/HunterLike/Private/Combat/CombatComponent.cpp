// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/CombatComponent.h"
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

	// ...
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
	if (!CharacterRef || bIsCharging) return;

	bIsCharging = true;
	ChargeStartTime = GetWorld()->GetTimeSeconds();

	CharacterRef->GetCharacterMovement()->DisableMovement();
	if (ChargeStartMontage)
	{
		CharacterRef->PlayAnimMontage(ChargeStartMontage);
	}
}

float UCombatComponent::StopChargeAttack()
{
	if (!CharacterRef || !bIsCharging) return 0.f;

	bIsCharging = false;

	float Elapsed = GetWorld()->GetTimeSeconds() - ChargeStartTime;

	CharacterRef->GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	if (ChargeReleaseMontage)
	{
		CharacterRef->PlayAnimMontage(ChargeReleaseMontage);
	}

	return Elapsed;
}


