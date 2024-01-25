// Fill out your copyright notice in the Description page of Project Settings.


#include "Stage1_GameMode.h"


void AStage1_GameMode::BeginPlay()
{
	MapName = FName(TEXT("아시나성 성하"));

	// 맵 사운드 목록 갱신
	UGlobalGameInstance* Inst = GetGameInstance<UGlobalGameInstance>();
	USoundBase* MapSound = Inst->GetSoundData(TEXT("Stage1"), TEXT("BGM"));

	if (IsValid(MapSound))
	{
		AllSound.Add(MapSound);
	}

	Super::BeginPlay();
}
