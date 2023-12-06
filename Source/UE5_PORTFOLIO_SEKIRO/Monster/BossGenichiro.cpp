// Fill out your copyright notice in the Description page of Project Settings.


#include "BossGenichiro.h"


ABossGenichiro::ABossGenichiro()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	Tags.Add(TEXT("아시나 겐이치로"));
	
	// 추후 몬스터 데이터로 추가
}