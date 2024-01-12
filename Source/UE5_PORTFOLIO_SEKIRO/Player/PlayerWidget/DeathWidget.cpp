// Fill out your copyright notice in the Description page of Project Settings.


#include "DeathWidget.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameMode/GlobalGameMode.h"


void UDeathWidget::NativeConstruct()
{
	Super::NativeConstruct();

	BackgroundImage = Cast<UImage>(GetWidgetFromName(TEXT("Background")));
	DeathUIImage = Cast<UImage>(GetWidgetFromName(TEXT("DeathUI")));

	FadeInDeltaTime = 0.03f;
	FadeOutDeltaTime = 0.03f;
	FadeInDeltaOpacity = 0.015f;
	FadeOutDeltaOpacity = 0.015f;
	TimerDelayTime = 2.f;

	FadeIn();
}

void UDeathWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	if (bFadeIn)
	{
		SumTime += InDeltaTime;

		if (SumTime > FadeInDeltaTime)
		{
			Canvas->SetRenderOpacity(CanvasOpacity + FadeInDeltaOpacity);
			CanvasOpacity = Canvas->GetRenderOpacity();
			SumTime = 0.f;

			if (CanvasOpacity >= 1.f)
			{
				CanvasOpacity = 1.f;
				bFadeIn = false;

				FTimerHandle myTimerHandle;
				GetWorld()->GetTimerManager().SetTimer(myTimerHandle, FTimerDelegate::CreateLambda([&]()
					{
						DeathUIOff();

						AGlobalGameMode* GameMode = Cast<AGlobalGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
						if (GameMode != nullptr)
						{
							GameMode->StopSound();
						}

						GetWorld()->GetTimerManager().ClearTimer(myTimerHandle);
					}), TimerDelayTime, false);
			}
		}
	}
	
	if (bOnBackground)
	{
		BackgroundSumTime += InDeltaTime;

		if (BackgroundSumTime > FadeInDeltaTime)
		{
			BackgroundImage->SetOpacity(BackgroundOpacity + FadeInDeltaOpacity);
			BackgroundOpacity = BackgroundImage->ColorAndOpacity.A;
			BackgroundSumTime = 0.f;

			if (BackgroundOpacity >= 1.f)
			{
				BackgroundOpacity = 1.f;
				bOnBackground = false;
			}
		}
	}

	if (bOffBackground)
	{
		BackgroundSumTime += InDeltaTime;

		if (BackgroundSumTime > FadeOutDeltaTime)
		{
			BackgroundImage->SetOpacity(BackgroundOpacity - FadeOutDeltaOpacity);
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

		if (DeathUISumTime > FadeInDeltaTime)
		{
			DeathUIImage->SetOpacity(DeathUIOpacity + FadeInDeltaOpacity);
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

		if (DeathUISumTime > FadeOutDeltaTime)
		{
			DeathUIImage->SetOpacity(DeathUIOpacity - FadeOutDeltaOpacity);
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