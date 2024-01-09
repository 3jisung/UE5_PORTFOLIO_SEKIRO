// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GlobalGameMode.h"
#include "Title_GameMode.generated.h"

/**
 * 
 */
UCLASS()
class UE5_PORTFOLIO_SEKIRO_API ATitle_GameMode : public AGlobalGameMode
{
	GENERATED_BODY()
	

protected:
	// Called when the game starts or when spawned
	void BeginPlay() override;

};