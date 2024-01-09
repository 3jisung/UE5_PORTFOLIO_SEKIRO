// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GlobalGameMode.h"
#include "Stage2_GameMode.generated.h"

/**
 * 
 */
UCLASS()
class UE5_PORTFOLIO_SEKIRO_API AStage2_GameMode : public AGlobalGameMode
{
	GENERATED_BODY()
	

protected:
	// Called when the game starts or when spawned
	void BeginPlay() override;

};
