// Fill out your copyright notice in the Description page of Project Settings.


#include "MainWidget.h"


void UMainWidget::NativeConstruct()
{
	Super::NativeConstruct();

	PlayerHP = Cast<UImage>(GetWidgetFromName(TEXT("HP")));
	PlayerGetHitHP = Cast<UImage>(GetWidgetFromName(TEXT("GetHitHP")));

	Player = Cast<APlayerSekiro>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	MaxHPSize = Cast<UCanvasPanelSlot>(PlayerHP->Slot)->GetSize().X;		// 390.0
	CurHPValue = Player->GetMaxHP();
}

void UMainWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (Player->GetHP() != CurHPValue)
	{
		GetHitEvent(Player->GetHP() - CurHPValue);
		CurHPValue = Player->GetHP();
	}

	if (Cast<UCanvasPanelSlot>(PlayerGetHitHP->Slot)->GetSize().X > 0)
	{
		if (bAdjustGetHitHP)
		{
			double TickXSize = Cast<UCanvasPanelSlot>(PlayerGetHitHP->Slot)->GetSize().X - 2.5;

			if (TickXSize < 0)
			{
				TickXSize = 0;
			}

			FVector2D AdjustSize = FVector2D(TickXSize, Cast<UCanvasPanelSlot>(PlayerGetHitHP->Slot)->GetSize().Y);
			Cast<UCanvasPanelSlot>(PlayerGetHitHP->Slot)->SetSize(AdjustSize);
		}
	}
	else
	{
		bAdjustGetHitHP = false;
	}
}

void UMainWidget::GetHitEvent(float HPDifference)
{
	FVector2D HPScale = FVector2D((Player->GetHP() / Player->GetMaxHP()), 1);
	PlayerHP->SetRenderScale(HPScale);
	
	// 피가 줄어든 경우에만 피격바 출력
	if (HPDifference < 0)
	{
		// 피격바 시작점 계산
		double AdjustXPosition = Cast<UCanvasPanelSlot>(PlayerHP->Slot)->GetPosition().X + (HPScale.X * MaxHPSize);
		FVector2D GetHitHPPosition = FVector2D(AdjustXPosition, Cast<UCanvasPanelSlot>(PlayerGetHitHP->Slot)->GetPosition().Y);
		Cast<UCanvasPanelSlot>(PlayerGetHitHP->Slot)->SetPosition(GetHitHPPosition);

		// 피격바 크기 계산(기존 피격바가 남아있을 경우 누적)
		double AdjustXSize = FMath::Abs(HPDifference / Player->GetMaxHP()) * MaxHPSize + Cast<UCanvasPanelSlot>(PlayerGetHitHP->Slot)->GetSize().X;
		FVector2D GetHitHPSize = FVector2D(AdjustXSize, Cast<UCanvasPanelSlot>(PlayerGetHitHP->Slot)->GetSize().Y);
		Cast<UCanvasPanelSlot>(PlayerGetHitHP->Slot)->SetSize(GetHitHPSize);

		// 1초 동안 피격바 크기 유지 후 Size 감소 시작
		float delayTime = 1.0f;
		FTimerHandle myTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(myTimerHandle, FTimerDelegate::CreateLambda([&]()
			{
				bAdjustGetHitHP = true;

				GetWorld()->GetTimerManager().ClearTimer(myTimerHandle);
			}), delayTime, false);
	}
}