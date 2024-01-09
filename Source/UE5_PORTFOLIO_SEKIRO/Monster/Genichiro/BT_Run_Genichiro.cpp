// Fill out your copyright notice in the Description page of Project Settings.


#include "BT_Run_Genichiro.h"
#include "GameFramework/CharacterMovementComponent.h"


EBTNodeResult::Type UBT_Run_Genichiro::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
		MoveCom->MaxWalkSpeed = 800.f;
	}

	UObject* TargetObject = GetBlackboardComponent(OwnerComp)->GetValueAsObject(TEXT("TargetActor"));
	AActor* TargetActor = Cast<AActor>(TargetObject);
	FVector TargetPos = TargetActor->GetActorLocation();

	UNavigationPath* PathPoint = PathFindNavPath(OwnerComp, TargetPos);
	GetBlackboardComponent(OwnerComp)->SetValueAsObject(TEXT("NavPath"), PathPoint);
	GetBlackboardComponent(OwnerComp)->SetValueAsInt(TEXT("PathPointIndex"), 1);
	GetBlackboardComponent(OwnerComp)->SetValueAsVector(TEXT("LastTargetPos"), TargetPos);

	return EBTNodeResult::Type::InProgress;
}

void UBT_Run_Genichiro::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (AnimChangeCheck(OwnerComp))
	{
		return;
	}

	UObject* TargetObject = GetBlackboardComponent(OwnerComp)->GetValueAsObject(TEXT("TargetActor"));
	AGlobalCharacter* TargetActor = Cast<AGlobalCharacter>(TargetObject);

	if (nullptr == TargetActor || TargetActor->GetHP() <= 0.f)
	{
		SetStateChange(OwnerComp, GenichiroState::Idle);
		return;
	}

	FVector ThisPos = GetGlobalCharacter(OwnerComp)->GetActorLocation();
	FVector PointPos = TargetActor->GetActorLocation();			// 타겟까지 가는 길의 PathPoints[index] 가 들어갈 값
	FVector TargetPos = TargetActor->GetActorLocation();		// 타겟의 현재 위치
	FVector CheckLastPos = GetBlackboardComponent(OwnerComp)->GetValueAsVector(TEXT("LastTargetPos"));

	if (TargetPos != CheckLastPos)
	{
		UNavigationPath* NewPath = PathFindNavPath(OwnerComp, TargetPos);
		GetBlackboardComponent(OwnerComp)->SetValueAsObject(TEXT("NavPath"), NewPath);
		GetBlackboardComponent(OwnerComp)->SetValueAsInt(TEXT("PathPointIndex"), 1);
	}

	UObject* NavObject = GetBlackboardComponent(OwnerComp)->GetValueAsObject(TEXT("NavPath"));
	UNavigationPath* NavPath = Cast<UNavigationPath>(NavObject);
	int PathPointIndex = GetBlackboardComponent(OwnerComp)->GetValueAsInt(TEXT("PathPointIndex"));

	if (nullptr == NavPath)
	{
		SetStateChange(OwnerComp, GenichiroState::Idle);
		return;
	}

	if (nullptr != NavPath && true == NavPath->PathPoints.IsEmpty())
	{
		SetStateChange(OwnerComp, GenichiroState::Idle);
		return;
	}

	if (nullptr != NavPath)
	{
		PointPos = NavPath->PathPoints[PathPointIndex];
	}

	// 방향 조정
	GetGlobalCharacter(OwnerComp)->AdjustAngle(DeltaSeconds, PointPos, 10.f);

	ThisPos.Z = 0.f;
	PointPos.Z = 0.f;
	TargetPos.Z = 0.f;

	FVector PointDir = PointPos - ThisPos;
	PointDir.Normalize();

	// 이동
	GetGlobalCharacter(OwnerComp)->AddMovementInput(PointDir);

	{
		FVector Dir = PointPos - ThisPos;

		// PathPoint[index]까지 접근 완료
		if (50.f >= Dir.Size())
		{
			if (NavPath->PathPoints.Num() - 1 > PathPointIndex)
			{
				GetBlackboardComponent(OwnerComp)->SetValueAsInt(TEXT("PathPointIndex"), ++PathPointIndex);
			}
		}
	}

	float SearchRange = GetBlackboardComponent(OwnerComp)->GetValueAsFloat(TEXT("SearchRange"));
	float AttackRange = GetBlackboardComponent(OwnerComp)->GetValueAsFloat(TEXT("AttackRange"));

	FVector LastDir = TargetPos - ThisPos;

	// 타겟이 탐색 범위보다 멀어질 경우 Idle로 전환
	if (SearchRange < LastDir.Size())
	{
		SetStateChange(OwnerComp, GenichiroState::Idle);
		return;
	}

	// 공격 범위까지 근접
	if (AttackRange >= LastDir.Size())
	{
		int BehaviorValue = UGlobalFunctionLibrary::MainRandom.RandRange(0, 4);
		switch (BehaviorValue)
		{
		case 0:
			SetStateChange(OwnerComp, GenichiroState::BasicAttack1);
			break;

		case 1:
			SetStateChange(OwnerComp, GenichiroState::StabAttack);
			break;

		case 2:
			SetStateChange(OwnerComp, GenichiroState::TakeDownAttack);
			break;

		case 3:
			SetStateChange(OwnerComp, GenichiroState::BottomAttack);
			break;

		case 4:
		{
			ABossGenichiro* Genichiro = Cast<ABossGenichiro>(GetGlobalCharacter(OwnerComp));

			if (Genichiro->GetDeathblowCount() >= 2)
			{
				SetStateChange(OwnerComp, GenichiroState::BasicAttack1);
			}
			else
			{
				SetStateChange(OwnerComp, GenichiroState::ElectricSlash1);
			}

			break;
		}
		default:
			SetStateChange(OwnerComp, GenichiroState::Idle);
			break;
		}

		return;
	}
}