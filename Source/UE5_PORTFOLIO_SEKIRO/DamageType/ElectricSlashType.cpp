// Fill out your copyright notice in the Description page of Project Settings.


#include "ElectricSlashType.h"


UElectricSlashType::UElectricSlashType()
{
	HPDamageMultiple = 3.0f;
	PostureDamageMultiple = 3.0f;
	PushPower = 0.0f;
	bEnableGuard = false;
	bEnableParrying = false;
	bEnableMikiri = false;
	bEnableJumpEvasion = false;
	bEnableLightningReversal = true;
}