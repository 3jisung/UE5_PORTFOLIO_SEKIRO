// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BtnEventWidget.h"
#include "SceneTransitionWidget.h"
#include "TitleWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE5_PORTFOLIO_SEKIRO_API UTitleWidget : public UBtnEventWidget
{
	GENERATED_BODY()
	
public:
	void NativeConstruct() override;

	void MenuEvent() override;

	UFUNCTION(BlueprintCallable)
	void SetSceneTransitionWidget(UUserWidget* UserWidget)
	{
		SceneTransitionWidget = Cast<USceneTransitionWidget>(UserWidget);
	}


protected:
	USceneTransitionWidget* SceneTransitionWidget = nullptr;
};
