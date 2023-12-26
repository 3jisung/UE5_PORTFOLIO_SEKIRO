// Fill out your copyright notice in the Description page of Project Settings.


#include "StabType.h"


UStabType::UStabType()
{
	HPDamageMultiple = 2.0f;
	PostureDamageMultiple = 2.0f;
	PushPower = 5000.0f;
	bEnableGuard = false;
	bEnableParrying = true;
	bEnableMikiri = true;
	bEnableJumpEvasion = false;
	bEnableLightningReversal = false;
}