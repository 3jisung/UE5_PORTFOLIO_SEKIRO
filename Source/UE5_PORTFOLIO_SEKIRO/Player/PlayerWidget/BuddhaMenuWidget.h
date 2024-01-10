// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GlobalUI/FadeInOutWidget.h"
#include "Components/TextBlock.h"
#include "BuddhaMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE5_PORTFOLIO_SEKIRO_API UBuddhaMenuWidget : public UFadeInOutWidget
{
	GENERATED_BODY()
	

public:
	void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void SetMapName(FText _MapName)
	{
		if (MapNameText != nullptr)
		{
			MapNameText->SetText(_MapName);
		}
	}

	UPROPERTY(Category = "Text", EditAnywhere, BlueprintReadWrite)
	UTextBlock* MapNameText = nullptr;
};
