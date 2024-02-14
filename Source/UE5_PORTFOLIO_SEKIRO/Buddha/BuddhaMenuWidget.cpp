// Fill out your copyright notice in the Description page of Project Settings.


#include "BuddhaMenuWidget.h"
#include "Player/PlayerSekiro.h"
#include "GameMode/GlobalGameMode.h"
#include "BuddhaBossFightWidget.h"
#include "GlobalUI/OKCancelWidget.h"


void UBuddhaMenuWidget::NativeConstruct()
{
	UImage* Hover1 = Cast<UImage>(GetWidgetFromName(TEXT("Hovered_Rest")));
	if (IsValid(Hover1))
	{
		BtnHoveredImage.Add(Hover1);
	}

	UImage* Hover2 = Cast<UImage>(GetWidgetFromName(TEXT("Hovered_Fight")));
	if (IsValid(Hover2))
	{
		BtnHoveredImage.Add(Hover2);
	}

	UImage* Hover3 = Cast<UImage>(GetWidgetFromName(TEXT("Hovered_Title")));
	if (IsValid(Hover3))
	{
		BtnHoveredImage.Add(Hover3);
	}
	
	Super::NativeConstruct();

	MapNameText = Cast<UTextBlock>(GetWidgetFromName(TEXT("MapName")));
	MapNameText->SetText(FText());

	AGlobalGameMode* GameMode = Cast<AGlobalGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (IsValid(GameMode))
	{
		SetMapName(FText::FromName(GameMode->GetMapName()));
	}

	ExplainText = Cast<UTextBlock>(GetWidgetFromName(TEXT("MenuExplain")));
	ExplainText->SetText(FText());

	ExplainArray.Add(FText::FromString(TEXT("귀불에서 휴식합니다")));
	ExplainArray.Add(FText::FromString(TEXT("희귀한 강자와 싸우러 이동합니다")));
	ExplainArray.Add(FText::FromString(TEXT("타이틀 화면으로 이동합니다")));

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
		if (AniStateValue != SekiroState::SitStart && AniStateValue != SekiroState::SitLoop && AniStateValue != SekiroState::SitEnd)
		{
			PlayerController->SetInputMode(FInputModeGameOnly());
			PlayerController->SetShowMouseCursor(false);

			if (Player->GetbLockOn())
			{
				Player->ToggleLockOn();
			}

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

		FTimerHandle myTimerHandle1;
		float DelayTime = 0.5f;
		GetWorld()->GetTimerManager().SetTimer(myTimerHandle1, FTimerDelegate::CreateLambda([&]()
			{
				Player->BuddhaRest();

				FTimerHandle myTimerHandle2;
				float DelayTime = 3.f;
				GetWorld()->GetTimerManager().SetTimer(myTimerHandle2, FTimerDelegate::CreateLambda([&]()
					{
						SekiroState AniStateValue = Player->GetAniState<SekiroState>();
						if (AniStateValue == SekiroState::SitStart || AniStateValue == SekiroState::SitLoop || AniStateValue == SekiroState::SitEnd)
						{
							FadeIn();

							PlayerController->SetInputMode(FInputModeUIOnly());
							PlayerController->SetShowMouseCursor(true);

							SetFocus();
						}

						GetWorld()->GetTimerManager().ClearTimer(myTimerHandle2);
					}), DelayTime, false);

				GetWorld()->GetTimerManager().ClearTimer(myTimerHandle1);
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
	case 2:
	{
		Super::MenuEvent();

		UGlobalGameInstance* Inst = GetGameInstance<UGlobalGameInstance>();
		TSubclassOf<UUserWidget> WidgetClass = Inst->GetWidgetClassData(TEXT("Global"), TEXT("OKCancel"));

		if (IsValid(WidgetClass))
		{
			UOKCancelWidget* OKCancelWidget = Cast<UOKCancelWidget>(CreateWidget(GetWorld(), WidgetClass));

			if (IsValid(OKCancelWidget))
			{
				OKCancelWidget->SetParentWidget(this);
				OKCancelWidget->AddToViewport();
				OKCancelWidget->SetNoticeText(FText::FromString(TEXT("타이틀 화면으로 이동하시겠습니까?")));
			}
		}
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

void UBuddhaMenuWidget::PopupWidgetReturn(int _PopupIndex)
{
	AGlobalGameMode* GameMode = Cast<AGlobalGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (IsValid(GameMode))
	{
		GameMode->StopSound();
	}

	UGlobalGameInstance* Inst = GetGameInstance<UGlobalGameInstance>();
	TSubclassOf<UUserWidget> WidgetClass = Inst->GetWidgetClassData(TEXT("Global"), TEXT("SceneTransition"));
	if (IsValid(WidgetClass))
	{
		UFadeInOutWidget* SceneTransitionWidget = Cast<UFadeInOutWidget>(CreateWidget(GetWorld(), WidgetClass));

		if (IsValid(SceneTransitionWidget))
		{
			SceneTransitionWidget->AddToViewport();

			SceneTransitionWidget->FadeIn();

			FTimerHandle myTimerHandle;
			float DelayTime = 1.f;
			GetWorld()->GetTimerManager().SetTimer(myTimerHandle, FTimerDelegate::CreateLambda([&]()
				{
					UGameplayStatics::OpenLevel(GetWorld(), TEXT("Title"));

					GetWorld()->GetTimerManager().ClearTimer(myTimerHandle);
				}), DelayTime, false);
		}
	}
}