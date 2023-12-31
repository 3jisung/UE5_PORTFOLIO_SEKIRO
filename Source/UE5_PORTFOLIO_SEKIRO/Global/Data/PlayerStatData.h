// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "UObject/NoExportTypes.h"
#include "PlayerStatData.generated.h"


USTRUCT(BlueprintType)
struct UE5_PORTFOLIO_SEKIRO_API FPlayerStatData : public FTableRowBase
{
	GENERATED_BODY()

	FPlayerStatData() {}
	~FPlayerStatData() {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float MaxHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float MaxPosture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float Power;
};
