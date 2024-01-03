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

		if (SumTime > 0.01)
		{
			WarningIcon->SetOpacity(IconOpacity + 0.1);
			IconOpacity = WarningIcon->ColorAndOpacity.A;
			SumTime = 0;

			if (IconOpacity >= 1)
			{
				IconOpacity = 1.0f;
				bOnIcon = false;
			}
		}
	}

	if (bOffIcon)
	{
		SumTime += InDeltaTime;

		if (SumTime > 0.03)
		{
			WarningIcon->SetOpacity(IconOpacity - 0.1);
			IconOpacity = WarningIcon->ColorAndOpacity.A;
			SumTime = 0;

			if (IconOpacity <= 0)
			{
				IconOpacity = 0.0f;
				bOffIcon = false;
			}
		}
	}
}