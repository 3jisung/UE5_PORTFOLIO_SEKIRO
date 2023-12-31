// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "CustomDamageTypeBase.generated.h"

/**
 * 
 */
UCLASS()
class UE5_PORTFOLIO_SEKIRO_API UCustomDamageTypeBase : public UDamageType
{
	GENERATED_BODY()
	
public:
	float HPDamageMultiple;
	float PostureDamageMultiple;
	float PushPower;
	bool bEnableGuard;
	bool bEnableParrying;
	bool bEnableMikiri;
	bool bEnableJumpEvasion;
	bool bEnableLightningReversal;
};
