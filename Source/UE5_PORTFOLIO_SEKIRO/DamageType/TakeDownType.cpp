// Fill out your copyright notice in the Description page of Project Settings.


#include "TakeDownType.h"


UTakeDownType::UTakeDownType()
{
	HPDamageMultiple = 2.f;
	PostureDamageMultiple = 2.f;
	PushPower = 3500.f;
	bEnableGuard = true;
	bEnableParrying = true;
	bEnableMikiri = false;
	bEnableJumpEvasion = false;
	bEnableLightningReversal = false;
}