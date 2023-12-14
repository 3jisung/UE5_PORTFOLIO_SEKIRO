// Fill out your copyright notice in the Description page of Project Settings.


#include "BossGenichiro.h"
#include "../../Global/GlobalAnimInstance.h"
#include "../../Global/GlobalGameInstance.h"
#include "../../Player/PlayerSekiro.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"


ABossGenichiro::ABossGenichiro()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ABossGenichiro::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add(TEXT("아시나 겐이치로"));

	UGlobalGameInstance* Inst = GetGameInstance<UGlobalGameInstance>();

	// 겐이치로 몬스터 값 설정
	MonsterData = Inst->GetMonster(TEXT("Genichiro"));
	MaxHP = MonsterData->MaxHP;
	HP = MaxHP;
	MaxPosture = MonsterData->MaxPosture;
	Posture = MaxPosture;
	Power = MonsterData->Power;
	DeathblowCount = MonsterData->DeathblowCount;

	// 비헤이비어 트리 설정(현재는 블루프린트 클래스에 미리 넣는 구조)
	BehaviorTree = MonsterData->AI;

	// 애니메이션 설정
	SetAllAnimation(MonsterData->Animations);
	GetGlobalAnimInstance()->AllAnimations = AllAnimations;
	SetAniState(GenichiroState::Idle);

	// 항상 플레이어 락온 상태
	GetCharacterMovement()->bOrientRotationToMovement = false;

	GetBlackboardComponent()->SetValueAsEnum(TEXT("GenichiroState"), static_cast<uint8>(GenichiroState::Idle));
	GetBlackboardComponent()->SetValueAsString(TEXT("TargetTag"), TEXT("Player"));
	GetBlackboardComponent()->SetValueAsFloat(TEXT("SearchRange"), 10000.0f);
	GetBlackboardComponent()->SetValueAsFloat(TEXT("AttackRange"), 200.0f);
}

void ABossGenichiro::AttackMove()
{
	float AttackMoveImpulse = 0.f;

	GenichiroState AniStateValue = GetAniState<GenichiroState>();

	if (AniStateValue == GenichiroState::BasicAttack1 || AniStateValue == GenichiroState::BasicAttack2
		|| AniStateValue == GenichiroState::BasicAttack3)
	{
		AttackMoveImpulse = 3000.0f;
	}
	else if (AniStateValue == GenichiroState::StabAttack)
	{
		AttackMoveImpulse = 5000.0f;
	}

	GetCharacterMovement()->AddImpulse(GetActorForwardVector() * AttackMoveImpulse, true);
}

void ABossGenichiro::Damage()
{
	if (bCollisionActor == false)
	{
		return;
	}

	GenichiroState AniStateValue = GetAniState<GenichiroState>();

	float AttackDamage = 0.f;

	if (AniStateValue == GenichiroState::BasicAttack1 || AniStateValue == GenichiroState::BasicAttack2
		|| AniStateValue == GenichiroState::BasicAttack3)
	{
		AttackDamage = Power;
	}
	else if (AniStateValue == GenichiroState::StabAttack || AniStateValue == GenichiroState::TakeDownAttack
		|| AniStateValue == GenichiroState::BottomAttack)
	{
		AttackDamage = Power * 2;
	}
	else if (AniStateValue == GenichiroState::ElectricSlash2)
	{
		AttackDamage = Power * 3;
	}
	else
	{
		AttackDamage = 0.f;
	}

	UObject* TargetObject = GetBlackboardComponent()->GetValueAsObject(TEXT("TargetActor"));
	APlayerSekiro* TargetActor = Cast<APlayerSekiro>(TargetObject);

	if (TargetActor == nullptr)
	{
		return;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("damage test"));
		//UCustomDamageType CustomDamageType;
		//UGameplayStatics::ApplyDamage(TargetActor, AttackDamage, GetController(), this, a);
	}
}