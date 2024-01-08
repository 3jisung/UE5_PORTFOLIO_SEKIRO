// Fill out your copyright notice in the Description page of Project Settings.


#include "DeathblowWidget.h"


void UDeathblowWidget::NativeConstruct()
{
	Super::NativeConstruct();

	DeathblowIcon = Cast<UImage>(GetWidgetFromName(TEXT("Icon")));
}

void UDeathblowWidget::NativeDestruct()
{
	Super::NativeDestruct();
	
}

void UDeathblowWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	if (bOnIcon)
	{
		SumTime += InDeltaTime;

		if (SumTime > 0.01f)
		{
			DeathblowIcon->SetOpacity(IconOpacity + 0.1f);
			IconOpacity = DeathblowIcon->ColorAndOpacity.A;
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

		if (SumTime > 0.01f)
		{
			DeathblowIcon->SetOpacity(IconOpacity - 0.1f);
			IconOpacity = DeathblowIcon->ColorAndOpacity.A;
			SumTime = 0.0f;

			if (IconOpacity <= 0.0f)
			{
				IconOpacity = 0.0f;
				bOffIcon = false;
			}
		}
	}
}