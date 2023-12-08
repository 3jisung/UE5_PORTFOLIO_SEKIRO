// Fill out your copyright notice in the Description page of Project Settings.


#include "BT_Idle_Genichiro.h"


EBTNodeResult::Type UBT_Idle_Genichiro::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	GetGlobalCharacter(OwnerComp)->SetAniState(GenichiroState::Idle);

	return EBTNodeResult::Type::InProgress;
}

void UBT_Idle_Genichiro::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (1.0f <= GetStateTime(OwnerComp))
	{
		//int Value = UGlobalFunctionLibrary::MainRandom.RandRange(0, 1);
	}

	AActor* ResultActor = GetTargetSearch(OwnerComp);

	if (nullptr != ResultActor)
	{
		GetBlackboardComponent(OwnerComp)->SetValueAsObject(TEXT("TargetActor"), ResultActor);
		//UE_LOG(LogTemp, Error, TEXT("Targeting"));
		SetStateChange(OwnerComp, GenichiroState::BasicAttack1);
		return;
	}

	return;
}