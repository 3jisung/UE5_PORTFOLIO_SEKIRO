// Fill out your copyright notice in the Description page of Project Settings.


#include "WarningWidget.h"


void UWarningWidget::NativeConstruct()
{
	Super::NativeConstruct();

	WarningIcon = Cast<UImage>(GetWidgetFromName(TEXT("Icon")));
}

void UWarningWidget::NativeDestruct()
{
	Super::NativeDestruct();

}

void UWarningWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	if (bOnIcon)
	{
		SumTime += InDeltaTime;

		if (SumTime > 0.01f)
		{
			WarningIcon->SetOpacity(IconOpacity + 0.1f);
			IconOpacity = WarningIcon->ColorAndOpacity.A;
			SumTime = 0.0f;

			if (IconOpacity >= 1.0f)
			{
				IconOpacity = 1.0f;
				bOnIcon = false;
			}
		}
	}

	if (bOffIcon)
	{
		SumTime += InDeltaTime;

		if (SumTime > 0.03f)
		{
			WarningIcon->SetOpacity(IconOpacity - 0.1f);
			IconOpacity = WarningIcon->ColorAndOpacity.A;
			SumTime = 0.0f;

			if (IconOpacity <= 0.0f)
			{
				IconOpacity = 0.0f;
				bOffIcon = false;
			}
		}
	}
}