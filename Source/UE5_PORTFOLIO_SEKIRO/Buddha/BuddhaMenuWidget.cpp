// Fill out your copyright notice in the Description page of Project Settings.


#include "BuddhaMenuWidget.h"
#include "Player/PlayerSekiro.h"
#include "GameMode/GlobalGameMode.h"
#include "BuddhaBossFightWidget.h"


void UBuddhaMenuWidget::NativeConstruct()
{
	BtnHoveredImage.Add(Cast<UImage>(GetWidgetFromName(TEXT("Hovered_Rest"))));
	BtnHoveredImage.Add(Cast<UImage>(GetWidgetFromName(TEXT("Hovered_Fight"))));
	
	Super::NativeConstruct();

	MapNameText = Cast<UTextBlock>(GetWidgetFromName(TEXT("MapName")));
	MapNameText->SetText(FText());

	AGlobalGameMode* GameMode = Cast<AGlobalGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	SetMapName(FText::FromName(GameMode->GetMapName()));

	ExplainText = Cast<UTextBlock>(GetWidgetFromName(TEXT("MenuExplain")));
	ExplainText->SetText(FText());

	ExplainArray.Add(FText::FromString(TEXT("귀불에서 휴식합니다")));
	ExplainArray.Add(FText::FromString(TEXT("희귀한 강자와 싸우러 이동합니다")));

	HPWidget = Cast<UHPWidget>(GetWidgetFromName(TEXT("WBP_HPWidget")));
	PlayerVitality = Cast<UTextBlock>(GetWidgetFromName(TEXT("VitalityValue")));
	PlayerAttackPower = Cast<UTextBlock>(GetWidgetFromName(TEXT("AttackPowerValue")));

	if (PlayerController)
	{
		Player = Cast<APlayerSekiro>(PlayerController->GetCharacter());

		PlayerController->SetInputMode(FInputModeUIOnly());
		PlayerController->SetShowMouseCursor(true);

		if (Player && IsValid(HPWidget) && IsValid(PlayerVitality) && IsValid(PlayerAttackPower))
		{
			HPWidget->CharacterSetting(Player);
			PlayerVitality->SetText(FText::FromString(FString::SanitizeFloat(Player->GetMaxHP())));
			PlayerAttackPower->SetText(FText::FromString(FString::SanitizeFloat(Player->GetPower())));
		}
	}

	FadeInDeltaTime = 0.01f;
	FadeOutDeltaTime = 0.01f;
	FadeInDeltaOpacity = 0.1f;
	FadeOutDeltaOpacity = 0.1f;

	FadeIn();

	if (ExplainArray.IsValidIndex(0))
	{
		ExplainText->SetText(ExplainArray[0]);
	}
}

void UBuddhaMenuWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// UI 이벤트 도중 피격 당할 시 UI 강제 종료
	if (Player && PlayerController)
	{
		SekiroState AniStateValue = Player->GetAniState<SekiroState>();
		if (AniStateValue != SekiroState::SitStart && AniStateValue != SekiroState::SitEnd)
		{
			PlayerController->SetInputMode(FInputModeGameOnly());
			PlayerController->SetShowMouseCursor(false);

			if (bDestructWidget == false)
			{
				FadeOut(true);
			}
		}
	}
}

bool UBuddhaMenuWidget::SetHoveredBtn(int _Index)
{
	if (Super::SetHoveredBtn(_Index) && ExplainArray.IsValidIndex(_Index))
	{
		ExplainText->SetText(ExplainArray[_Index]);

		return true;
	}
	else
	{
		return false;
	}
}

void UBuddhaMenuWidget::MenuEvent()
{
	if (IsValid(Player) == false)
	{
		return;
	}
	else if (IsValid(Player->Controller) == false)
	{
		return;
	}
	
	switch (HoveredIndex)
	{
	case 0:
	{
		PlayerController->SetInputMode(FInputModeGameOnly());
		PlayerController->SetShowMouseCursor(false);

		FadeOut();

		Player->BuddhaRest();

		FTimerHandle myTimerHandle;
		float DelayTime = 3.f;
		GetWorld()->GetTimerManager().SetTimer(myTimerHandle, FTimerDelegate::CreateLambda([&]()
			{
				FadeIn();

				PlayerController->SetInputMode(FInputModeUIOnly());
				PlayerController->SetShowMouseCursor(true);
				
				SetFocus();

				GetWorld()->GetTimerManager().ClearTimer(myTimerHandle);
			}), DelayTime, false);

		break;
	}

	case 1:
	{
		Super::MenuEvent();
		
		PlayerController->SetInputMode(FInputModeGameOnly());
		FadeOut();

		FTimerHandle myTimerHandle;
		float DelayTime = 0.1f;
		GetWorld()->GetTimerManager().SetTimer(myTimerHandle, FTimerDelegate::CreateLambda([&]()
			{
				UGlobalGameInstance* Inst = GetGameInstance<UGlobalGameInstance>();
				TSubclassOf<UUserWidget> WidgetClass = Inst->GetWidgetClassData(TEXT("Buddha"), TEXT("BossFight"));

				if (IsValid(WidgetClass))
				{
					UBuddhaBossFightWidget* BuddhaFightBossWidget = Cast<UBuddhaBossFightWidget>(CreateWidget(GetWorld(), WidgetClass));

					if (IsValid(BuddhaFightBossWidget))
					{
						BuddhaFightBossWidget->SetParentWidget(this);
						BuddhaFightBossWidget->AddToViewport();
					}
				}

				GetWorld()->GetTimerManager().ClearTimer(myTimerHandle);
			}), DelayTime, false);
		
		break;
	}	

	default:
		break;
	}
}

void UBuddhaMenuWidget::ExitWidget()
{
	if (Player && PlayerController)
	{
		Player->SetAniState(SekiroState::SitEnd);

		PlayerController->SetInputMode(FInputModeGameOnly());
		PlayerController->SetShowMouseCursor(false);

		FadeOut(true);
	}
}