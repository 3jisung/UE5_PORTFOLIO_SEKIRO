// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class GenichiroState : uint8
{
	NONE UMETA(DisplayName = "사용안함"),
	IDLE UMETA(DisplayName = "대기"),
	MOVE UMETA(DisplayName = "이동"),
	RETURN UMETA(DisplayName = "복귀"),
	PATROL UMETA(DisplayName = "순찰"),
	ATTACK UMETA(DisplayName = "공격"),
	DEATH UMETA(DisplayName = "죽음"),
};