// Fill out your copyright notice in the Description page of Project Settings.


#include "DeathblowWidget.h"


void UDeathblowWidget::NativeConstruct()
{
	Super::NativeConstruct();

	DeathblowIcon = Cast<UImage>(GetWidgetFromName(TEXT("Icon")));

	IconOn();
}

void UDeathblowWidget::NativeDestruct()
{
	Super::NativeDestruct();
	
	// IconOff();
}

void UDeathblowWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	if (bOnIcon)
	{
		SumTime += InDeltaTime;

		if (SumTime > 0.05)
		{
			DeathblowIcon->SetOpacity(IconOpacity + 0.2);
			IconOpacity = DeathblowIcon->ColorAndOpacity.A;
			SumTime = 0;

			if (IconOpacity >= 1)
			{
				bOnIcon = false;
			}
		}
	}

	if (bOffIcon)
	{
		SumTime += InDeltaTime;

		if (SumTime > 0.05)
		{
			DeathblowIcon->SetOpacity(IconOpacity - 0.2);
			IconOpacity = DeathblowIcon->ColorAndOpacity.A;
			SumTime = 0;

			if (IconOpacity <= 0)
			{
				bOffIcon = false;
			}
		}
	}
}