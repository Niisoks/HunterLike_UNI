// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/AI/BTT_RangedAttack.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/EEnemyState.h"

EBTNodeResult::Type UBTT_RangedAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ACharacter* CharacterRef{ 
		OwnerComp.GetAIOwner()->GetPawn<ACharacter>()
	};

	if (!IsValid(CharacterRef)) { return EBTNodeResult::Failed; }

	CharacterRef->PlayAnimMontage(AnimMontage);

	double RandomValue{ UKismetMathLibrary::RandomFloat() };

	if (RandomValue > Threshold) {
		Threshold = 0.9;

		OwnerComp.GetBlackboardComponent()
			->SetValueAsEnum(
				TEXT("CurrentState"),
				EEnemyState::Charge
			);
	}
	else {
		Threshold -= 0.1;
	}

	return EBTNodeResult::Succeeded;
}
