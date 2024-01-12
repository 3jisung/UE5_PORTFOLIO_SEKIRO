// Fill out your copyright notice in the Description page of Project Settings.


#include "PostureWidget.h"


void UPostureWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CharacterPosture = Cast<UImage>(GetWidgetFromName(TEXT("Posture")));
}

void UPostureWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (IsValid(Character))
	{
		// 체간 바 실시간 조정
		float AdjustScaleX = (Character->GetMaxPosture() - Character->GetPosture()) / Character->GetMaxPosture();
		CharacterPosture->SetRenderScale(FVector2D(AdjustScaleX, 1.0));
	}
}