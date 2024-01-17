// Fill out your copyright notice in the Description page of Project Settings.


#include "BuddhaBossFightWidget.h"
#include "Player/PlayerSekiro.h"


void UBuddhaBossFightWidget::NativeConstruct()
{
	BtnHoveredImage.Add(Cast<UImage>(GetWidgetFromName(TEXT("Hovered_Boss1"))));
	BtnHoveredImage.Add(Cast<UImage>(GetWidgetFromName(TEXT("Hovered_Boss2"))));

	Super::NativeConstruct();

	BossNameText = Cast<UTextBlock>(GetWidgetFromName(TEXT("HoveredBossName")));
	BossNameText->SetText(FText());

	MapNameText = Cast<UTextBlock>(GetWidgetFromName(TEXT("MapName")));
	MapNameText->SetText(FText());

	BossThumbnailImage = Cast<UImage>(GetWidgetFromName(TEXT("BossThumbnail")));
	BossThumbnailImage->SetBrushResourceObject(nullptr);

	BossNameArray.Add(Cast<UTextBlock>(GetWidgetFromName(TEXT("BossName1")))->GetText());
	BossNameArray.Add(Cast<UTextBlock>(GetWidgetFromName(TEXT("BossName2")))->GetText());

	MapNameArray.Add(FText::FromString(TEXT("아시나성 본성")));
	MapNameArray.Add(FText::FromString(TEXT("기원의 궁")));

	BossLevelArray.Add(TEXT("Stage2"));

	UGlobalGameInstance* Inst = GetGameInstance<UGlobalGameInstance>();
	BossThumbnailArray = Inst->GetImageData(TEXT("BossThumbnail"));

	if (PlayerController)
	{
		Player = Cast<APlayerSekiro>(PlayerController->GetCharacter());
		
		PlayerController->SetInputMode(FInputModeUIOnly());
		PlayerController->SetShowMouseCursor(true);
	}

	FadeInDeltaTime = 0.01f;
	FadeOutDeltaTime = 0.01f;
	FadeInDeltaOpacity = 0.1f;
	FadeOutDeltaOpacity = 0.1f;

	FadeIn();

	if (BossNameArray.IsValidIndex(0) && MapNameArray.IsValidIndex(0) && BossThumbnailArray.IsValidIndex(0))
	{
		BossNameText->SetText(BossNameArray[0]);
		MapNameText->SetText(MapNameArray[0]);
		BossThumbnailImage->SetBrushResourceObject(BossThumbnailArray[0]);
	}
}

void UBuddhaBossFightWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
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

bool UBuddhaBossFightWidget::SetHoveredBtn(int _Index)
{
	if (Super::SetHoveredBtn(_Index))
	{
		if (BossNameArray.IsValidIndex(_Index) && MapNameArray.IsValidIndex(_Index) && BossThumbnailArray.IsValidIndex(_Index))
		{
			BossNameText->SetText(BossNameArray[_Index]);
			MapNameText->SetText(MapNameArray[_Index]);
			BossThumbnailImage->SetBrushResourceObject(BossThumbnailArray[_Index]);

			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

void UBuddhaBossFightWidget::MenuEvent()
{
	if (IsValid(Player) == false)
	{
		return;
	}
	else if (IsValid(Player->Controller) == false)
	{
		return;
	}

	Super::MenuEvent();

	if (BossLevelArray.IsValidIndex(HoveredIndex))
	{
		// 위젯 생성
	}
	else
	{
		// 위젯 생성
	}
}

void UBuddhaBossFightWidget::ExitWidget()
{
	FadeOut(true);

	FTimerHandle myTimerHandle;
	float DelayTime = 0.1f;
	GetWorld()->GetTimerManager().SetTimer(myTimerHandle, FTimerDelegate::CreateLambda([&]()
		{
			if (IsValid(ParentWidget))
			{
				ParentWidget->FadeIn();
				ParentWidget->SetFocus();
			}

			GetWorld()->GetTimerManager().ClearTimer(myTimerHandle);
		}), DelayTime, false);
}