// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GlobalUI/BtnEventWidget.h"
#include "OKCancelWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE5_PORTFOLIO_SEKIRO_API UOKCancelWidget : public UBtnEventWidget
{
	GENERATED_BODY()
	

public:
	void NativeConstruct() override;
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void MenuEvent() override;

	UFUNCTION(BlueprintCallable)
	virtual void ExitWidget();

	void SetParentWidget(UFadeInOutWidget* _ParentWidget)
	{
		if (IsValid(_ParentWidget))
		{
			ParentWidget = _ParentWidget;
		}
	}

	virtual void SetNoticeText(FText _Text)
	{
		if (IsValid(NoticeText))
		{
			NoticeText->SetText(_Text);
		}
	}

	UPROPERTY(Category = "Notice", EditAnywhere, BlueprintReadWrite)
	UTextBlock* NoticeText = nullptr;

protected:
	UFadeInOutWidget* ParentWidget = nullptr;

	class APlayerSekiro* Player = nullptr;
};
