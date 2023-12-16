// Fill out your copyright notice in the Description page of Project Settings.


#include "BT_GetHit_Genichiro.h"


EBTNodeResult::Type UBT_GetHit_Genichiro::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	ResetStateTime(OwnerComp);

	GetGlobalCharacter(OwnerComp)->SetAniState(UBTTask_Genichiro::GetGenichiroState(OwnerComp));

	return EBTNodeResult::Type::InProgress;
}

void UBT_GetHit_Genichiro::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (IsDeathCheck(OwnerComp))
	{
		return;
	}

	// 방향, 위치 조정 추가
	{

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
		else
		{
			SetStateChange(OwnerComp, GenichiroState::Idle);
			return;
		}
	}
}