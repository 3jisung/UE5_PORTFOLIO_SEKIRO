// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

// 세키로 애니메이션 상태값
UENUM(BlueprintType)
enum class SekiroState : uint8
{
	None UMETA(DisplayName = "사용안함"),
	Idle UMETA(DisplayName = "서있기"),

	LeftWalk UMETA(DisplayName = "왼쪽걷기"),
	RightWalk UMETA(DisplayName = "오른쪽걷기"),
	ForwardWalk UMETA(DisplayName = "앞으로걷기"),
	BackwardWalk UMETA(DisplayName = "뒤로걷기"),

	LeftRun UMETA(DisplayName = "왼쪽뛰기"),
	RightRun UMETA(DisplayName = "오른쪽뛰기"),
	ForwardRun UMETA(DisplayName = "앞으로뛰기"),
	BackwardRun UMETA(DisplayName = "뒤로뛰기"),

	BasicAttack1 UMETA(DisplayName = "평타1"),
	BasicAttack2 UMETA(DisplayName = "평타2"),
	BasicAttack3 UMETA(DisplayName = "평타3"),

	StabAttack UMETA(DisplayName = "찌르기"),
	JumpAttack UMETA(DisplayName = "점프공격"),
	DashAttack UMETA(DisplayName = "대쉬공격"),
	
	LightningReversal1 UMETA(DisplayName = "뇌반1"),
	LightningReversal2 UMETA(DisplayName = "뇌반2"),

	Guard UMETA(DisplayName = "가드"),
	Parrying1 UMETA(DisplayName = "패링1"),
	Parrying2 UMETA(DisplayName = "패링2"),
	MikiriCounter UMETA(DisplayName = "간파하기"),

	Hit UMETA(DisplayName = "히트"),
	Stun UMETA(DisplayName = "스턴"),
	Knockdown UMETA(DisplayName = "넉다운"),
	Getup UMETA(DisplayName = "기상"),
	Death UMETA(DisplayName = "죽음"),

	Jump UMETA(DisplayName = "점프"),
	Trample UMETA(DisplayName = "밟기"),

	Sit UMETA(DisplayName = "정좌"),
	DeathblowBoss UMETA(DisplayName = "보스인살"),
};

// 플레이어 피격 상태값
UENUM(BlueprintType)
enum class PlayerHitState : uint8
{
	NONE UMETA(DisplayName = "사용안함"),
	OFFGUARD UMETA(DisplayName = "무방비"),
	GUARD UMETA(DisplayName = "가드"),
	PARRYING UMETA(DisplayName = "패링"),
	INVINCIBLE UMETA(DisplayName = "무적"),
	DASHINVINCIBLE UMETA(DisplayName = "대쉬무적"),
};

UENUM(BlueprintType)
enum class MonsterState : uint8
{
	NONE UMETA(DisplayName = "사용안함"),
	IDLE UMETA(DisplayName = "대기"),
	MOVE UMETA(DisplayName = "이동"),
	RETURN UMETA(DisplayName = "복귀"),
	PATROL UMETA(DisplayName = "순찰"),
	ATTACK UMETA(DisplayName = "공격"),
	DEATH UMETA(DisplayName = "죽음"),
};