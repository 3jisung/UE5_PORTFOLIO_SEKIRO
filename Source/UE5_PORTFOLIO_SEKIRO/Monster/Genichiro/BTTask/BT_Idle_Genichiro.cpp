// Fill out your copyright notice in the Description page of Project Settings.


#include "BT_Idle_Genichiro.h"
#include "GameFramework/CharacterMovementComponent.h"


EBTNodeResult::Type UBT_Idle_Genichiro::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	ResetStateTime(OwnerComp);

	if (AnimChangeCheck(OwnerComp))
	{
		return EBTNodeResult::Type::Failed;
	}

	UObject* TargetObject = GetBlackboardComponent(OwnerComp)->GetValueAsObject(TEXT("TargetActor"));

	if (nullptr != TargetObject)
	{
		APlayerSekiro* TargetActor = Cast<APlayerSekiro>(TargetObject);
		TargetActor->TargetBoss = nullptr;
	}
	
	GetBlackboardComponent(OwnerComp)->SetValueAsObject(TEXT("TargetActor"), nullptr);

	UCharacterMovementComponent* MoveCom = Cast<UCharacterMovementComponent>(GetGlobalCharacter(OwnerComp)->GetMovementComponent());

	if (nullptr != MoveCom)
	{
		MoveCom->MaxWalkSpeed = 100.f;
	}

	// IdleOffGuard 값이 true일 시 offguard
	bool IdleOffGuard = GetBlackboardComponent(OwnerComp)->GetValueAsBool(TEXT("IdleOffGuard"));
	if (IdleOffGuard)
	{
		GetBlackboardComponent(OwnerComp)->SetValueAsBool(TEXT("IdleOffGuard"), false);
		Cast<AMonster>(GetGlobalCharacter(OwnerComp))->SetHitState(MonsterHitState::OFFGUARD);
	}
	else
	{
		Cast<AMonster>(GetGlobalCharacter(OwnerComp))->SetHitState(MonsterHitState::GUARD);
	}

	// IdleWaitTime 만큼 Idle 대기 시간 증가
	float IdleWaitTime = GetBlackboardComponent(OwnerComp)->GetValueAsFloat(TEXT("IdleWaitTime"));
	IdleTime = MinIdleTime + IdleWaitTime;
	GetBlackboardComponent(OwnerComp)->SetValueAsFloat(TEXT("IdleWaitTime"), 0.f);

	return EBTNodeResult::Type::InProgress;
}

void UBT_Idle_Genichiro::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (AnimChangeCheck(OwnerComp))
	{
		return;
	}

	AActor* ResultActor = GetTargetSearch(OwnerComp);

	if (nullptr != ResultActor)
	{
		GetBlackboardComponent(OwnerComp)->SetValueAsObject(TEXT("TargetActor"), ResultActor);

		// 방향 조정
		GetGlobalCharacter(OwnerComp)->AdjustAngle(DeltaSeconds, ResultActor->GetActorLocation(), 10.f);

		// 보스 몬스터가 플레이어를 타겟으로 지정했을 경우 플레이어의 TargetBoss 변수에 보스 객체 업데이트
		if (GetGlobalCharacter(OwnerComp)->ActorHasTag(TEXT("보스")))
		{
			APlayerSekiro* TargetActor = Cast<APlayerSekiro>(ResultActor);
			TargetActor->TargetBoss = Cast<AMonster>(GetGlobalCharacter(OwnerComp));
		}
	}
	else
	{
		return;
	}

	if (IdleTime <= GetStateTime(OwnerComp))
	{
		ResetStateTime(OwnerComp);

		// 버퍼에 등록된 상태(공격)값이 있을 경우 즉시 Run
		uint8 BufferedAttack = GetBlackboardComponent(OwnerComp)->GetValueAsEnum(TEXT("StateBuffer"));
		if (BufferedAttack != 0)
		{
			SetStateChange(OwnerComp, GenichiroState::ForwardRun);
			return;
		}

		// Walk : 견제 / Run : 대상에게 바로 접근
		int BehaviorValue = UGlobalFunctionLibrary::MainRandom.RandRange(0, 9);
		switch (BehaviorValue)
		{
		case 0:
			SetStateChange(OwnerComp, GenichiroState::LeftWalk);
			break;

		case 1:
			SetStateChange(OwnerComp, GenichiroState::RightWalk);
			break;

		default:
			SetStateChange(OwnerComp, GenichiroState::ForwardRun);
			break;
		}
	}
}