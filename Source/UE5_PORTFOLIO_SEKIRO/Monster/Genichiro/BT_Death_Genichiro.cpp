// Fill out your copyright notice in the Description page of Project Settings.


#include "BT_Death_Genichiro.h"


EBTNodeResult::Type UBT_Death_Genichiro::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	ResetStateTime(OwnerComp);

	Cast<AMonster>(GetGlobalCharacter(OwnerComp))->SetHitState(MonsterHitState::INVINCIBLE);

	return EBTNodeResult::Type::InProgress;
}

void UBT_Death_Genichiro::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
}