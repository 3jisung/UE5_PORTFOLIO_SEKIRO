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

	StabAttack1 UMETA(DisplayName = "찌르기1"),
	StabAttack2 UMETA(DisplayName = "찌르기2"),
	JumpAttack UMETA(DisplayName = "점프공격"),
	DashAttack UMETA(DisplayName = "대쉬공격"),
	
	LightningReversal1 UMETA(DisplayName = "뇌반1"),
	LightningReversal2 UMETA(DisplayName = "뇌반2"),

	Guard UMETA(DisplayName = "가드"),
	Parrying1 UMETA(DisplayName = "패링1"),
	Parrying2 UMETA(DisplayName = "패링2"),
	Blocked UMETA(DisplayName = "경직"),
	MikiriCounter UMETA(DisplayName = "간파하기"),
	ReadyToDeathblowMikiri UMETA(DisplayName = "간파인살준비"),

	Hit UMETA(DisplayName = "히트"),
	Exhaust UMETA(DisplayName = "탈진"),
	Knockdown UMETA(DisplayName = "넉다운"),
	Getup UMETA(DisplayName = "기상"),
	Shock UMETA(DisplayName = "타뢰"),
	Heal UMETA(DisplayName = "회복"),
	Death UMETA(DisplayName = "죽음"),

	JumpStart UMETA(DisplayName = "점프시작"),
	JumpLoop UMETA(DisplayName = "점프중"),
	JumpEnd UMETA(DisplayName = "점프종료"),
	Trample UMETA(DisplayName = "밟기"),

	SitStart UMETA(DisplayName = "정좌시작"),
	SitEnd UMETA(DisplayName = "정좌끝"),

	DeathblowNormal UMETA(DisplayName = "일반인살"),
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


// 겐이치로 애니메이션 상태값
UENUM(BlueprintType)
enum class GenichiroState : uint8
{
	None UMETA(DisplayName = "사용안함"),

	Idle UMETA(DisplayName = "서있기"),
	LeftWalk UMETA(DisplayName = "왼쪽걷기"),
	RightWalk UMETA(DisplayName = "오른쪽걷기"),
	ForwardRun UMETA(DisplayName = "앞으로뛰기"),

	BasicAttack1 UMETA(DisplayName = "평타1"),
	BasicAttack2 UMETA(DisplayName = "평타2"),
	BasicAttack3 UMETA(DisplayName = "평타3"),

	StabAttack UMETA(DisplayName = "찌르기"),
	TakeDownAttack UMETA(DisplayName = "내려찍기"),
	BottomAttack UMETA(DisplayName = "하단베기"),

	ElectricSlash1 UMETA(DisplayName = "뇌격1"),
	ElectricSlash2 UMETA(DisplayName = "뇌격2"),

	Guard UMETA(DisplayName = "가드"),
	Parrying1 UMETA(DisplayName = "패링1"),
	Parrying2 UMETA(DisplayName = "패링2"),
	Blocked UMETA(DisplayName = "경직"),

	Hit UMETA(DisplayName = "히트"),
	ExhaustStart UMETA(DisplayName = "탈진시작"),
	ExhaustLoop UMETA(DisplayName = "탈진중"),
	ExhaustEnd UMETA(DisplayName = "탈진종료"),
	Shock UMETA(DisplayName = "타뢰"),
	Death UMETA(DisplayName = "죽음"),

	Phase2Intro1 UMETA(DisplayName = "2페이즈_인트로1"),
	Phase2Intro2 UMETA(DisplayName = "2페이즈_인트로2"),

	MikiriCounter1 UMETA(DisplayName = "간파1"),
	MikiriCounter2 UMETA(DisplayName = "간파2"),

	Deathblow1 UMETA(DisplayName = "인살1"),
	Deathblow2 UMETA(DisplayName = "인살2"),
	Deathblow3 UMETA(DisplayName = "인살3"),
};

// 몬스터 피격 상태값
UENUM(BlueprintType)
enum class MonsterHitState : uint8
{
	NONE UMETA(DisplayName = "사용안함"),
	OFFGUARD UMETA(DisplayName = "무방비"),
	GUARD UMETA(DisplayName = "가드"),
	PARRYING UMETA(DisplayName = "패링"),
	SUPERARMOR UMETA(DisplayName = "슈퍼아머"),
	INVINCIBLE UMETA(DisplayName = "무적"),
};