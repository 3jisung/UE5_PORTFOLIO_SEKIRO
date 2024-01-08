// Fill out your copyright notice in the Description page of Project Settings.


#include "Stage2_GameMode.h"
#include "../Global/GlobalGameInstance.h"


AStage2_GameMode::AStage2_GameMode()
{
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
}

void AStage2_GameMode::BeginPlay()
{
	Super::BeginPlay();
	
	// 맵 사운드 목록 갱신
	UGlobalGameInstance* Inst = GetGameInstance<UGlobalGameInstance>();
	AllSound = Inst->GetSoundData(TEXT("Genichiro"));

	if (AllSound.Num() <= 0)
	{
		return;
	}

	AudioComponent->SetSound(AllSound[0]);
	AudioComponent->Play();
	AudioComponent->FadeIn(FadeInOutDuration);
}

void AStage2_GameMode::Tick(float _Delta)
{
	Super::Tick(_Delta);

}

void AStage2_GameMode::ModifySound(int index, float ChangeDuration)
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