// Fill out your copyright notice in the Description page of Project Settings.


#include "ParryType.h"


UParryType::UParryType()
{
	HPDamageMultiple = 0.f;
	PostureDamageMultiple = 1.f;
	PushPower = 0.f;
	bEnableGuard = false;
	bEnableParrying = false;
	bEnableMikiri = false;
	bEnableJumpEvasion = false;
	bEnableLightningReversal = false;
}