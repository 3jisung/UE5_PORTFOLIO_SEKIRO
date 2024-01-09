// Fill out your copyright notice in the Description page of Project Settings.


#include "BT_Walk_Genichiro.h"
#include "GameFramework/CharacterMovementComponent.h"


EBTNodeResult::Type UBT_Walk_Genichiro::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	ResetStateTime(OwnerComp);

	if (AnimChangeCheck(OwnerComp))
	{
		return EBTNodeResult::Type::Failed;
	}

	Cast<AMonster>(GetGlobalCharacter(OwnerComp))->SetHitState(MonsterHitState::GUARD);

	UCharacterMovementComponent* MoveCom = Cast<UCharacterMovementComponent>(GetGlobalCharacter(OwnerComp)->GetMovementComponent());

	if (nullptr != MoveCom)
	{
		MoveCom->MaxWalkSpeed = 100.f;
	}

	UObject* TargetObject = GetBlackboardComponent(OwnerComp)->GetValueAsObject(TEXT("TargetActor"));
	AActor* TargetActor = Cast<AActor>(TargetObject);
	FVector TargetPos = TargetActor->GetActorLocation();
	GetBlackboardComponent(OwnerComp)->SetValueAsVector(TEXT("LastTargetPos"), TargetPos);

	return EBTNodeResult::Type::InProgress;
}

void UBT_Walk_Genichiro::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (AnimChangeCheck(OwnerComp))
	{
		return;
	}

	UObject* TargetObject = GetBlackboardComponent(OwnerComp)->GetValueAsObject(TEXT("TargetActor"));
	AActor* TargetActor = Cast<AActor>(TargetObject);

	FVector ThisPos = GetGlobalCharacter(OwnerComp)->GetActorLocation();
	FVector TargetPos = TargetActor->GetActorLocation();

	if (nullptr == TargetActor)
	{
		SetStateChange(OwnerComp, GenichiroState::Idle);
		return;
	}

	// 3초간 Walk 후 상태 전환 
	if (3.f <= GetStateTime(OwnerComp))
	{
		ResetStateTime(OwnerComp);

		float AttackRange = GetBlackboardComponent(OwnerComp)->GetValueAsFloat(TEXT("AttackRange"));
		FVector LastDir = TargetPos - ThisPos;

		// 공격 범위까지 근접
		if (AttackRange >= LastDir.Size())
		{
			SetStateChange(OwnerComp, GenichiroState::BasicAttack1);
			return;
		}
		// 거리가 멀 경우 Run 상태로 전환하여 거리 좁히기
		else
		{
			SetStateChange(OwnerComp, GenichiroState::ForwardRun);
			return;
		}
	}
	else
	{
		FVector CheckLastTargetPos = GetBlackboardComponent(OwnerComp)->GetValueAsVector(TEXT("LastTargetPos"));

		if (TargetPos != CheckLastTargetPos)
		{
			// 타겟이 움직였을 경우에만 새로 갱신
			GetBlackboardComponent(OwnerComp)->SetValueAsVector(TEXT("LastTargetPos"), TargetPos);
		}

		GetGlobalCharacter(OwnerComp)->AdjustAngle(DeltaSeconds, TargetPos, 10.f);

		ThisPos.Z = 0.f;
		TargetPos.Z = 0.f;

		FVector MoveDir = TargetPos - ThisPos;
		MoveDir.Normalize();

		GenichiroState BehaviorState = UBTTask_Genichiro::GetGenichiroState(OwnerComp);

		// 들어온 행동값에 따라 이동 방향 다르게 설정
		if (BehaviorState == GenichiroState::LeftWalk)
		{
			MoveDir = MoveDir.RotateAngleAxis(-90.f, FVector::UpVector);
		}
		else if (BehaviorState == GenichiroState::RightWalk)
		{
			MoveDir = MoveDir.RotateAngleAxis(90.f, FVector::UpVector);
		}
		else
		{
			SetStateChange(OwnerComp, GenichiroState::Idle);
			return;
		}

		GetGlobalCharacter(OwnerComp)->AddMovementInput(MoveDir, 1.f);
	}

	return;
}