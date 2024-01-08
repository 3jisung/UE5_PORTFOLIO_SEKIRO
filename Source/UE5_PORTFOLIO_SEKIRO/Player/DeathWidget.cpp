// Fill out your copyright notice in the Description page of Project Settings.


#include "DeathWidget.h"
#include "Kismet/KismetSystemLibrary.h"

void UDeathWidget::NativeConstruct()
{
	Super::NativeConstruct();

	BackgroundImage = Cast<UImage>(GetWidgetFromName(TEXT("Background")));
	DeathUIImage = Cast<UImage>(GetWidgetFromName(TEXT("DeathUI")));

	BackgroundOn();
	DeathUIOn();
}

void UDeathWidget::NativeDestruct()
{
	Super::NativeDestruct();

}

void UDeathWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	if (bOnBackground)
	{
		BackgroundSumTime += InDeltaTime;

		if (BackgroundSumTime > 0.03f)
		{
			BackgroundImage->SetOpacity(BackgroundOpacity + 0.015f);
			BackgroundOpacity = BackgroundImage->ColorAndOpacity.A;
			BackgroundSumTime = 0.0f;

			if (BackgroundOpacity >= 1.0f)
			{
				BackgroundOpacity = 1.0f;
				bOnBackground = false;

				FTimerHandle myTimerHandle;
				float delayTime = 2.0f;
				GetWorld()->GetTimerManager().SetTimer(myTimerHandle, FTimerDelegate::CreateLambda([&]()
					{
						DeathUIOff();

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
			BackgroundImage->SetOpacity(BackgroundOpacity - 0.015f);
			BackgroundOpacity = BackgroundImage->ColorAndOpacity.A;
			BackgroundSumTime = 0.0f;

			if (BackgroundOpacity <= 0.0f)
			{
				BackgroundOpacity = 0.0f;
				bOffBackground = false;
			}
		}
	}

	if (bOnDeathUI)
	{
		DeathUISumTime += InDeltaTime;

		if (DeathUISumTime > 0.03f)
		{
			DeathUIImage->SetOpacity(DeathUIOpacity + 0.015f);
			DeathUIOpacity = DeathUIImage->ColorAndOpacity.A;
			DeathUISumTime = 0.0f;

			if (DeathUIOpacity >= 1.0f)
			{
				DeathUIOpacity = 1.0f;
				bOnDeathUI = false;
			}
		}
	}

	if (bOffDeathUI)
	{
		DeathUISumTime += InDeltaTime;

		if (DeathUISumTime > 0.03f)
		{
			DeathUIImage->SetOpacity(DeathUIOpacity - 0.015f);
			DeathUIOpacity = DeathUIImage->ColorAndOpacity.A;
			DeathUISumTime = 0.0f;

			if (DeathUIOpacity <= 0.0f)
			{
				DeathUIOpacity = 0.0f;
				bOffDeathUI = false;

				FTimerHandle myTimerHandle;
				float delayTime = 1.0f;
				GetWorld()->GetTimerManager().SetTimer(myTimerHandle, FTimerDelegate::CreateLambda([&]()
					{
						UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, false);

						GetWorld()->GetTimerManager().ClearTimer(myTimerHandle);
					}), delayTime, false);
			}
		}
	}
}