// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "UObject/NoExportTypes.h"
#include "EffectData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct UE5_PORTFOLIO_SEKIRO_API FEffectData : public FTableRowBase
{
	GENERATED_BODY()

	FEffectData() {}
	~FEffectData() {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	UParticleSystem* Effect;
};
