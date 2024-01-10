// Fill out your copyright notice in the Description page of Project Settings.


#include "BossInfoWidget.h"


void UBossInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HPWidget = Cast<UHPWidget>(GetWidgetFromName(TEXT("WBP_HPWidget")));
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
}

void UBossInfoWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (Player->TargetBoss != nullptr)
	{
		HPWidget->CharacterSetting(Player->TargetBoss);
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
	}
	else
	{
		HPWidget->CharacterSetting(nullptr);
		BossName->SetText(FText());
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