// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
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
	virtual void FadeOut(bool Destruct = false)
	{
		CanvasOpacity = 1.f;
		bFadeOut = true;
		bFadeIn = false;

		if (Destruct)
		{
			bDestructWidget = Destruct;
		}
	}

	UFUNCTION(BlueprintCallable)
	float GetCanvasOpacity()
	{
		return CanvasOpacity;
	}

	UFUNCTION(BlueprintCallable)
	void SetCanvasRenderOpacity(float _Opacity)
	{
		if (Canvas)
		{
			CanvasOpacity = _Opacity;
			Canvas->SetRenderOpacity(CanvasOpacity);
		}
	}

	virtual void PopupWidgetReturn(int _PopupIndex)
	{
		FadeOut(true);
	}

	UPROPERTY(Category = "Canvas", EditAnywhere, BlueprintReadWrite)
	UCanvasPanel* Canvas = nullptr;


protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowProtectedAccess = "true"))
	float CanvasOpacity = 0.f;

	UPROPERTY(BlueprintReadWrite, meta = (AllowProtectedAccess = "true"))
	bool bFadeIn = false;

	UPROPERTY(BlueprintReadWrite, meta = (AllowProtectedAccess = "true"))
	bool bFadeOut = false;

	float SumTime = 0.f;

	float FadeInDeltaTime = 0.f;
	float FadeOutDeltaTime = 0.f;
	float FadeInDeltaOpacity = 0.f;
	float FadeOutDeltaOpacity = 0.f;
	float TimerDelayTime = 0.f;

	bool bDestructWidget = false;

	APlayerController* PlayerController = nullptr;
};
