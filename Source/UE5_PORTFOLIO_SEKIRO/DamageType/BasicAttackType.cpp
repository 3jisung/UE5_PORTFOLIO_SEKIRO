// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicAttackType.h"


UBasicAttackType::UBasicAttackType()
{
	HPDamageMultiple = 1.0f;
	PostureDamageMultiple = 1.0f;
	PushPower = 1500.0f;
	bEnableGuard = true;
	bEnableParrying = true;
	bEnableMikiri = false;
	bEnableJumpEvasion = false;
	bEnableLightningReversal = false;
}