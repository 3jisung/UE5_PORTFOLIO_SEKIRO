// Fill out your copyright notice in the Description page of Project Settings.


#include "BossInfoWidget.h"


void UBossInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	BossHP = Cast<UImage>(GetWidgetFromName(TEXT("HP")));
	BossGetHitHP = Cast<UImage>(GetWidgetFromName(TEXT("GetHitHP")));
	BossName = Cast<UTextBlock>(GetWidgetFromName(TEXT("Name")));

	DeathblowBackgroundImage.Add(Cast<UImage>(GetWidgetFromName(TEXT("Deathblow1_Background"))));
	DeathblowBackgroundImage.Add(Cast<UImage>(GetWidgetFromName(TEXT("Deathblow2_Background"))));
	DeathblowBackgroundImage.Add(Cast<UImage>(GetWidgetFromName(TEXT("Deathblow3_Background"))));

	DeathblowImage.Add(Cast<UImage>(GetWidgetFromName(TEXT("Deathblow1"))));
	DeathblowImage.Add(Cast<UImage>(GetWidgetFromName(TEXT("Deathblow2"))));
	DeathblowImage.Add(Cast<UImage>(GetWidgetFromName(TEXT("Deathblow3"))));

	for (size_t i = 0; i < DeathblowImage.Num(); i++)
	{
		DeathblowBackgroundImage[i]->SetVisibility(ESlateVisibility::Hidden);
		DeathblowImage[i]->SetOpacity(0.f);
		
	}

	Player = Cast<APlayerSekiro>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	MaxHPSize = Cast<UCanvasPanelSlot>(BossHP->Slot)->GetSize().X;		// 390.0
}

void UBossInfoWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (Player->TargetBoss != nullptr)
	{
		BossName->SetText(FText::FromName(Player->TargetBoss->Tags[2]));

		int TargetMaxDeathblowCount = Player->TargetBoss->GetMaxDeathblowCount();
		int TargetDeathblowCount = Player->TargetBoss->GetDeathblowCount();

		if (TargetMaxDeathblowCount <= DeathblowImage.Num())
		{
			for (size_t i = 0; i < DeathblowImage.Num(); i++)
			{
				if (i < TargetMaxDeathblowCount)
				{
					DeathblowBackgroundImage[i]->SetVisibility(ESlateVisibility::Visible);
				}
				else
				{
					DeathblowBackgroundImage[i]->SetVisibility(ESlateVisibility::Hidden);
				}
			}

			if (TargetDeathblowCount != DeathblowCount)
			{
				UpdateDeathblowUI(TargetDeathblowCount, InDeltaTime);
			}
		}

		if (Player->TargetBoss->GetHP() != CurHPValue)
		{
			GetHitEvent(Player->TargetBoss->GetHP() - CurHPValue);
			CurHPValue = Player->TargetBoss->GetHP();
		}	
	}

	if (Cast<UCanvasPanelSlot>(BossGetHitHP->Slot)->GetSize().X > 0)
	{
		if (bAdjustGetHitHP)
		{
			double TickXSize = Cast<UCanvasPanelSlot>(BossGetHitHP->Slot)->GetSize().X - 2.5;

			if (TickXSize < 0)
			{
				TickXSize = 0;
			}

			FVector2D AdjustSize = FVector2D(TickXSize, Cast<UCanvasPanelSlot>(BossGetHitHP->Slot)->GetSize().Y);
			Cast<UCanvasPanelSlot>(BossGetHitHP->Slot)->SetSize(AdjustSize);
		}
	}
	else
	{
		bAdjustGetHitHP = false;
	}
}

void UBossInfoWidget::GetHitEvent(float HPDifference)
{
	FVector2D HPScale = FVector2D((Player->TargetBoss->GetHP() / Player->TargetBoss->GetMaxHP()), 1);
	BossHP->SetRenderScale(HPScale);

	// 피가 줄어든 경우에만 피격바 출력
	if (HPDifference < 0.f)
	{
		// 피격바 시작점 계산
		double AdjustXPosition = Cast<UCanvasPanelSlot>(BossHP->Slot)->GetPosition().X + (HPScale.X * MaxHPSize);
		FVector2D GetHitHPPosition = FVector2D(AdjustXPosition, Cast<UCanvasPanelSlot>(BossGetHitHP->Slot)->GetPosition().Y);
		Cast<UCanvasPanelSlot>(BossGetHitHP->Slot)->SetPosition(GetHitHPPosition);

		// 피격바 크기 계산(기존 피격바가 남아있을 경우 누적)
		double AdjustXSize = FMath::Abs(HPDifference / Player->TargetBoss->GetMaxHP()) * MaxHPSize + Cast<UCanvasPanelSlot>(BossGetHitHP->Slot)->GetSize().X;
		FVector2D GetHitHPSize = FVector2D(AdjustXSize, Cast<UCanvasPanelSlot>(BossGetHitHP->Slot)->GetSize().Y);
		Cast<UCanvasPanelSlot>(BossGetHitHP->Slot)->SetSize(GetHitHPSize);

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
		double AdjustXPosition = Cast<UCanvasPanelSlot>(BossHP->Slot)->GetPosition().X + (HPScale.X * MaxHPSize);
		FVector2D GetHitHPPosition = FVector2D(AdjustXPosition, Cast<UCanvasPanelSlot>(BossGetHitHP->Slot)->GetPosition().Y);
		Cast<UCanvasPanelSlot>(BossGetHitHP->Slot)->SetPosition(GetHitHPPosition);

		// 피격바 사이즈 수정
		double AdjustXSize = Cast<UCanvasPanelSlot>(BossGetHitHP->Slot)->GetSize().X - (MaxHPSize * (HPDifference / Player->TargetBoss->GetMaxHP()));
		if (AdjustXSize <= 0)
		{
			AdjustXSize = 0;
		}

		FVector2D AdjustSize = FVector2D(AdjustXSize, Cast<UCanvasPanelSlot>(BossGetHitHP->Slot)->GetSize().Y);
		Cast<UCanvasPanelSlot>(BossGetHitHP->Slot)->SetSize(AdjustSize);
	}
}

void UBossInfoWidget::UpdateDeathblowUI(int NewDeathblowCount, float Delta)
{
	static float SumTime = 0.f;

	SumTime += Delta;

	if (SumTime > 0.05f)
	{
		if (DeathblowCount < NewDeathblowCount)
		{
			for (size_t i = DeathblowCount; i < NewDeathblowCount; i++)
			{
				DeathblowImage[i]->SetOpacity(DeathblowImage[i]->ColorAndOpacity.A + 0.1f);

				if (i == NewDeathblowCount - 1 && DeathblowImage[i]->ColorAndOpacity.A >= 1.f)
				{
					DeathblowCount = NewDeathblowCount;
				}
			}
		}
		else
		{
			for (size_t i = NewDeathblowCount; i < DeathblowCount; i++)
			{
				DeathblowImage[i]->SetOpacity(DeathblowImage[i]->ColorAndOpacity.A - 0.1f);

				if (i == NewDeathblowCount - 1 && DeathblowImage[i]->ColorAndOpacity.A <= 0.f)
				{
					DeathblowCount = NewDeathblowCount;
				}
			}
		}

		SumTime = 0.f;
	}
}