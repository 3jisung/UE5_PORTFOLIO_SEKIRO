// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicAttackType.h"


UBasicAttackType::UBasicAttackType()
{
	HPDamageMultiple = 1.f;
	PostureDamageMultiple = 1.f;
	PushPower = 1500.f;
	bEnableGuard = true;
	bEnableParrying = true;
	bEnableMikiri = false;
	bEnableJumpEvasion = false;
	bEnableLightningReversal = false;
}