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

		if (SumTime > 0.05)
		{
			FadeScreen->SetOpacity(ScreenOpacity - 0.2);
			ScreenOpacity = FadeScreen->ColorAndOpacity.A;
			SumTime = 0;

			if (ScreenOpacity <= 0)
			{
				FadeInState = false;
			}
		}
	}

	// FadeOut
	if (FadeOutState)
	{
		SumTime += InDeltaTime;

		if (SumTime > 0.05)
		{
			FadeScreen->SetOpacity(ScreenOpacity + 0.2);
			ScreenOpacity = FadeScreen->ColorAndOpacity.A;
			SumTime = 0;

			if (ScreenOpacity >= 1)
			{
				FadeOutState = false;
			}
		}
	}
}