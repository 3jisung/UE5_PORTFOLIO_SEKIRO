// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "UObject/NoExportTypes.h"
#include "WidgetClassData.generated.h"


USTRUCT(BlueprintType)
struct UE5_PORTFOLIO_SEKIRO_API FWidgetClassData : public FTableRowBase
{
	GENERATED_BODY()

	FWidgetClassData() {}
	~FWidgetClassData() {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	TMap<FName, TSubclassOf<UUserWidget>> GameWidgetClass;
};
