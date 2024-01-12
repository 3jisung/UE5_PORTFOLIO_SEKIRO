// Fill out your copyright notice in the Description page of Project Settings.


#include "BossInfoWidget.h"
#include "Player/PlayerSekiro.h"


void UBossInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Canvas)
	{
		Canvas->SetRenderOpacity(1.f);
	}

	HPWidget = Cast<UHPWidget>(GetWidgetFromName(TEXT("WBP_HPWidget")));
	BossPostureWidget = Cast<UPostureWidget>(GetWidgetFromName(TEXT("WBP_BossPosture")));
	BossName = Cast<UTextBlock>(GetWidgetFromName(TEXT("Name")));

	if (IsValid(BossPostureWidget) && IsValid(BossName))
	{
		BossPostureWidget->SetVisibility(ESlateVisibility::Hidden);
		BossName->SetText(FText());
	}

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

	if (PlayerController)
	{
		Player = Cast<APlayerSekiro>(PlayerController->GetCharacter());
	}
}

void UBossInfoWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (IsValid(Player) && IsValid(Player->TargetBoss))
	{
		if (IsValid(HPWidget) && IsValid(BossPostureWidget) && IsValid(BossName))
		{
			HPWidget->CharacterSetting(Player->TargetBoss);
			BossPostureWidget->CharacterSetting(Player->TargetBoss);
			BossName->SetText(FText::FromName(Player->TargetBoss->Tags[2]));

			// 체간 게이지는 Max가 아닐 경우에만 Visible
			if (Player->TargetBoss->GetPosture() < Player->TargetBoss->GetMaxPosture())
			{
				BossPostureWidget->SetVisibility(ESlateVisibility::Visible);
			}
			else
			{
				FTimerHandle myTimerHandle;
				float DelayTime = 0.5f;
				GetWorld()->GetTimerManager().SetTimer(myTimerHandle, FTimerDelegate::CreateLambda([&]()
					{
						BossPostureWidget->SetVisibility(ESlateVisibility::Hidden);

						GetWorld()->GetTimerManager().ClearTimer(myTimerHandle);
					}), DelayTime, false);
			}
		}

		int TargetMaxDeathblowCount = Player->TargetBoss->GetMaxDeathblowCount();
		int TargetDeathblowCount = Player->TargetBoss->GetDeathblowCount();

		// 인살 구슬 갱신
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
	}
	else
	{
		HPWidget->CharacterSetting(nullptr);
		BossPostureWidget->CharacterSetting(nullptr);
		BossName->SetText(FText());
	}
}

void UBossInfoWidget::UpdateDeathblowUI(int NewDeathblowCount, float Delta)
{
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