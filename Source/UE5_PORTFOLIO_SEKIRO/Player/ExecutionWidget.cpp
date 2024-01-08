// Fill out your copyright notice in the Description page of Project Settings.


#include "ExecutionWidget.h"


void UExecutionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	BackgroundImage = Cast<UImage>(GetWidgetFromName(TEXT("Background")));
	ExecutionUIImage = Cast<UImage>(GetWidgetFromName(TEXT("ExecutionUI")));

	BackgroundOn();
	ExecutionUIOn();
}

void UExecutionWidget::NativeDestruct()
{
	Super::NativeDestruct();

}

void UExecutionWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	if (bOnBackground)
	{
		BackgroundSumTime += InDeltaTime;

		if (BackgroundSumTime > 0.01f)
		{
			BackgroundImage->SetOpacity(BackgroundOpacity + 0.05f);
			BackgroundOpacity = BackgroundImage->ColorAndOpacity.A;
			BackgroundSumTime = 0.0f;

			if (BackgroundOpacity >= 1.0f)
			{
				BackgroundOpacity = 1.0f;
				bOnBackground = false;

				FTimerHandle myTimerHandle;
				float delayTime = 5.0f;
				GetWorld()->GetTimerManager().SetTimer(myTimerHandle, FTimerDelegate::CreateLambda([&]()
					{
						BackgroundOff();
						ExecutionUIOff();

						GetWorld()->GetTimerManager().ClearTimer(myTimerHandle);
					}), delayTime, false);
			}
		}
	}

	if (bOffBackground)
	{
		BackgroundSumTime += InDeltaTime;

		if (BackgroundSumTime > 0.03f)
		{
			BackgroundImage->SetOpacity(BackgroundOpacity - 0.03f);
			BackgroundOpacity = BackgroundImage->ColorAndOpacity.A;
			BackgroundSumTime = 0.0f;

			if (BackgroundOpacity <= 0.0f)
			{
				BackgroundOpacity = 0.0f;
				bOffBackground = false;
			}
		}
	}

	if (bOnExecutionUI)
	{
		ExecutionUISumTime += InDeltaTime;

		if (ExecutionUISumTime > 0.01f)
		{
			ExecutionUIImage->SetOpacity(ExecutionUIOpacity + 0.05f);
			ExecutionUIOpacity = ExecutionUIImage->ColorAndOpacity.A;
			ExecutionUISumTime = 0.0f;

			if (ExecutionUIOpacity >= 1.0f)
			{
				ExecutionUIOpacity = 1.0f;
				bOnExecutionUI = false;
			}
		}
	}

	if (bOffExecutionUI)
	{
		ExecutionUISumTime += InDeltaTime;

		if (ExecutionUISumTime > 0.03f)
		{
			ExecutionUIImage->SetOpacity(ExecutionUIOpacity - 0.03f);
			ExecutionUIOpacity = ExecutionUIImage->ColorAndOpacity.A;
			ExecutionUISumTime = 0.0f;

			if (ExecutionUIOpacity <= 0.0f)
			{
				ExecutionUIOpacity = 0.0f;
				bOffExecutionUI = false;
			}
		}
	}
}