// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BTTask_Genichiro.h"
#include "BT_Idle_Genichiro.generated.h"

/**
 * 
 */
UCLASS()
class UE5_PORTFOLIO_SEKIRO_API UBT_Idle_Genichiro : public UBTTask_Genichiro
{
	GENERATED_BODY()
	
public:
	bool InitIdleFunction()
	{
		FTimerHandle myTimerHandle;
		float delayTime = 1.f;
		GetWorld()->GetTimerManager().SetTimer(myTimerHandle, FTimerDelegate::CreateLambda([&]()
			{
				bInitIdle = false;

				GetWorld()->GetTimerManager().ClearTimer(myTimerHandle);
			}), delayTime, false);

		return true;
	}


protected:
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
	void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds);


private:
	bool bInitIdle = true;
};
