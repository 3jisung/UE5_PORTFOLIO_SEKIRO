// Fill out your copyright notice in the Description page of Project Settings.


#include "GlobalGameMode.h"


AGlobalGameMode::AGlobalGameMode()
{
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
}

void AGlobalGameMode::BeginPlay()
{
	if (AllSound.IsValidIndex(0))
	{
		AudioComponent->SetSound(AllSound[0]);
	}

	Super::BeginPlay();
}

void AGlobalGameMode::Tick(float _Delta)
{
	Super::Tick(_Delta);

}

void AGlobalGameMode::ModifySound(int index, float ChangeDuration)
{
	if (AllSound.IsValidIndex(index))
	{
		NewSoundIndex = index;
		AudioComponent->FadeOut(FadeInOutDuration, 0.f);

		FTimerHandle myTimerHandle;

		if (ChangeDuration < FadeInOutDuration)
		{
			ChangeDuration = FadeInOutDuration;
		}

		float delayTime = ChangeDuration;
		GetWorld()->GetTimerManager().SetTimer(myTimerHandle, FTimerDelegate::CreateLambda([&]()
			{
				AudioComponent->SetSound(AllSound[NewSoundIndex]);
				AudioComponent->FadeIn(FadeInOutDuration);
				NewSoundIndex = -1;

				GetWorld()->GetTimerManager().ClearTimer(myTimerHandle);
			}), delayTime, false);
	}
}