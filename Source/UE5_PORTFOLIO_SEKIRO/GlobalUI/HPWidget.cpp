// Fill out your copyright notice in the Description page of Project Settings.


#include "HPWidget.h"


void UHPWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CharacterHP = Cast<UImage>(GetWidgetFromName(TEXT("HP")));
	CharacterGetHitHP = Cast<UImage>(GetWidgetFromName(TEXT("GetHitHP")));

	MaxHPSize = Cast<UCanvasPanelSlot>(CharacterHP->Slot)->GetSize().X;		// 390.0
}

void UHPWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (IsValid(Character) == false)
	{
		return;
	}

	// 피격 시 HP바 이벤트 발생
	if (Character->GetHP() != CurHPValue)
	{
		GetHitEvent(Character->GetHP() - CurHPValue);
		CurHPValue = Character->GetHP();
	}

	// 피격 바(회색 바)는 시간에 따라 점차 감소
	if (Cast<UCanvasPanelSlot>(CharacterGetHitHP->Slot)->GetSize().X > 0)
	{
		if (bAdjustGetHitHP)
		{
			double TickXSize = Cast<UCanvasPanelSlot>(CharacterGetHitHP->Slot)->GetSize().X - 2.5;

			if (TickXSize < 0)
			{
				TickXSize = 0;
			}

			FVector2D AdjustSize = FVector2D(TickXSize, Cast<UCanvasPanelSlot>(CharacterGetHitHP->Slot)->GetSize().Y);
			Cast<UCanvasPanelSlot>(CharacterGetHitHP->Slot)->SetSize(AdjustSize);
		}
	}
	else
	{
		bAdjustGetHitHP = false;
	}
}

void UHPWidget::GetHitEvent(float HPDifference)
{
	FVector2D HPScale = FVector2D((Character->GetHP() / Character->GetMaxHP()), 1);
	CharacterHP->SetRenderScale(HPScale);

	// 피가 줄어든 경우에만 피격바 출력
	if (HPDifference < 0.f)
	{
		// 피격바 시작점 계산
		double AdjustXPosition = Cast<UCanvasPanelSlot>(CharacterHP->Slot)->GetPosition().X + (HPScale.X * MaxHPSize);
		FVector2D GetHitHPPosition = FVector2D(AdjustXPosition, Cast<UCanvasPanelSlot>(CharacterGetHitHP->Slot)->GetPosition().Y);
		Cast<UCanvasPanelSlot>(CharacterGetHitHP->Slot)->SetPosition(GetHitHPPosition);

		// 피격바 크기 계산(기존 피격바가 남아있을 경우 누적)
		double AdjustXSize = FMath::Abs(HPDifference / Character->GetMaxHP()) * MaxHPSize + Cast<UCanvasPanelSlot>(CharacterGetHitHP->Slot)->GetSize().X;
		FVector2D GetHitHPSize = FVector2D(AdjustXSize, Cast<UCanvasPanelSlot>(CharacterGetHitHP->Slot)->GetSize().Y);
		Cast<UCanvasPanelSlot>(CharacterGetHitHP->Slot)->SetSize(GetHitHPSize);

		// 1초 동안 피격바 크기 유지 후 Size 감소 시작
		float delayTime = 1.f;
		FTimerHandle myTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(myTimerHandle, FTimerDelegate::CreateLambda([&]()
			{
				bAdjustGetHitHP = true;

				GetWorld()->GetTimerManager().ClearTimer(myTimerHandle);
			}), delayTime, false);
	}
	// 피가 찼을 경우 피격바 조정
	else
	{
		// 피격바 시작점 계산
		double AdjustXPosition = Cast<UCanvasPanelSlot>(CharacterHP->Slot)->GetPosition().X + (HPScale.X * MaxHPSize);
		FVector2D GetHitHPPosition = FVector2D(AdjustXPosition, Cast<UCanvasPanelSlot>(CharacterGetHitHP->Slot)->GetPosition().Y);
		Cast<UCanvasPanelSlot>(CharacterGetHitHP->Slot)->SetPosition(GetHitHPPosition);

		// 피격바 사이즈 수정
		double AdjustXSize = Cast<UCanvasPanelSlot>(CharacterGetHitHP->Slot)->GetSize().X - (MaxHPSize * (HPDifference / Character->GetMaxHP()));
		if (AdjustXSize <= 0)
		{
			AdjustXSize = 0;
		}

		FVector2D AdjustSize = FVector2D(AdjustXSize, Cast<UCanvasPanelSlot>(CharacterGetHitHP->Slot)->GetSize().Y);
		Cast<UCanvasPanelSlot>(CharacterGetHitHP->Slot)->SetSize(AdjustSize);
	}
}