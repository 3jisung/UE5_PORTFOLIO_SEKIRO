// Fill out your copyright notice in the Description page of Project Settings.


#include "WarningWidget.h"


void UWarningWidget::NativeConstruct()
{
	Super::NativeConstruct();

	FadeInDeltaTime = 0.01f;
	FadeOutDeltaTime = 0.03f;
	FadeInDeltaOpacity = 0.1f;
	FadeOutDeltaOpacity = 0.1f;
}
