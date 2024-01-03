// Fill out your copyright notice in the Description page of Project Settings.


#include "BT_Attack_Genichiro.h"


EBTNodeResult::Type UBT_Attack_Genichiro::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	ResetStateTime(OwnerComp);

	if (AnimChangeCheck(OwnerComp))
	{
		return EBTNodeResult::Type::Failed;
	}

	if (GetGenichiroState(OwnerComp) == GenichiroState::BasicAttack1 || GetGenichiroState(OwnerComp) == GenichiroState::BasicAttack2
		|| GetGenichiroState(OwnerComp) == GenichiroState::BasicAttack3)
	{
		Cast<AMonster>(GetGlobalCharacter(OwnerComp))->SetHitState(MonsterHitState::OFFGUARD);
	}
	else
	{
		Cast<AMonster>(GetGlobalCharacter(OwnerComp))->SetHitState(MonsterHitState::SUPERARMOR);
	}

	return EBTNodeResult::Type::InProgress;
}

void UBT_Attack_Genichiro::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	
	if (AnimChangeCheck(OwnerComp))
	{
		return;
	}
	
	// 공격 시 방향 조정
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

	UAnimMontage* Montage = GetGlobalCharacter(OwnerComp)->GetAnimMontage(UBTTask_Genichiro::GetGenichiroState(OwnerComp));
	float ComboTime = 0.f;

	for (FAnimNotifyEvent NotifyEvent : Montage->Notifies)
	{
		if (NotifyEvent.GetNotifyEventName() == TEXT("AnimNotify_ComboTrigger"))
		{
			ComboTime = NotifyEvent.GetTriggerTime() * (1 / Montage->RateScale);
			break;
		}
	}

	// 콤보 노티파이에 트리거 시 다음 공격으로 전환
	GenichiroState BehaviorState = UBTTask_Genichiro::GetGenichiroState(OwnerComp);

	if (ComboTime != 0.f)
	{
		if (ComboTime <= GetStateTime(OwnerComp))
		{
			ResetStateTime(OwnerComp);

			if (BehaviorState == GenichiroState::BasicAttack1)
			{
				SetStateChange(OwnerComp, GenichiroState::BasicAttack2);
			}
			else if (BehaviorState == GenichiroState::BasicAttack2)
			{
				int BehaviorValue = UGlobalFunctionLibrary::MainRandom.RandRange(0, 2);
				switch (BehaviorValue)
				{
				case 0:
					SetStateChange(OwnerComp, GenichiroState::BasicAttack3);
					break;

				case 1:
					SetStateChange(OwnerComp, GenichiroState::StabAttack);
					break;

				case 2:
					SetStateChange(OwnerComp, GenichiroState::BottomAttack);
					break;

				default:
					SetStateChange(OwnerComp, GenichiroState::Idle);
					break;
				}
			}
			else if (BehaviorState == GenichiroState::TakeDownAttack)
			{
				int BehaviorValue = UGlobalFunctionLibrary::MainRandom.RandRange(0, 2);
				switch (BehaviorValue)
				{
				case 0:
					SetStateChange(OwnerComp, GenichiroState::StabAttack);
					break;

				case 1:
					SetStateChange(OwnerComp, GenichiroState::BottomAttack);
					break;

				default:
					SetStateChange(OwnerComp, GenichiroState::Idle);
					break;
				}
			}
			else if (BehaviorState == GenichiroState::ElectricSlash1)
			{
				SetStateChange(OwnerComp, GenichiroState::ElectricSlash2);
			}
			else
			{
				SetStateChange(OwnerComp, GenichiroState::Idle);
				return;
			}
		}
	}
	// 단타형 공격 or 마지막 콤보 공격
	else
	{
		float MontageLength = Montage->CalculateSequenceLength() * (1 / Montage->RateScale);

		if (MontageLength <= GetStateTime(OwnerComp))
		{
			int BehaviorValue = UGlobalFunctionLibrary::MainRandom.RandRange(0, 5);
			switch (BehaviorValue)
			{
			case 0:
				SetStateChange(OwnerComp, GenichiroState::Idle);
				break;

			default:
				SetStateChange(OwnerComp, GenichiroState::ForwardRun);
				break;
			}
		}
	}
}