// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "UObject/NoExportTypes.h"
#include "../GlobalEnums.h"
#include "PlayerAnimData.generated.h"


USTRUCT(BlueprintType)
struct UE5_PORTFOLIO_SEKIRO_API FPlayerAnimData : public FTableRowBase
{
	GENERATED_BODY()

	FPlayerAnimData() {}
	~FPlayerAnimData() {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	TMap<SekiroState, class UAnimMontage*> Animations;
};
