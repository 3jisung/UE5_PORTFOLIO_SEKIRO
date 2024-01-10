// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GlobalUI/FadeInOutWidget.h"
#include "BuddhaTriggerWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE5_PORTFOLIO_SEKIRO_API UBuddhaTriggerWidget : public UFadeInOutWidget
{
	GENERATED_BODY()
	

public:
	void NativeConstruct() override;

	void FadeIn() override
	{
		bFadeIn = true;
		bFadeOut = false;
	}

	void FadeOut() override
	{
		bFadeOut = true;
		bFadeIn = false;
	}
};