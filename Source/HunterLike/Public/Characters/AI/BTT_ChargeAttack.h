// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_ChargeAttack.generated.h"

/**
 * 
 */
UCLASS()
class HUNTERLIKE_API UBTT_ChargeAttack : public UBTTaskNode
{
	GENERATED_BODY()

	AAIController* ControllerRef;

	ACharacter* CharacterRef;

	class UBossAnimInstance* BossAnim;

	UPROPERTY(EditAnywhere)
	float AcceptableRadius{ 100.0f };

	FScriptDelegate MoveCompletedDelegate;

	float OriginalWalkSpeed;

	UPROPERTY(EditAnywhere)

	float ChargeWalkSpeed{ 2000.0f };

	bool bIsFinished{ false };

public:
	virtual EBTNodeResult::Type ExecuteTask(
		UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory
	) override;

	UBTT_ChargeAttack();

	void ChargeAtPlayer();

	UFUNCTION()
	void HandleMoveCompleted();


	UFUNCTION()
	void FinishAttackTask();
	
protected:
	virtual void TickTask(
		UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory,
		float DeltaSeconds
	) override;
};
