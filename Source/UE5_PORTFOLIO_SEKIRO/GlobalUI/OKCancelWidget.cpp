// Fill out your copyright notice in the Description page of Project Settings.


#include "OKCancelWidget.h"
#include "Player/PlayerSekiro.h"


void UOKCancelWidget::NativeConstruct()
{
	UImage* OK = Cast<UImage>(GetWidgetFromName(TEXT("Hovered_OK")));
	if (IsValid(OK))
	{
		BtnHoveredImage.Add(OK);
	}

	UImage* Cancel = Cast<UImage>(GetWidgetFromName(TEXT("Hovered_Cancel")));
	if (IsValid(Cancel))
	{
		BtnHoveredImage.Add(Cancel);
	}

	Super::NativeConstruct();

	NoticeText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Notice")));
	NoticeText->SetText(FText());

	if (PlayerController)
	{
		Player = Cast<APlayerSekiro>(PlayerController->GetCharacter());
		
		PlayerController->SetInputMode(FInputModeUIOnly());
		PlayerController->SetShowMouseCursor(true);
	}

	FadeInDeltaTime = 0.01f;
	FadeOutDeltaTime = 0.01f;
	FadeInDeltaOpacity = 0.1f;
	FadeOutDeltaOpacity = 0.1f;

	FadeIn();
}

void UOKCancelWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// UI 이벤트 도중 피격 당할 시 UI 강제 종료
	if (Player && PlayerController)
	{
		SekiroState AniStateValue = Player->GetAniState<SekiroState>();
		if (AniStateValue != SekiroState::SitStart && AniStateValue != SekiroState::SitLoop && AniStateValue != SekiroState::SitEnd)
		{
			PlayerController->SetInputMode(FInputModeGameOnly());
			PlayerController->SetShowMouseCursor(false);

			if (Player->GetbLockOn())
			{
				Player->ToggleLockOn();
			}

			if (bDestructWidget == false)
			{
				FadeOut(true);
			}
		}
	}
}

void UOKCancelWidget::MenuEvent()
{
	if (IsValid(Player) == false)
	{
		return;
	}
	else if (IsValid(Player->Controller) == false)
	{
		return;
	}

	if (IsValid(ParentWidget) && BtnHoveredImage.Num() > 1 && HoveredIndex == 0)
	{
		Super::MenuEvent();
		FadeOut(true);
		ParentWidget->PopupWidgetReturn(HoveredIndex);
	}
	else
	{
		ExitWidget();
	}
}

void UOKCancelWidget::ExitWidget()
{
	FadeOut(true);

	FTimerHandle myTimerHandle;
	float DelayTime = 0.1f;
	GetWorld()->GetTimerManager().SetTimer(myTimerHandle, FTimerDelegate::CreateLambda([&]()
		{
			if (IsValid(ParentWidget))
			{
				if (ParentWidget->GetCanvasOpacity() <= 0.f)
				{
					ParentWidget->FadeIn();
				}
				
				ParentWidget->SetFocus();
			}

			GetWorld()->GetTimerManager().ClearTimer(myTimerHandle);
		}), DelayTime, false);
}