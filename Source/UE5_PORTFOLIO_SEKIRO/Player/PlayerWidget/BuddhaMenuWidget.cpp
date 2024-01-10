// Fill out your copyright notice in the Description page of Project Settings.


#include "BuddhaMenuWidget.h"


void UBuddhaMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	MapNameText = Cast<UTextBlock>(GetWidgetFromName(TEXT("MapName")));
	MapNameText->SetText(FText());

	FadeInDeltaTime = 0.01f;
	FadeOutDeltaTime = 0.01f;
	FadeInDeltaOpacity = 0.1f;
	FadeOutDeltaOpacity = 0.1f;

	FadeIn();
}