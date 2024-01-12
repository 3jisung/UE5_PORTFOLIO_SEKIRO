// Fill out your copyright notice in the Description page of Project Settings.


#include "MainWidget.h"
#include "Player/PlayerSekiro.h"


void UMainWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Canvas)
	{
		Canvas->SetRenderOpacity(1.f);
	}

	GourdImage = Cast<UImage>(GetWidgetFromName(TEXT("Gourd")));
	HealCount = Cast<UTextBlock>(GetWidgetFromName(TEXT("GourdCount")));
	HPWidget = Cast<UHPWidget>(GetWidgetFromName(TEXT("WBP_HPWidget")));
	PlayerPostureWidget = Cast<UPostureWidget>(GetWidgetFromName(TEXT("WBP_PlayerPosture")));

	if (PlayerController)
	{
		Player = Cast<APlayerSekiro>(PlayerController->GetCharacter());
		
		if (Player && IsValid(HPWidget) && IsValid(PlayerPostureWidget))
		{
			HPWidget->CharacterSetting(Player);
			PlayerPostureWidget->CharacterSetting(Player);
			PlayerPostureUIOnOff = ESlateVisibility::Hidden;
		}
	}
}

void UMainWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (Player)
	{
		// 표주박 개수 최신화
		HealCount->SetText(FText::FromString(FString::FromInt(Player->GetHealCount())));

		if (Player->GetHealCount() <= 0)
		{
			GourdImage->SetColorAndOpacity(FLinearColor(1.f, 0.2f, 0.2f, 1.f));
		}
		else
		{
			GourdImage->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 1.f));
		}
	}
}