// Fill out your copyright notice in the Description page of Project Settings.


#include "BT_Block_Genichiro.h"


EBTNodeResult::Type UBT_Block_Genichiro::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	ResetStateTime(OwnerComp);

	if (AnimChangeCheck(OwnerComp))
	{
		return EBTNodeResult::Type::Failed;
	}

	if (GetGenichiroState(OwnerComp) == GenichiroState::Guard
		|| GetGenichiroState(OwnerComp) == GenichiroState::Parrying1
		|| GetGenichiroState(OwnerComp) == GenichiroState::Parrying2)
	{
		Cast<AMonster>(GetGlobalCharacter(OwnerComp))->SetHitState(MonsterHitState::PARRYING);
	}
	else
	{
		Cast<AMonster>(GetGlobalCharacter(OwnerComp))->SetHitState(MonsterHitState::GUARD);
	}

	return EBTNodeResult::Type::InProgress;
}

void UBT_Block_Genichiro::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (AnimChangeCheck(OwnerComp))
	{
		return;
	}

	UObject* TargetObject = GetBlackboardComponent(OwnerComp)->GetValueAsObject(TEXT("TargetActor"));
	AActor* TargetActor = Cast<AActor>(TargetObject);

	if (nullptr != TargetActor)
	{
		GetGlobalCharacter(OwnerComp)->AdjustAngle(DeltaSeconds, TargetActor->GetActorLocation(), 10.0f);
	}
	else
	{
		SetStateChange(OwnerComp, GenichiroState::Idle);
		return;
	}

	GenichiroState BehaviorState = UBTTask_Genichiro::GetGenichiroState(OwnerComp);

	if (BehaviorState == GenichiroState::Guard)
	{
		// 가드 상태에 들어가면 0.6초 동안 가드 유지
		// 이때 공격하면 패링 당한다.
		if (0.6f <= GetStateTime(OwnerComp))
		{
			SetStateChange(OwnerComp, GenichiroState::ForwardRun);
			return;
		}
	}
	else
	{
		UAnimMontage* Montage = GetGlobalCharacter(OwnerComp)->GetAnimMontage(UBTTask_Genichiro::GetGenichiroState(OwnerComp));
		float Time = Montage->CalculateSequenceLength() * (1 / Montage->RateScale);
		if (Time <= GetStateTime(OwnerComp))
		{
			SetStateChange(OwnerComp, GenichiroState::ForwardRun);
			return;
		}
	}
}