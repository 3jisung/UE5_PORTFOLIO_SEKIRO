// Fill out your copyright notice in the Description page of Project Settings.


#include "Stage2_GameMode.h"


void AStage2_GameMode::BeginPlay()
{
	// 맵 사운드 목록 갱신
	UGlobalGameInstance* Inst = GetGameInstance<UGlobalGameInstance>();
	AllSound = Inst->GetSoundData(TEXT("Stage2"));

	Super::BeginPlay();
}
