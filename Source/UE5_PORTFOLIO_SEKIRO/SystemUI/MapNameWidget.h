// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../SystemUI/FadeInOutWidget.h"
#include "Components/TextBlock.h"
#include "MapNameWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE5_PORTFOLIO_SEKIRO_API UMapNameWidget : public UFadeInOutWidget
{
	GENERATED_BODY()
	

public:
	void NativeConstruct() override;
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void PrintMapName(FText _MapName)
	{
		MapNameText->SetText(_MapName);

		FadeIn();
	}

	UPROPERTY(Category = "Text", EditAnywhere, BlueprintReadWrite)
	UTextBlock* MapNameText = nullptr;
};
