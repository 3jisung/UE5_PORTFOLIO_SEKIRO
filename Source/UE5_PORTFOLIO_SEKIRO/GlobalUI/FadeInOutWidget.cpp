// Fill out your copyright notice in the Description page of Project Settings.


#include "FadeInOutWidget.h"


void UFadeInOutWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Canvas = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("CanvasPanel")));
	SetCanvasRenderOpacity(0.f);

	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (Player)
	{
		PlayerController = Cast<APlayerController>(Player->Controller);
	}
}

void UFadeInOutWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	// FadeIn
	if (bFadeIn)
	{
		SumTime += InDeltaTime;

		if (SumTime > FadeInDeltaTime)
		{
			Canvas->SetRenderOpacity(CanvasOpacity + FadeInDeltaOpacity);
			CanvasOpacity = Canvas->GetRenderOpacity();
			SumTime = 0.f;

			if (CanvasOpacity >= 1.f)
			{
				CanvasOpacity = 1.f;
				bFadeIn = false;

				FTimerHandle myTimerHandle;
				GetWorld()->GetTimerManager().SetTimer(myTimerHandle, FTimerDelegate::CreateLambda([&]()
					{
						FadeOut();

						GetWorld()->GetTimerManager().ClearTimer(myTimerHandle);
					}), TimerDelayTime, false);
			}
		}
	}

	// FadeOut
	if (bFadeOut)
	{
		SumTime += InDeltaTime;

		if (SumTime > FadeOutDeltaTime)
		{
			Canvas->SetRenderOpacity(CanvasOpacity - FadeOutDeltaOpacity);
			CanvasOpacity = Canvas->GetRenderOpacity();
			SumTime = 0.f;

			if (CanvasOpacity <= 0.f)
			{
				CanvasOpacity = 0.f;
				bFadeOut = false;

				if (bDestructWidget)
				{
					RemoveFromParent();
				}
			}
		}
	}
}