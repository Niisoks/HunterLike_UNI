// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_MeleeAttack.generated.h"

/**
 * 
 */
UCLASS()
class HUNTERLIKE_API UBTT_MeleeAttack : public UBTTaskNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float AttackRadius{ 200.0f };

	UPROPERTY(EditAnywhere)
	float AcceptableRadius{ 100.0f };

	FScriptDelegate MoveDelegate;

	bool bIsFinished{ false };

protected:
	virtual EBTNodeResult::Type ExecuteTask(
		UBehaviorTreeComponent& OwnerComp, 
		uint8* Nodememory
	) override;

	void PlayPreAttackAnimation();

	virtual void TickTask(
		UBehaviorTreeComponent& OwnerComp,
		uint8* Nodememory,
		float DeltaSeconds
	) override;


	FTimerHandle AttackTimerHandle;
	FTimerHandle PreAttackHandle;

	// Store ref to owner so StartAttack() can access it
	UBehaviorTreeComponent* OwnerCompRef;

	// Pre-attack delay in seconds
	UPROPERTY(EditAnywhere, Category = "Attack")
	float PreAttackDelay;

	// Actual attack after delay
	void StartAttack();


public:

	UBTT_MeleeAttack();

	UFUNCTION()
	void FinishAttackTask();

	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* PreAttackMontage;
	
};
