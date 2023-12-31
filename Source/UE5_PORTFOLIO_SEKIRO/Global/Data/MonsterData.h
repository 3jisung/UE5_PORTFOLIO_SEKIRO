// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "UObject/NoExportTypes.h"
#include "../GlobalEnums.h"
#include "MonsterData.generated.h"


USTRUCT(BlueprintType)
struct UE5_PORTFOLIO_SEKIRO_API FMonsterData : public FTableRowBase
{
	GENERATED_BODY()

	FMonsterData() {}
	~FMonsterData() {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float MaxHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float MaxPosture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float Power;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int MaxDeathblowCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	class UBehaviorTree* AI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	TMap<GenichiroState, class UAnimMontage*> Animations;
};