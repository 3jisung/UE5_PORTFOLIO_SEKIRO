// Fill out your copyright notice in the Description page of Project Settings.


#include "DeathWidget.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameMode/GlobalGameMode.h"


void UDeathWidget::NativeConstruct()
{
	Super::NativeConstruct();

	BackgroundImage = Cast<UImage>(GetWidgetFromName(TEXT("Background")));
	DeathUIImage = Cast<UImage>(GetWidgetFromName(TEXT("DeathUI")));

	BackgroundOn();
	DeathUIOn();
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
			BackgroundSumTime = 0.f;

			if (BackgroundOpacity >= 1.f)
			{
				BackgroundOpacity = 1.f;
				bOnBackground = false;

				FTimerHandle myTimerHandle;
				float delayTime = 2.f;
				GetWorld()->GetTimerManager().SetTimer(myTimerHandle, FTimerDelegate::CreateLambda([&]()
					{
						DeathUIOff();

						AGlobalGameMode* GameMode = Cast<AGlobalGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
						if (GameMode != nullptr)
						{
							GameMode->StopSound();
						}

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
			BackgroundSumTime = 0.f;

			if (BackgroundOpacity <= 0.f)
			{
				BackgroundOpacity = 0.f;
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
			DeathUISumTime = 0.f;

			if (DeathUIOpacity >= 1.f)
			{
				DeathUIOpacity = 1.f;
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
			DeathUISumTime = 0.f;

			if (DeathUIOpacity <= 0.f)
			{
				DeathUIOpacity = 0.f;
				bOffDeathUI = false;

				FTimerHandle myTimerHandle;
				float delayTime = 1.f;
				GetWorld()->GetTimerManager().SetTimer(myTimerHandle, FTimerDelegate::CreateLambda([&]()
					{
						UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, false);

						GetWorld()->GetTimerManager().ClearTimer(myTimerHandle);
					}), delayTime, false);
			}
		}
	}
}