// Fill out your copyright notice in the Description page of Project Settings.


#include "BT_Attack_Genichiro.h"


EBTNodeResult::Type UBT_Attack_Genichiro::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	ResetStateTime(OwnerComp);

	GetGlobalCharacter(OwnerComp)->SetAniState(UBTTask_Genichiro::GetGenichiroState(OwnerComp));

	return EBTNodeResult::Type::InProgress;
}

void UBT_Attack_Genichiro::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	
	// 공격 시 방향 조정
	{
		UObject* TargetObject = GetBlackboardComponent(OwnerComp)->GetValueAsObject(TEXT("TargetActor"));
		AActor* TargetActor = Cast<AActor>(TargetObject);

		FVector ThisPos = GetGlobalCharacter(OwnerComp)->GetActorLocation();
		FVector TargetPos = TargetActor->GetActorLocation();

		ThisPos.Z = 0.0f;
		TargetPos.Z = 0.0f;

		FVector TargetDir = TargetPos - ThisPos;
		TargetDir.Normalize();

		FVector OtherForward = GetGlobalCharacter(OwnerComp)->GetActorForwardVector();
		OtherForward.Normalize();

		FVector Cross = FVector::CrossProduct(OtherForward, TargetDir);

		float Angle0 = TargetDir.Rotation().Yaw;
		float Angle1 = OtherForward.Rotation().Yaw;

		if (FMath::Abs(Angle0 - Angle1) >= 10.0f)
		{
			FRotator Rot = FRotator::MakeFromEuler({ 0, 0, Cross.Z * 500.0f * DeltaSeconds });
			GetGlobalCharacter(OwnerComp)->AddActorWorldRotation(Rot);
		}
		else
		{
			FRotator Rot = TargetDir.Rotation();
			GetGlobalCharacter(OwnerComp)->SetActorRotation(Rot);
		}
	}

	UAnimMontage* Montage = GetGlobalCharacter(OwnerComp)->GetAnimMontage(UBTTask_Genichiro::GetGenichiroState(OwnerComp));
	float ComboTime = 0.f;

	for (FAnimNotifyEvent NotifyEvent : Montage->Notifies)
	{
		if (NotifyEvent.GetNotifyEventName() == TEXT("AnimNotify_ComboTrigger"))
		{
			ComboTime = NotifyEvent.GetTriggerTime();
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
		float MontageLength = Montage->CalculateSequenceLength();

		if (MontageLength <= GetStateTime(OwnerComp))
		{
			ResetStateTime(OwnerComp);
			SetStateChange(OwnerComp, GenichiroState::Idle);
		}
	}
}