// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GlobalUI/BtnEventWidget.h"
#include "Components/TextBlock.h"
#include "Global/GlobalEnums.h"
#include "GameFramework/PlayerController.h"
#include "BuddhaMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE5_PORTFOLIO_SEKIRO_API UBuddhaMenuWidget : public UBtnEventWidget
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

	bool SetHoveredBtn(int _Index) override;

	void MenuEvent() override;

	UFUNCTION(BlueprintCallable)
	void ExitWidget();

	UPROPERTY(Category = "Text", EditAnywhere, BlueprintReadWrite)
	UTextBlock* MapNameText = nullptr;

	UPROPERTY(Category = "Text", EditAnywhere, BlueprintReadWrite)
	UTextBlock* ExplainText = nullptr;

	TArray<FText> ExplainArray;


protected:
	class APlayerSekiro* Player = nullptr;
};
