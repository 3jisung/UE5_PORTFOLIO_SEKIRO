// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GlobalUI/BtnEventWidget.h"
#include "BuddhaBossFightWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE5_PORTFOLIO_SEKIRO_API UBuddhaBossFightWidget : public UBtnEventWidget
{
	GENERATED_BODY()
	

public:
	void NativeConstruct() override;
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	bool SetHoveredBtn(int _Index) override;

	void MenuEvent() override;

	UFUNCTION(BlueprintCallable)
	void ExitWidget();

	void PopupWidgetReturn(int _PopupIndex) override;

	void SetParentWidget(UFadeInOutWidget* _ParentWidget)
	{
		if (IsValid(_ParentWidget))
		{
			ParentWidget = _ParentWidget;
		}
	}

	UFadeInOutWidget* GetParentWidget()
	{
		return ParentWidget;
	}


	UPROPERTY(Category = "Boss", EditAnywhere, BlueprintReadWrite)
	UTextBlock* BossNameText = nullptr;

	UPROPERTY(Category = "Boss", EditAnywhere, BlueprintReadWrite)
	UTextBlock* MapNameText = nullptr;

	UPROPERTY(Category = "Boss", EditAnywhere, BlueprintReadWrite)
	UImage* BossThumbnailImage = nullptr;

	TArray<FText> BossNameArray;
	TArray<FText> MapNameArray;
	TArray<UTexture2D*> BossThumbnailArray;

	TArray<FName> BossLevelArray;


protected:
	class APlayerSekiro* Player = nullptr;

	UFadeInOutWidget* ParentWidget = nullptr;
};
