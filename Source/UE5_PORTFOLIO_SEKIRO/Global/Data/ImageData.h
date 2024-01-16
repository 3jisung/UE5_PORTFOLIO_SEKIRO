// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "UObject/NoExportTypes.h"
#include "Engine/Texture2D.h"
#include "ImageData.generated.h"


USTRUCT(BlueprintType)
struct UE5_PORTFOLIO_SEKIRO_API FImageData : public FTableRowBase
{
	GENERATED_BODY()

	FImageData() {}
	~FImageData() {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	TArray<UTexture2D*> GameImage;
};
