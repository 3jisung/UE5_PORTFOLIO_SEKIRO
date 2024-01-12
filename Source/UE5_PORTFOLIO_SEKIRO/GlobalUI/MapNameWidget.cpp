// Fill out your copyright notice in the Description page of Project Settings.


#include "MapNameWidget.h"


void UMapNameWidget::NativeConstruct()
{
	Super::NativeConstruct();

	MapNameText = Cast<UTextBlock>(GetWidgetFromName(TEXT("MapName")));
	MapNameText->SetText(FText());

	FadeInDeltaTime = 0.03f;
	FadeOutDeltaTime = 0.03f;
	FadeInDeltaOpacity = 0.1f;
	FadeOutDeltaOpacity = 0.1f;
	TimerDelayTime = 2.f;
	bDestructWidget = true;
}

void UMapNameWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	if (MapNameText->GetText().ToString() == "")
	{
		return;
	}
	
	Super::NativeTick(MyGeometry, InDeltaTime);
}