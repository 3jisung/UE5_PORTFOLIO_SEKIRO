// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "UObject/NoExportTypes.h"
#include "SoundData.generated.h"


USTRUCT(BlueprintType)
struct UE5_PORTFOLIO_SEKIRO_API FSoundData : public FTableRowBase
{
	GENERATED_BODY()

	FSoundData() {}
	~FSoundData() {}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	TMap<FName, class USoundBase*> GameSound;
};
