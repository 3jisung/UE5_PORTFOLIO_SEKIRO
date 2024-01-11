// Fill out your copyright notice in the Description page of Project Settings.


#include "TitleWidget.h"
#include "GameMode/GlobalGameMode.h"


void UTitleWidget::NativeConstruct()
{
	BtnHoveredImage.Add(Cast<UImage>(GetWidgetFromName(TEXT("Hovered_Start"))));
	BtnHoveredImage.Add(Cast<UImage>(GetWidgetFromName(TEXT("Hovered_Exit"))));

	Super::NativeConstruct();

	FadeInDeltaTime = 0.01f;
	FadeOutDeltaTime = 0.01f;
	FadeInDeltaOpacity = 0.1f;
	FadeOutDeltaOpacity = 0.1f;

	FadeIn();
}

void UTitleWidget::MenuEvent()
{
	Super::MenuEvent();

	AGlobalGameMode* GameMode = Cast<AGlobalGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	GameMode->StopSound();

	if (IsValid(SceneTransitionWidget))
	{
		SceneTransitionWidget->AddToViewport();

		SceneTransitionWidget->FadeOut();

		FTimerHandle myTimerHandle;
		float DelayTime = 1.f;
		GetWorld()->GetTimerManager().SetTimer(myTimerHandle, FTimerDelegate::CreateLambda([&]()
			{
				switch (HoveredIndex)
				{
				case 0:
				{
					UGameplayStatics::OpenLevel(GetWorld(), TEXT("Stage1"));
					break;
				}

				case 1:
				{
					UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, false);
					break;
				}

				default:
					break;
				}

				GetWorld()->GetTimerManager().ClearTimer(myTimerHandle);
			}), DelayTime, false);
	}
}
