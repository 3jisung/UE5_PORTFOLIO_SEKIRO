// Fill out your copyright notice in the Description page of Project Settings.


#include "Title_GameMode.h"


void ATitle_GameMode::BeginPlay()
{
	// 맵 사운드 목록 갱신
	UGlobalGameInstance* Inst = GetGameInstance<UGlobalGameInstance>();
	USoundBase* MapSound = Inst->GetSoundData(TEXT("Title"), TEXT("BGM"));

	if (IsValid(MapSound))
	{
		AllSound.Add(MapSound);
	}

	Super::BeginPlay();
}
