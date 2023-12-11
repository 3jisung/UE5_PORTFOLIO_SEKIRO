// Fill out your copyright notice in the Description page of Project Settings.


#include "BT_Run_Genichiro.h"
#include "GameFramework/CharacterMovementComponent.h"


EBTNodeResult::Type UBT_Run_Genichiro::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	GetGlobalCharacter(OwnerComp)->SetAniState(UBTTask_Genichiro::GetGenichiroState(OwnerComp));

	UCharacterMovementComponent* MoveCom = Cast<UCharacterMovementComponent>(GetGlobalCharacter(OwnerComp)->GetMovementComponent());

	if (nullptr != MoveCom)
	{
		MoveCom->MaxWalkSpeed = 500.0f;
	}

	return EBTNodeResult::Type::InProgress;
}

void UBT_Run_Genichiro::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	return;
}