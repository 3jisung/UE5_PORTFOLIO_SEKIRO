// Fill out your copyright notice in the Description page of Project Settings.


#include "BT_Walk_Genichiro.h"
#include "GameFramework/CharacterMovementComponent.h"


EBTNodeResult::Type UBT_Walk_Genichiro::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	GetGlobalCharacter(OwnerComp)->SetAniState(UBTTask_Genichiro::GetGenichiroState(OwnerComp));

	UCharacterMovementComponent* MoveCom = Cast<UCharacterMovementComponent>(GetGlobalCharacter(OwnerComp)->GetMovementComponent());

	if (nullptr != MoveCom)
	{
		MoveCom->MaxWalkSpeed = 100.0f;
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
	if (3.0f <= GetStateTime(OwnerComp))
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

		ThisPos.Z = 0.0f;
		TargetPos.Z = 0.0f;

		FVector TargetDir = TargetPos - ThisPos;
		TargetDir.Normalize();

		FVector OtherForward = GetGlobalCharacter(OwnerComp)->GetActorForwardVector();
		OtherForward.Normalize();

		// 벡터곱을 통해 타겟과의 각도 방향을 알 수 있다.(정확히는 벡터곱의 Z값)
		FVector Cross = FVector::CrossProduct(OtherForward, TargetDir);

		float Angle0 = TargetDir.Rotation().Yaw;
		float Angle1 = OtherForward.Rotation().Yaw;

		// 정면 벡터를 기준으로 타겟과 방향이 10.0f 이상 어긋날 경우 방향 조정
		if (FMath::Abs(Angle0 - Angle1) >= 10.0f)
		{
			FRotator Rot = FRotator::MakeFromEuler({ 0, 0, Cross.Z * 500.0f * DeltaSeconds });
			GetGlobalCharacter(OwnerComp)->AddActorWorldRotation(Rot);
		}
		// 그 외에는 방향값 그대로 유지
		else
		{
			FRotator Rot = TargetDir.Rotation();
			GetGlobalCharacter(OwnerComp)->SetActorRotation(Rot);
		}

		FVector MoveDir = TargetDir;
		GenichiroState BehaviorState = UBTTask_Genichiro::GetGenichiroState(OwnerComp);

		// 들어온 행동값에 따라 이동 방향 다르게 설정
		if (BehaviorState == GenichiroState::LeftWalk)
		{
			MoveDir = TargetDir.RotateAngleAxis(-90.0f, FVector::UpVector);
		}
		else if (BehaviorState == GenichiroState::RightWalk)
		{
			MoveDir = TargetDir.RotateAngleAxis(90.0f, FVector::UpVector);
		}
		else
		{
			SetStateChange(OwnerComp, GenichiroState::Idle);
			return;
		}

		GetGlobalCharacter(OwnerComp)->AddMovementInput(MoveDir, 1.0f);
	}

	return;
}