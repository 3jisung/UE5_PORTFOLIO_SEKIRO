// Fill out your copyright notice in the Description page of Project Settings.


#include "MainWidget.h"


void UMainWidget::NativeConstruct()
{
	Super::NativeConstruct();

	GourdImage = Cast<UImage>(GetWidgetFromName(TEXT("Gourd")));
	HealCount = Cast<UTextBlock>(GetWidgetFromName(TEXT("GourdCount")));
	HPWidget = Cast<UHPWidget>(GetWidgetFromName(TEXT("WBP_HPWidget")));

	Player = Cast<APlayerSekiro>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	HPWidget->CharacterSetting(Player);
}

void UMainWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

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