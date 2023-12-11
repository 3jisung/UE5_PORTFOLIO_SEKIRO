// Fill out your copyright notice in the Description page of Project Settings.


#include "BT_Idle_Genichiro.h"
#include "GameFramework/CharacterMovementComponent.h"


EBTNodeResult::Type UBT_Idle_Genichiro::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	GetGlobalCharacter(OwnerComp)->SetAniState(GenichiroState::Idle);

	GetBlackboardComponent(OwnerComp)->SetValueAsObject(TEXT("TargetActor"), nullptr);

	UCharacterMovementComponent* MoveCom = Cast<UCharacterMovementComponent>(GetGlobalCharacter(OwnerComp)->GetMovementComponent());

	if (nullptr != MoveCom)
	{
		MoveCom->MaxWalkSpeed = 100.0f;
	}

	return EBTNodeResult::Type::InProgress;
}

void UBT_Idle_Genichiro::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (2.0f <= GetStateTime(OwnerComp))
	{
		ResetStateTime(OwnerComp);

		AActor* ResultActor = GetTargetSearch(OwnerComp);

		if (nullptr != ResultActor)
		{
			GetBlackboardComponent(OwnerComp)->SetValueAsObject(TEXT("TargetActor"), ResultActor);

			int BehaviorValue = UGlobalFunctionLibrary::MainRandom.RandRange(0, 2);
			switch (BehaviorValue)
			{
			case 0:
				SetStateChange(OwnerComp, GenichiroState::LeftWalk);
				break;
				
			case 1:
				SetStateChange(OwnerComp, GenichiroState::RightWalk);
				break;

			case 2:
				SetStateChange(OwnerComp, GenichiroState::ForwardRun);
				break;

			default:
				SetStateChange(OwnerComp, GenichiroState::Idle);
				break;
			}
		}

		else
		{
			return;
		}
	}
}