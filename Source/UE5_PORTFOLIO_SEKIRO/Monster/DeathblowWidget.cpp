// Fill out your copyright notice in the Description page of Project Settings.


#include "DeathblowWidget.h"


void UDeathblowWidget::NativeConstruct()
{
	Super::NativeConstruct();

	FadeInDeltaTime = 0.01f;
	FadeOutDeltaTime = 0.01f;
	FadeInDeltaOpacity = 0.1f;
	FadeOutDeltaOpacity = 0.1f;
}