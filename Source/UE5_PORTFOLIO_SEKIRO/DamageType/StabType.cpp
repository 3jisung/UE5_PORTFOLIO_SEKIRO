// Fill out your copyright notice in the Description page of Project Settings.


#include "StabType.h"


UStabType::UStabType()
{
	HPDamageMultiple = 2.f;
	PostureDamageMultiple = 2.f;
	PushPower = 5000.f;
	bEnableGuard = false;
	bEnableParrying = true;
	bEnableMikiri = true;
	bEnableJumpEvasion = false;
	bEnableLightningReversal = false;
}