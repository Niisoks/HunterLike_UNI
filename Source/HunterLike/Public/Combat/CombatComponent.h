// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_OneParam(
	FOnAttackPerformedSignature,
	UCombatComponent, OnAttackPerformedDelegate,
	float, Amount
);


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChargeAttackFinished);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HUNTERLIKE_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<UAnimMontage*> AttackAnimations;

	bool bIsCharging = false;
	bool bWasCharging = false;

	float ChargeStartTime = 0.f;

	UPROPERTY(EditAnywhere)
	UAnimMontage* ChargeStartMontage;

	UPROPERTY(EditAnywhere)
	UAnimMontage* ChargeReleaseMontage;

	ACharacter* CharacterRef;

	UPROPERTY(VisibleAnywhere)
	int ComboCounter{ 0 };

	UPROPERTY(VisibleAnywhere)
	bool bCanAttack{ true };

	UPROPERTY(EditAnywhere)
	float StaminaCost{ 5.0f };

public:	
	// Sets default values for this component's properties
	UCombatComponent();

	UPROPERTY(BlueprintAssignable)
	FOnAttackPerformedSignature OnAttackPerformedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Combat")
	FOnChargeAttackFinished OnChargeAttackFinished;

	float AnimDuration;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void ComboAttack();

	UFUNCTION(BlueprintCallable)
	void HandleResetAttack();

	UFUNCTION(BlueprintCallable)
	void HandleResetCombo();

	void RandomAttack();

	UFUNCTION(BlueprintCallable)
	float GetChargeDuration() const;

	UFUNCTION(BlueprintCallable)
	void StartChargeAttack();

	void ExecuteStartChargeAttack();

	UFUNCTION(BlueprintCallable)
	float StopChargeAttack();

	UFUNCTION(BlueprintCallable)
	bool GetIsCharging();

	UFUNCTION(BlueprintCallable)
	bool GetCanAttack();

	UFUNCTION(BlueprintCallable)
	void KillCharge();


};
