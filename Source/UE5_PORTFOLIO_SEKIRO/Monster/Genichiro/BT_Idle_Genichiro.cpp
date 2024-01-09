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

	Cast<AMonster>(GetGlobalCharacter(OwnerComp))->SetHitState(MonsterHitState::GUARD);

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

	if (0.3f <= GetStateTime(OwnerComp))
	{
		ResetStateTime(OwnerComp);

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