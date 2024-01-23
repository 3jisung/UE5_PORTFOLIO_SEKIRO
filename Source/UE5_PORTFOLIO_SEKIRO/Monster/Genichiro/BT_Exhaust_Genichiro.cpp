// Fill out your copyright notice in the Description page of Project Settings.


#include "BT_Exhaust_Genichiro.h"
#include "Kismet/GameplayStatics.h"
#include "GameMode/GlobalGameMode.h"


EBTNodeResult::Type UBT_Exhaust_Genichiro::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	ResetStateTime(OwnerComp);

	if (AnimChangeCheck(OwnerComp))
	{
		return EBTNodeResult::Type::Failed;
	}

	Cast<AMonster>(GetGlobalCharacter(OwnerComp))->SetHitState(MonsterHitState::INVINCIBLE);

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

	ABossGenichiro* Genichiro = Cast<ABossGenichiro>(GetGlobalCharacter(OwnerComp));
	GenichiroState BehaviorState = UBTTask_Genichiro::GetGenichiroState(OwnerComp);

	if (nullptr == TargetActor)
	{
		SetStateChange(OwnerComp, GenichiroState::Idle);
		return;
	}

	// 타겟이 나를 바라보고 있는 경우에만 인살 구슬 표시
	if (BehaviorState == GenichiroState::ExhaustStart || BehaviorState == GenichiroState::ExhaustLoop)
	{
		EObjectTypeQuery ObjectType = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel2);
		TArray<AActor*> HitActor = Cast<AGlobalCharacter>(TargetActor)->TraceObjects(ObjectType, TargetActor->GetActorForwardVector(), 45.f, 50.f, 70.f);

		if (HitActor.Num() == 0)
		{
			Genichiro->DeathblowIconOnOff(false);
		}

		for (size_t i = 0; i < HitActor.Num(); i++)
		{
			if (HitActor[i] == Cast<AActor>(Genichiro))
			{
				Genichiro->DeathblowIconOnOff(true);
				break;
			}

			if (i == HitActor.Num() - 1)
			{
				Genichiro->DeathblowIconOnOff(false);
			}
		}
	}
	else
	{
		Genichiro->DeathblowIconOnOff(false);
	}

	UAnimMontage* Montage = GetGlobalCharacter(OwnerComp)->GetAnimMontage(UBTTask_Genichiro::GetGenichiroState(OwnerComp));
	float Time = Montage->CalculateSequenceLength() * (1 / Montage->RateScale);
	if (Time <= GetStateTime(OwnerComp))
	{
		if (BehaviorState == GenichiroState::ExhaustStart)
		{
			SetStateChange(OwnerComp, GenichiroState::ExhaustLoop);
		}
		else if (BehaviorState == GenichiroState::ExhaustLoop)
		{
			// 인살 실패 시 몬스터는 체간 절반만큼 회복
			Genichiro->SetPosture(Genichiro->GetMaxPosture() * 0.5f);
			SetStateChange(OwnerComp, GenichiroState::ExhaustEnd);
		}
		else if (BehaviorState == GenichiroState::Deathblow1)
		{
			Genichiro->SetHP(0.f);
			Genichiro->SetDeathblowCount(Genichiro->GetDeathblowCount() - 1);

			SetStateChange(OwnerComp, GenichiroState::Deathblow2);
		}
		else if (BehaviorState == GenichiroState::Deathblow2)
		{
			if (Genichiro->GetDeathblowCount() >= 1)
			{
				SetStateChange(OwnerComp, GenichiroState::Deathblow3);
			}
			else
			{	
				SetStateChange(OwnerComp, GenichiroState::Death);
			}
		}
		else if (BehaviorState == GenichiroState::Deathblow3)
		{
			Genichiro->SetHP(Genichiro->GetMaxHP());
			Genichiro->SetPosture(Genichiro->GetMaxPosture());

			SetStateChange(OwnerComp, GenichiroState::Phase2Intro1);
		}
		else if (BehaviorState == GenichiroState::Phase2Intro1)
		{
			SetStateChange(OwnerComp, GenichiroState::Phase2Intro2);
		}
		else if (BehaviorState == GenichiroState::Phase2Intro2)
		{
			// 페이즈2 시작하자마자 플레이어 탐색 후 뇌격 실행
			GetBlackboardComponent(OwnerComp)->SetValueAsEnum(TEXT("StateBuffer"), static_cast<uint8>(GenichiroState::ElectricSlash1));
			SetStateChange(OwnerComp, GenichiroState::ForwardRun);
		}
		else
		{
			Genichiro->DeathblowIconOnOff(false);
			SetStateChange(OwnerComp, GenichiroState::Idle);
			return;
		}
	}
}