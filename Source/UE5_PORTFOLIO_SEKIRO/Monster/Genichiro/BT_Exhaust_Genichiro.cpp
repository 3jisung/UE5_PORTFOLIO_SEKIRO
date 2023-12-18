// Fill out your copyright notice in the Description page of Project Settings.


#include "BT_Exhaust_Genichiro.h"


EBTNodeResult::Type UBT_Exhaust_Genichiro::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	ResetStateTime(OwnerComp);

	GetGlobalCharacter(OwnerComp)->SetAniState(UBTTask_Genichiro::GetGenichiroState(OwnerComp));

	if (GetGenichiroState(OwnerComp) == GenichiroState::ExhaustStart
		|| GetGenichiroState(OwnerComp) == GenichiroState::ExhaustLoop)
	{
		Cast<AMonster>(GetGlobalCharacter(OwnerComp))->SetHitState(MonsterHitState::SUPERARMOR);
	}
	else if (GetGenichiroState(OwnerComp) == GenichiroState::Deathblow1
		|| GetGenichiroState(OwnerComp) == GenichiroState::Deathblow2
		|| GetGenichiroState(OwnerComp) == GenichiroState::Deathblow3)
	{
		Cast<AMonster>(GetGlobalCharacter(OwnerComp))->SetHitState(MonsterHitState::INVINCIBLE);
	}
	else
	{
		Cast<AMonster>(GetGlobalCharacter(OwnerComp))->SetHitState(MonsterHitState::OFFGUARD);
	}

	return EBTNodeResult::Type::InProgress;
}

void UBT_Exhaust_Genichiro::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	// 탈진 도중 인살 공격이 들어오면 상태 전환
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
		ABossGenichiro* Genichiro = Cast<ABossGenichiro>(GetGlobalCharacter(OwnerComp));
		GenichiroState BehaviorState = UBTTask_Genichiro::GetGenichiroState(OwnerComp);

		if (BehaviorState == GenichiroState::ExhaustStart)
		{
			SetStateChange(OwnerComp, GenichiroState::ExhaustLoop);
		}
		else if (BehaviorState == GenichiroState::Deathblow1)
		{
			SetStateChange(OwnerComp, GenichiroState::Deathblow2);
		}
		else if (BehaviorState == GenichiroState::Deathblow2)
		{
			Genichiro->SetPosture(Genichiro->GetMaxPosture());
			Genichiro->SetDeathblowCount(Genichiro->GetDeathblowCount() - 1);

			if (Genichiro->GetDeathblowCount() >= 1)
			{
				SetStateChange(OwnerComp, GenichiroState::Deathblow3);
			}
			else
			{
				SetStateChange(OwnerComp, GenichiroState::Death);
			}
		}
		else
		{
			// 인살 실패 시 몬스터는 체간 절반만큼 회복
			Genichiro->SetPosture(Genichiro->GetMaxPosture() * 0.5);
			SetStateChange(OwnerComp, GenichiroState::Idle);
			return;
		}
	}
}