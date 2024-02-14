// Fill out your copyright notice in the Description page of Project Settings.


#include "BuddhaBossFightWidget.h"
#include "GlobalUI/OKCancelWidget.h"
#include "Player/PlayerSekiro.h"
#include "GameMode/GlobalGameMode.h"


void UBuddhaBossFightWidget::NativeConstruct()
{
	UImage* Hover1 = Cast<UImage>(GetWidgetFromName(TEXT("Hovered_Boss1")));
	if (IsValid(Hover1))
	{
		BtnHoveredImage.Add(Hover1);
	}

	UImage* Hover2 = Cast<UImage>(GetWidgetFromName(TEXT("Hovered_Boss2")));
	if (IsValid(Hover2))
	{
		BtnHoveredImage.Add(Hover2);
	}

	Super::NativeConstruct();

	BossNameText = Cast<UTextBlock>(GetWidgetFromName(TEXT("HoveredBossName")));
	BossNameText->SetText(FText());

	MapNameText = Cast<UTextBlock>(GetWidgetFromName(TEXT("MapName")));
	MapNameText->SetText(FText());

	BossThumbnailImage = Cast<UImage>(GetWidgetFromName(TEXT("BossThumbnail")));
	BossThumbnailImage->SetBrushResourceObject(nullptr);

	UTextBlock* Name1 = Cast<UTextBlock>(GetWidgetFromName(TEXT("BossName1")));
	if (IsValid(Name1))
	{
		BossNameArray.Add(Name1->GetText());
	}

	UTextBlock* Name2 = Cast<UTextBlock>(GetWidgetFromName(TEXT("BossName2")));
	if (IsValid(Name2))
	{
		BossNameArray.Add(Name2->GetText());
	}

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

	UGlobalGameInstance* Inst = GetGameInstance<UGlobalGameInstance>();

	if (BossLevelArray.IsValidIndex(HoveredIndex) && BossNameArray.IsValidIndex(HoveredIndex))
	{
		TSubclassOf<UUserWidget> WidgetClass = Inst->GetWidgetClassData(TEXT("Global"), TEXT("OKCancel"));

		if (IsValid(WidgetClass))
		{
			UOKCancelWidget* OKCancelWidget = Cast<UOKCancelWidget>(CreateWidget(GetWorld(), WidgetClass));

			if (IsValid(OKCancelWidget))
			{
				OKCancelWidget->SetParentWidget(this);
				OKCancelWidget->AddToViewport();
				OKCancelWidget->SetNoticeText(FText::FromString(BossNameArray[HoveredIndex].ToString() + TEXT("(와)과의 싸움을 시작하시겠습니까?")));
			}
		}
	}
	else
	{
		TSubclassOf<UUserWidget> WidgetClass = Inst->GetWidgetClassData(TEXT("Global"), TEXT("OK"));
		
		if (IsValid(WidgetClass))
		{
			UOKCancelWidget* OKWidget = Cast<UOKCancelWidget>(CreateWidget(GetWorld(), WidgetClass));

			if (IsValid(OKWidget))
			{
				OKWidget->SetParentWidget(this);
				OKWidget->AddToViewport();
				OKWidget->SetNoticeText(FText::FromString(TEXT("미구현 보스입니다")));
			}
		}
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

void UBuddhaBossFightWidget::PopupWidgetReturn(int _PopupIndex)
{
	// 보스 맵 이동
	if (_PopupIndex == 0)
	{
		if (IsValid(ParentWidget))
		{
			Player->SetHitState(PlayerHitState::INVINCIBLE);
			
			ParentWidget->FadeOut(true);
			FadeOut(true);

			FTimerHandle myTimerHandle1;
			float DelayTime = 2.f;
			GetWorld()->GetTimerManager().SetTimer(myTimerHandle1, FTimerDelegate::CreateLambda([&]()
				{
					AGlobalGameMode* GameMode = Cast<AGlobalGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
					if (IsValid(GameMode))
					{
						GameMode->StopSound();
					}

					UGlobalGameInstance* Inst = GetGameInstance<UGlobalGameInstance>();
					USoundBase* TeleportSound = Inst->GetSoundData(TEXT("Global"), TEXT("Teleportation"));

					if (IsValid(TeleportSound))
					{
						UGameplayStatics::PlaySound2D(GetWorld(), TeleportSound);
					}

					TSubclassOf<UUserWidget> WidgetClass = Inst->GetWidgetClassData(TEXT("Global"), TEXT("SceneTransition"));

					if (IsValid(WidgetClass))
					{
						UFadeInOutWidget* SceneTransitionWidget = Cast<UFadeInOutWidget>(CreateWidget(GetWorld(), WidgetClass));

						if (IsValid(SceneTransitionWidget))
						{
							SceneTransitionWidget->AddToViewport();

							SceneTransitionWidget->FadeIn();

							FTimerHandle myTimerHandle2;
							float DelayTime = 2.f;
							GetWorld()->GetTimerManager().SetTimer(myTimerHandle2, FTimerDelegate::CreateLambda([&]()
								{
									// Player->SetHitState(PlayerHitState::OFFGUARD);
									
									if (BossLevelArray.IsValidIndex(HoveredIndex))
									{
										UGameplayStatics::OpenLevel(GetWorld(), BossLevelArray[HoveredIndex]);
									}

									GetWorld()->GetTimerManager().ClearTimer(myTimerHandle2);
								}), DelayTime, false);
						}
					}

					GetWorld()->GetTimerManager().ClearTimer(myTimerHandle1);
				}), DelayTime, false);
		}
	}
}