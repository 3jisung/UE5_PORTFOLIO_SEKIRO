// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BTTask_Genichiro.h"
#include "BT_Walk_Genichiro.generated.h"

/**
 * 
 */
UCLASS()
class UE5_PORTFOLIO_SEKIRO_API UBT_Walk_Genichiro : public UBTTask_Genichiro
{
	GENERATED_BODY()
	
protected:
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
	void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds);
};
