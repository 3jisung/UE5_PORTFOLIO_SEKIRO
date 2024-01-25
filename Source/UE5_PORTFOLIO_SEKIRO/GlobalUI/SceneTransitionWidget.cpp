// Fill out your copyright notice in the Description page of Project Settings.


#include "SceneTransitionWidget.h"


void USceneTransitionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	FadeInDeltaTime = 0.01f;
	FadeOutDeltaTime = 0.01f;
	FadeInDeltaOpacity = 0.04f;
	FadeOutDeltaOpacity = 0.02f;
}