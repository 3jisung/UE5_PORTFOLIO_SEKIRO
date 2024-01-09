// Fill out your copyright notice in the Description page of Project Settings.


#include "ElectricSlashType.h"


UElectricSlashType::UElectricSlashType()
{
	HPDamageMultiple = 3.f;
	PostureDamageMultiple = 3.f;
	PushPower = 0.f;
	bEnableGuard = false;
	bEnableParrying = false;
	bEnableMikiri = false;
	bEnableJumpEvasion = false;
	bEnableLightningReversal = true;
}