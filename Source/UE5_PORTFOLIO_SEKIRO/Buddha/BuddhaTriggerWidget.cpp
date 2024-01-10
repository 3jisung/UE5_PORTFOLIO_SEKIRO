// Fill out your copyright notice in the Description page of Project Settings.


#include "BuddhaTriggerWidget.h"


void UBuddhaTriggerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	FadeInDeltaTime = 0.01f;
	FadeOutDeltaTime = 0.01f;
	FadeInDeltaOpacity = 0.1f;
	FadeOutDeltaOpacity = 0.1f;
}