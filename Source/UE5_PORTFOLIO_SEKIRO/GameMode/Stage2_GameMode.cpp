// Fill out your copyright notice in the Description page of Project Settings.


#include "Stage2_GameMode.h"


void AStage2_GameMode::BeginPlay()
{
	MapName = FName(TEXT("아시나성 본성"));
	
	// 맵 사운드 목록 갱신
	UGlobalGameInstance* Inst = GetGameInstance<UGlobalGameInstance>();
	USoundBase* Phase1 = Inst->GetSoundData(TEXT("Stage2"), TEXT("Phase1"));
	USoundBase* Phase2 = Inst->GetSoundData(TEXT("Stage2"), TEXT("Phase2"));
	USoundBase* MapSound = Inst->GetSoundData(TEXT("Stage2"), TEXT("BGM"));

	if (IsValid(Phase1) && IsValid(Phase2) && IsValid(MapSound))
	{
		AllSound.Add(Phase1);
		AllSound.Add(Phase2);
		AllSound.Add(MapSound);
	}

	Super::BeginPlay();
}
