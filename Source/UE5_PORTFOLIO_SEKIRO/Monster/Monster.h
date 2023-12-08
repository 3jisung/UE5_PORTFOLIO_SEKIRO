// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Global/GlobalCharacter.h"
#include "../Global/Data/MonsterData.h"
#include "Monster.generated.h"

/**
 * 
 */
UCLASS()
class UE5_PORTFOLIO_SEKIRO_API AMonster : public AGlobalCharacter
{
	GENERATED_BODY()
	

public:
	AMonster();

	FORCEINLINE class UBehaviorTree* GetBehaviorTree()
	{
		return BehaviorTree;
	}

	class UBlackboardComponent* GetBlackboardComponent();

	void LockOnIconOnOff(bool bLockOn);

	const struct FMonsterData* MonsterData;
	

protected:
	void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	class UBehaviorTree* BehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	class UBlackboardComponent* BlackboardComponent;

	UPROPERTY(Category = "Monster", EditAnywhere, BlueprintReadWrite)
	int DeathblowCount;

	UPROPERTY(Category = "Components", EditAnywhere, BlueprintReadWrite)
	class UWidgetComponent* WidgetComponent;

	TSubclassOf<UUserWidget> LockOnIconWidgetClass;
};
