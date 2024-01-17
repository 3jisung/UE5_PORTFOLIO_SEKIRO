// Fill out your copyright notice in the Description page of Project Settings.


#include "TitleWidget.h"
#include "GameMode/GlobalGameMode.h"


void UTitleWidget::NativeConstruct()
{
	UImage* Hover1 = Cast<UImage>(GetWidgetFromName(TEXT("Hovered_Start")));
	if (IsValid(Hover1))
	{
		BtnHoveredImage.Add(Hover1);
	}

	UImage* Hover2 = Cast<UImage>(GetWidgetFromName(TEXT("Hovered_Exit")));
	if (IsValid(Hover2))
	{
		BtnHoveredImage.Add(Hover2);
	}

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

	UGlobalGameInstance* Inst = GetGameInstance<UGlobalGameInstance>();
	TSubclassOf<UUserWidget> WidgetClass = Inst->GetWidgetClassData(TEXT("Global"), TEXT("SceneTransition"));
	if (IsValid(WidgetClass))
	{
		UFadeInOutWidget* SceneTransitionWidget = Cast<UFadeInOutWidget>(CreateWidget(GetWorld(), WidgetClass));

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

	
}
