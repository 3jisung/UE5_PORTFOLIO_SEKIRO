// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FadeInOutWidget.h"
#include "SceneTransitionWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE5_PORTFOLIO_SEKIRO_API USceneTransitionWidget : public UFadeInOutWidget
{
	GENERATED_BODY()
	

public:
	void NativeConstruct() override;
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void FadeIn() override
	{
		bFadeIn = true;
		bFadeOut = false;
		CanvasOpacity = 1.f;
	}

	void FadeOut(bool Destruct = false) override
	{
		bFadeOut = true;
		bFadeIn = false;
		CanvasOpacity = 0.f;
	}
};
