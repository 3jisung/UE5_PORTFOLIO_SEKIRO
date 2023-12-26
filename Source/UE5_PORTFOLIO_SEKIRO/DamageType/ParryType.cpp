// Fill out your copyright notice in the Description page of Project Settings.


#include "ParryType.h"


UParryType::UParryType()
{
	HPDamageMultiple = 0.0f;
	PostureDamageMultiple = 1.0f;
	PushPower = 0.0f;
	bEnableGuard = false;
	bEnableParrying = false;
	bEnableMikiri = false;
	bEnableJumpEvasion = false;
	bEnableLightningReversal = false;
}