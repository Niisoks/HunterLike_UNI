// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/AI/BTT_RangedAttack.h"
#include "AIController.h"
#include "GameFramework/Character.h"

EBTNodeResult::Type UBTT_RangedAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ACharacter* CharacterRef{ 
		OwnerComp.GetAIOwner()->GetPawn<ACharacter>()
	};

	if (!IsValid(CharacterRef)) { return EBTNodeResult::Failed; }

	CharacterRef->PlayAnimMontage(AnimMontage);
	return EBTNodeResult::Succeeded;
}
