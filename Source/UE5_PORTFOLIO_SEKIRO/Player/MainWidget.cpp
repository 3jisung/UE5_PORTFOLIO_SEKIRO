// Fill out your copyright notice in the Description page of Project Settings.


#include "MainWidget.h"


void UMainWidget::NativeConstruct()
{
	Super::NativeConstruct();

	PlayerHP = Cast<UImage>(GetWidgetFromName(TEXT("HP")));
	PlayerGetHitHP = Cast<UImage>(GetWidgetFromName(TEXT("GetHitHP")));
	GourdImage = Cast<UImage>(GetWidgetFromName(TEXT("Gourd")));
	HealCount = Cast<UTextBlock>(GetWidgetFromName(TEXT("GourdCount")));

	Player = Cast<APlayerSekiro>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	MaxHPSize = Cast<UCanvasPanelSlot>(PlayerHP->Slot)->GetSize().X;		// 390.0
	CurHPValue = Player->GetMaxHP();
}

void UMainWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// 피격 시 HP바 이벤트 발생
	if (Player->GetHP() != CurHPValue)
	{
		GetHitEvent(Player->GetHP() - CurHPValue);
		CurHPValue = Player->GetHP();
	}

	// 피격 바(회색 바)는 시간에 따라 점차 감소
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

	// 표주박 개수 최신화
	HealCount->SetText(FText::FromString(FString::FromInt(Player->GetHealCount())));

	if (Player->GetHealCount() <= 0)
	{
		GourdImage->SetColorAndOpacity(FLinearColor(1.0f, 0.2f, 0.2f, 1.0f));
	}
	else
	{
		GourdImage->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
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
	// 피가 찼을 경우 피격바 조정
	else
	{
		// 피격바 시작점 계산
		double AdjustXPosition = Cast<UCanvasPanelSlot>(PlayerHP->Slot)->GetPosition().X + (HPScale.X * MaxHPSize);
		FVector2D GetHitHPPosition = FVector2D(AdjustXPosition, Cast<UCanvasPanelSlot>(PlayerGetHitHP->Slot)->GetPosition().Y);
		Cast<UCanvasPanelSlot>(PlayerGetHitHP->Slot)->SetPosition(GetHitHPPosition);

		// 피격바 사이즈 수정
		double AdjustXSize = Cast<UCanvasPanelSlot>(PlayerGetHitHP->Slot)->GetSize().X - (MaxHPSize * (HPDifference / Player->GetMaxHP()));
		if (AdjustXSize <= 0)
		{
			AdjustXSize = 0;
		}

		FVector2D AdjustSize = FVector2D(AdjustXSize, Cast<UCanvasPanelSlot>(PlayerGetHitHP->Slot)->GetSize().Y);
		Cast<UCanvasPanelSlot>(PlayerGetHitHP->Slot)->SetSize(AdjustSize);
	}
}