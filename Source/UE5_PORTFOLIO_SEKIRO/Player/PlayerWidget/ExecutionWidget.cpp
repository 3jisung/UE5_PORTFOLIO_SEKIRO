// Fill out your copyright notice in the Description page of Project Settings.


#include "ExecutionWidget.h"


void UExecutionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	FadeInDeltaTime = 0.01f;
	FadeOutDeltaTime = 0.03f;
	FadeInDeltaOpacity = 0.05f;
	FadeOutDeltaOpacity = 0.03f;
	TimerDelayTime = 5.f;
	bDestructWidget = true;

	FadeIn();
}
