// Fill out your copyright notice in the Description page of Project Settings.


#include "TakeDownType.h"


UTakeDownType::UTakeDownType()
{
	HPDamageMultiple = 2.0f;
	PostureDamageMultiple = 2.0f;
	PushPower = 3500.0f;
	bEnableGuard = true;
	bEnableParrying = true;
	bEnableMikiri = false;
	bEnableJumpEvasion = false;
	bEnableLightningReversal = false;
}