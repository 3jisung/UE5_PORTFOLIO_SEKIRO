// Fill out your copyright notice in the Description page of Project Settings.


#include "BuddhaMenuWidget.h"
#include "Player/PlayerSekiro.h"


void UBuddhaMenuWidget::NativeConstruct()
{
	BtnHoveredImage.Add(Cast<UImage>(GetWidgetFromName(TEXT("Hovered_Rest"))));
	BtnHoveredImage.Add(Cast<UImage>(GetWidgetFromName(TEXT("Hovered_Fight"))));
	
	Super::NativeConstruct();

	MapNameText = Cast<UTextBlock>(GetWidgetFromName(TEXT("MapName")));
	MapNameText->SetText(FText());

	ExplainText = Cast<UTextBlock>(GetWidgetFromName(TEXT("MenuExplain")));
	ExplainText->SetText(FText());

	ExplainArray.Add(FText::FromString(TEXT("귀불에서 휴식합니다")));
	ExplainArray.Add(FText::FromString(TEXT("희귀한 강자와 싸우러 이동합니다")));

	Player = Cast<APlayerSekiro>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	FadeInDeltaTime = 0.01f;
	FadeOutDeltaTime = 0.01f;
	FadeInDeltaOpacity = 0.1f;
	FadeOutDeltaOpacity = 0.1f;

	FadeIn();

	if (ExplainArray.IsValidIndex(0))
	{
		BtnHoveredImage[0]->SetOpacity(1.f);
		ExplainText->SetText(ExplainArray[0]);
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
	if (Player == nullptr)
	{
		return;
	}
	else if (Player->Controller == nullptr)
	{
		return;
	}

	APlayerController* PlayerController = Cast<APlayerController>(Player->Controller);
	
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
		GetWorld()->GetTimerManager().SetTimer(myTimerHandle, FTimerDelegate::CreateLambda([&, PlayerController]()
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
		UGameplayStatics::PlaySound2D(GetWorld(), SoundEffects.FindRef(TEXT("Select")));
		
		PlayerController->SetInputMode(FInputModeGameOnly());
		FadeOut();

		FTimerHandle myTimerHandle;
		float DelayTime = 0.1f;
		GetWorld()->GetTimerManager().SetTimer(myTimerHandle, FTimerDelegate::CreateLambda([&]()
			{
				// 새로운 위젯 생성(부모 지정도 해줘야 할 것 같음)

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
	if (Player)
	{
		Player->SetAniState(SekiroState::SitEnd);

		if (Player->Controller)
		{
			APlayerController* PlayerController = Cast<APlayerController>(Player->Controller);
			PlayerController->SetInputMode(FInputModeGameOnly());
			PlayerController->SetShowMouseCursor(false);
		}

		FadeOut(true);
	}
}