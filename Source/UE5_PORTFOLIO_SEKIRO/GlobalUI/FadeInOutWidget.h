// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "FadeInOutWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE5_PORTFOLIO_SEKIRO_API UFadeInOutWidget : public UUserWidget
{
	GENERATED_BODY()
	

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintCallable)
	virtual void FadeIn()
	{
		CanvasOpacity = 0.f;
		bFadeIn = true;
		bFadeOut = false;
	}

	UFUNCTION(BlueprintCallable)
	virtual void FadeOut()
	{
		CanvasOpacity = 1.f;
		bFadeOut = true;
		bFadeIn = false;
	}

	UFUNCTION(BlueprintCallable)
	float GetCanvasOpacity()
	{
		return CanvasOpacity;
	}

	UPROPERTY(Category = "Canvas", EditAnywhere, BlueprintReadWrite)
	UCanvasPanel* Canvas = nullptr;


protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float CanvasOpacity = 0.f;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bFadeIn = false;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bFadeOut = false;

	float SumTime = 0.f;

	float FadeInDeltaTime = 0.f;
	float FadeOutDeltaTime = 0.f;
	float FadeInDeltaOpacity = 0.f;
	float FadeOutDeltaOpacity = 0.f;
	float TimerDelayTime = 0.f;

	bool bDestructWidget = false;
};
