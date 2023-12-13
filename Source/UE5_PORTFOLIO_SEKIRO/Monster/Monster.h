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

	int GetDeathblowCount()
	{
		return DeathblowCount;
	}

	void SetDeathblowCount(int _DeathblowCount)
	{
		DeathblowCount = _DeathblowCount;
	}

	void LockOnIconOnOff(bool bLockOn);

	UFUNCTION(BlueprintCallable)
	virtual void AttackMove() { UE_LOG(LogTemp, Error, TEXT("Monster AttackMove")); }

	UFUNCTION(BlueprintCallable)
	virtual void MontageEnd() { UE_LOG(LogTemp, Error, TEXT("Monster MontageEnd")); }

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

	FTimerHandle AttackMoveTimerHandle;
};
