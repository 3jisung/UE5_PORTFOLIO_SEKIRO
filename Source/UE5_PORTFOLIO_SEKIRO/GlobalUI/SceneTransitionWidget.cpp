// Fill out your copyright notice in the Description page of Project Settings.


#include "SceneTransitionWidget.h"


void USceneTransitionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	FadeInDeltaTime = 0.01f;
	FadeOutDeltaTime = 0.01f;
	FadeInDeltaOpacity = 0.02f;
	FadeOutDeltaOpacity = 0.04f;
}

void USceneTransitionWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	// FadeIn
	if (bFadeIn)
	{
		SumTime += InDeltaTime;

		if (SumTime > FadeInDeltaTime)
		{
			Canvas->SetRenderOpacity(CanvasOpacity - FadeInDeltaOpacity);
			CanvasOpacity = Canvas->GetRenderOpacity();
			SumTime = 0.f;

			if (CanvasOpacity <= 0.f)
			{
				CanvasOpacity = 0.f;
				bFadeIn = false;
			}
		}
	}

	// FadeOut
	if (bFadeOut)
	{
		SumTime += InDeltaTime;

		if (SumTime > FadeOutDeltaTime)
		{
			Canvas->SetRenderOpacity(CanvasOpacity + FadeOutDeltaOpacity);
			CanvasOpacity = Canvas->GetRenderOpacity();
			SumTime = 0.f;

			if (CanvasOpacity >= 1.f)
			{
				CanvasOpacity = 1.f;
				bFadeOut = false;
			}
		}
	}
}