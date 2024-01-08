// Fill out your copyright notice in the Description page of Project Settings.


#include "FadeInOutWidget.h"

void UFadeInOutWidget::NativeConstruct()
{
	Super::NativeConstruct();

	FadeScreen = Cast<UImage>(GetWidgetFromName(TEXT("FadeInOutScreen")));
}

void UFadeInOutWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	// FadeIn
	if (FadeInState)
	{
		SumTime += InDeltaTime;

		if (SumTime > 0.05f)
		{
			FadeScreen->SetOpacity(ScreenOpacity - 0.1f);
			ScreenOpacity = FadeScreen->ColorAndOpacity.A;
			SumTime = 0.0f;

			if (ScreenOpacity <= 0.0f)
			{
				ScreenOpacity = 0.0f;
				FadeInState = false;
			}
		}
	}

	// FadeOut
	if (FadeOutState)
	{
		SumTime += InDeltaTime;

		if (SumTime > 0.05f)
		{
			FadeScreen->SetOpacity(ScreenOpacity + 0.2f);
			ScreenOpacity = FadeScreen->ColorAndOpacity.A;
			SumTime = 0.0f;

			if (ScreenOpacity >= 1.0f)
			{
				ScreenOpacity = 1.0f;
				FadeOutState = false;
			}
		}
	}
}