// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include <BehaviorTree/BlackboardComponent.h>
#include "../AICon.h"
#include "../../Global/GlobalEnums.h"
#include "../../Global/GlobalCharacter.h"
#include "../../Global/GlobalFunctionLibrary.h"
#include "BossGenichiro.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "BTTask_Genichiro.generated.h"

/**
 * 
 */
UCLASS()
class UE5_PORTFOLIO_SEKIRO_API UBTTask_Genichiro : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_Genichiro();
	void OnGameplayTaskActivated(class UGameplayTask&) override {}
	void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

	class AGlobalCharacter* GetGlobalCharacter(UBehaviorTreeComponent& OwnerComp);
	class UBlackboardComponent* GetBlackboardComponent(UBehaviorTreeComponent& OwnerComp);

	float GetStateTime(UBehaviorTreeComponent& OwnerComp);
	void ResetStateTime(UBehaviorTreeComponent& OwnerComp);

	GenichiroState GetGenichiroState(UBehaviorTreeComponent& OwnerComp);

	template<typename EnumType>
	void SetStateChange(UBehaviorTreeComponent& OwnerComp, EnumType _State)
	{
		SetStateChange(OwnerComp, static_cast<uint8>(_State));
	}

	void SetStateChange(UBehaviorTreeComponent& OwnerComp, uint8 _State);

	class AActor* GetTargetSearch(UBehaviorTreeComponent& OwnerComp);

	bool IsDeathCheck(UBehaviorTreeComponent& OwnerComp);

	UNavigationPath* PathFindNavPath(UBehaviorTreeComponent& OwnerComp, AActor* _Actor);
	UNavigationPath* PathFindNavPath(UBehaviorTreeComponent& OwnerComp, FVector _Pos);
};
