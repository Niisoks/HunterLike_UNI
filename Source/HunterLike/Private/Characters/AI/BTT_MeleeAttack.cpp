// BTT_MeleeAttack.cpp

#include "Characters/AI/BTT_MeleeAttack.h"
#include "Navigation/PathFollowingComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Interfaces/Fighter.h"
#include "GameFramework/Character.h"
#include "Characters/EEnemyState.h"

EBTNodeResult::Type UBTT_MeleeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* Nodememory)
{
	bIsFinished = false;
	OwnerCompRef = &OwnerComp; 

	float Distance{
		OwnerComp.GetBlackboardComponent()->GetValueAsFloat(TEXT("Distance"))
	};

	AAIController* AIRef{ OwnerComp.GetAIOwner() };

	if (Distance > AttackRadius)
	{
		APawn* PlayerRef{ GetWorld()->GetFirstPlayerController()->GetPawn() };
		FAIMoveRequest MoveRequest{ PlayerRef };
		MoveRequest.SetUsePathfinding(true);
		MoveRequest.SetAcceptanceRadius(AcceptableRadius);

		AIRef->ReceiveMoveCompleted.AddUnique(MoveDelegate);

		AIRef->MoveTo(MoveRequest);
		AIRef->SetFocus(PlayerRef);
	}
	else
	{
		PlayPreAttackAnimation();
		// Start the pre-attack anticipation timer
		AIRef->GetWorld()->GetTimerManager().SetTimer(
			PreAttackHandle,
			this,
			&UBTT_MeleeAttack::StartAttack,
			PreAttackDelay,
			false
		);
	}

	return EBTNodeResult::InProgress;
}

void UBTT_MeleeAttack::PlayPreAttackAnimation()
{
	AAIController* AIRef = OwnerCompRef ? OwnerCompRef->GetAIOwner() : nullptr;
	ACharacter* Character = AIRef ? AIRef->GetCharacter() : nullptr;

	if (FMath::RandBool())
	{
		AActor* PlayerRef = GetWorld()->GetFirstPlayerController()->GetPawn();
		FRotator LookAt = (PlayerRef->GetActorLocation() - AIRef->GetPawn()->GetActorLocation()).Rotation();
		AIRef->GetPawn()->SetActorRotation(FRotator(0.f, LookAt.Yaw, 0.f));
	}

	if (Character && PreAttackMontage)
	{
		UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
		if (AnimInstance && !AnimInstance->Montage_IsPlaying(PreAttackMontage))
		{
			AnimInstance->Montage_Play(PreAttackMontage);
		}
	}
}

void UBTT_MeleeAttack::StartAttack()
{
	if (!OwnerCompRef) return;

	AAIController* AIRef = OwnerCompRef->GetAIOwner();
	IFighter* FighterRef = Cast<IFighter>(AIRef->GetCharacter());

	if (!FighterRef) return;

	FighterRef->Attack();

	AIRef->GetCharacter()->GetWorldTimerManager().SetTimer(
		AttackTimerHandle,
		this,
		&UBTT_MeleeAttack::FinishAttackTask,
		FighterRef->GetAnimDuration(),
		false
	);
}

void UBTT_MeleeAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* Nodememory, float DeltaSeconds)
{
	float Distance{ OwnerComp.GetBlackboardComponent()->GetValueAsFloat(TEXT("Distance")) };

	AAIController* AIRef{ OwnerComp.GetAIOwner() };

	IFighter* FighterRef{ Cast<IFighter>(AIRef->GetCharacter()) };

	if (Distance > FighterRef->GetMeleeRange())
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsEnum(
			TEXT("CurrentState"), EEnemyState::Range
		);

		AbortTask(OwnerComp, Nodememory);

		FinishLatentTask(OwnerComp, EBTNodeResult::Aborted);

		AIRef->StopMovement();
		AIRef->ClearFocus(EAIFocusPriority::Gameplay);

		AIRef->ReceiveMoveCompleted.Remove(MoveDelegate);
	}

	if (!bIsFinished) return;

	OwnerComp.GetAIOwner()->ReceiveMoveCompleted.Remove(MoveDelegate);

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}

UBTT_MeleeAttack::UBTT_MeleeAttack()
{
	MoveDelegate.BindUFunction(this, "FinishAttackTask");

	bNotifyTick = true;

	PreAttackDelay = 0.6f;
}

void UBTT_MeleeAttack::FinishAttackTask()
{
	bIsFinished = true;
}
