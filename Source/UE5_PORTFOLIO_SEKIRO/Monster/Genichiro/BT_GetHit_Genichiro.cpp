// Fill out your copyright notice in the Description page of Project Settings.


#include "BT_GetHit_Genichiro.h"
#include "BT_Idle_Genichiro.h"


EBTNodeResult::Type UBT_GetHit_Genichiro::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	ResetStateTime(OwnerComp);

	if (AnimChangeCheck(OwnerComp))
	{
		return EBTNodeResult::Type::Failed;
	}

	if (GetGenichiroState(OwnerComp) == GenichiroState::MikiriCounter1
		|| GetGenichiroState(OwnerComp) == GenichiroState::MikiriCounter2)
	{
		Cast<AMonster>(GetGlobalCharacter(OwnerComp))->SetHitState(MonsterHitState::OFFGUARD);
	}
	else if (GetGenichiroState(OwnerComp) == GenichiroState::Shock)
	{
		Cast<AMonster>(GetGlobalCharacter(OwnerComp))->SetHitState(MonsterHitState::SUPERARMOR);
	}
	else
	{
		Cast<AMonster>(GetGlobalCharacter(OwnerComp))->SetHitState(MonsterHitState::GUARD);
	}

	return EBTNodeResult::Type::InProgress;
}

void UBT_GetHit_Genichiro::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (AnimChangeCheck(OwnerComp))
	{
		return;
	}

	UObject* TargetObject = GetBlackboardComponent(OwnerComp)->GetValueAsObject(TEXT("TargetActor"));
	AActor* TargetActor = Cast<AActor>(TargetObject);

	if (nullptr == TargetActor)
	{
		SetStateChange(OwnerComp, GenichiroState::Idle);
		return;
	}

	UAnimMontage* Montage = GetGlobalCharacter(OwnerComp)->GetAnimMontage(UBTTask_Genichiro::GetGenichiroState(OwnerComp));
	float Time = Montage->CalculateSequenceLength() * (1 / Montage->RateScale);
	if (Time <= GetStateTime(OwnerComp))
	{
		GenichiroState BehaviorState = UBTTask_Genichiro::GetGenichiroState(OwnerComp);

		if (BehaviorState == GenichiroState::MikiriCounter1)
		{
			SetStateChange(OwnerComp, GenichiroState::MikiriCounter2);
		}
		else if (BehaviorState == GenichiroState::MikiriCounter2)
		{
			GetBlackboardComponent(OwnerComp)->SetValueAsBool(TEXT("IdleOffGuard"), true);
			SetStateChange(OwnerComp, GenichiroState::Idle);
			return;
		}
		else if (BehaviorState == GenichiroState::Shock)
		{
			GetBlackboardComponent(OwnerComp)->SetValueAsFloat(TEXT("IdleWaitTime"), 1.f);
			SetStateChange(OwnerComp, GenichiroState::Idle);
			return;
		}
		else
		{
			SetStateChange(OwnerComp, GenichiroState::Idle);
			return;
		}
	}
}