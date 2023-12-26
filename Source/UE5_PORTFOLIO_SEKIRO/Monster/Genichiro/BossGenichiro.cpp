// Fill out your copyright notice in the Description page of Project Settings.


#include "BossGenichiro.h"
#include "../../Global/GlobalAnimInstance.h"
#include "../../Global/GlobalGameInstance.h"
#include "BehaviorTree/BlackboardComponent.h"


ABossGenichiro::ABossGenichiro()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ABossGenichiro::BeginPlay()
{
	Super::BeginPlay();

	// 애니메이션 종료 시 MontageEnd를 콜백한다.
	GetGlobalAnimInstance()->OnMontageBlendingOut.AddDynamic(this, &ABossGenichiro::MontageBlendingOut);

	Tags.Add(TEXT("보스"));
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
	GetBlackboardComponent()->SetValueAsFloat(TEXT("AttackRange"), 150.0f);
}

void ABossGenichiro::Tick(float _Delta)
{
	Super::Tick(_Delta);

	GenichiroState AniStateValue = GetAniState<GenichiroState>();

	if (HP > 70.0f)
	{
		PostureRecoveryAmount = MaxPostureRecoveryAmount;
	}
	else if (HP > 40.0f)
	{
		PostureRecoveryAmount = MaxPostureRecoveryAmount * 0.5;
	}
	else
	{
		PostureRecoveryAmount = MaxPostureRecoveryAmount * 0.25;
	}

	if (bEnablePostureRecovery)
	{
		// 기본 자세 or 걷는 도중엔 체간 회복
		if (AniStateValue == GenichiroState::Idle
			|| AniStateValue == GenichiroState::LeftWalk || AniStateValue == GenichiroState::RightWalk)
		{
			Posture += PostureRecoveryAmount;
		}

		if (Posture > 100)
		{
			Posture = 100.0f;
		}
	}
}

float ABossGenichiro::TakeDamage(float DamageAmount,
	struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator,
	AActor* DamageCauser)
{
	float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	UCustomDamageTypeBase* DamageType;

	// 데미지 타입 확인 및 특수 이벤트 처리(찌르기, 간파하기, 트렘플, 뇌반)
	if (DamageEvent.DamageTypeClass == UBasicAttackType::StaticClass())
	{
		DamageType = Cast<UBasicAttackType>(DamageEvent.DamageTypeClass->GetDefaultObject());
	}
	else if (DamageEvent.DamageTypeClass == UStabType::StaticClass())
	{
		DamageType = Cast<UStabType>(DamageEvent.DamageTypeClass->GetDefaultObject());

		if (HitState == MonsterHitState::GUARD)
		{
			HitState = MonsterHitState::PARRYING;
		}
	}
	else if (DamageEvent.DamageTypeClass == UMikiriType::StaticClass())
	{
		DamageType = Cast<UMikiriType>(DamageEvent.DamageTypeClass->GetDefaultObject());

		Posture -= DamageAmount * (DamageType->PostureDamageMultiple);

		if (Posture <= 0)
		{
			Posture = 0.1;
		}
		
		// 간파하기 피격 시 회전값 조정
		AdjustAngle(DamageCauser->GetActorLocation());
		SetAniState(GenichiroState::MikiriCounter1);

		return Damage;
	}
	else if (DamageEvent.DamageTypeClass == UTrampleType::StaticClass())
	{
		DamageType = Cast<UTrampleType>(DamageEvent.DamageTypeClass->GetDefaultObject());
	}
	else if (DamageEvent.DamageTypeClass == UParryType::StaticClass())
	{
		DamageType = Cast<UParryType>(DamageEvent.DamageTypeClass->GetDefaultObject());

		Posture -= DamageAmount * (DamageType->PostureDamageMultiple);

		GenichiroState AniStateValue = GetAniState<GenichiroState>();

		if (Posture <= 0)
		{
			ExhaustAction();
		}
		else if (AniStateValue == GenichiroState::BasicAttack3)
		{
			SetAniState(GenichiroState::Blocked);
		}

		return Damage;
	}
	else if (DamageEvent.DamageTypeClass == UElectricSlashType::StaticClass())
	{
		DamageType = Cast<UElectricSlashType>(DamageEvent.DamageTypeClass->GetDefaultObject());

		HP -= DamageAmount * (DamageType->HPDamageMultiple);
		Posture -= DamageAmount * (DamageType->PostureDamageMultiple);

		if (HP <= 0)
		{
			HP = 0.1f;
		}
		else if (Posture <= 0)
		{
			Posture = 0.1f;
		}

		SetAniState(GenichiroState::Shock);

		return Damage;
	}
	else if (DamageEvent.DamageTypeClass == UDeathblowType::StaticClass())
	{
		DamageType = Cast<UDeathblowType>(DamageEvent.DamageTypeClass->GetDefaultObject());

		GetHitImpulseManager(DamageCauser, 2000.0f);

		AdjustAngle(DamageCauser->GetActorLocation());

		SetAniState(GenichiroState::Deathblow1);

		return Damage;
	}
	else
	{
		return Damage;
	}

	if (HitState == MonsterHitState::INVINCIBLE)
	{
		return Damage;
	}
	// 공격 방향 체크
	else if (CalculateAngle(DamageCauser->GetActorLocation()) > 90.0f)
	{
		GetHitExecute(DamageAmount, DamageType, DamageCauser);

		return Damage;
	}
	else if (HitState == MonsterHitState::GUARD && DamageType->bEnableGuard)
	{
		Posture -= DamageAmount * (DamageType->PostureDamageMultiple);

		GetHitImpulseManager(DamageCauser, 1500.0f);

		GetWorld()->GetTimerManager().ClearTimer(PostureRecoveryManagerTimerHandle);
		bEnablePostureRecovery = false;
		GetWorld()->GetTimerManager().SetTimer(PostureRecoveryManagerTimerHandle, this, &AGlobalCharacter::PostureRecoveryManagerTimer, 0.5f, false);

		if (Posture <= 0)
		{
			ExhaustAction();
		}
		else
		{
			SetAniState(GenichiroState::Guard);
		}
	}
	else if (HitState == MonsterHitState::PARRYING && DamageType->bEnableParrying)
	{
		// 패링 성공 시 체간 데미지 25% 감소
		Posture -= (DamageAmount * 0.75) * (DamageType->PostureDamageMultiple);

		GetHitImpulseManager(DamageCauser, 1500.0f);

		GetWorld()->GetTimerManager().ClearTimer(PostureRecoveryManagerTimerHandle);
		bEnablePostureRecovery = false;
		GetWorld()->GetTimerManager().SetTimer(PostureRecoveryManagerTimerHandle, this, &AGlobalCharacter::PostureRecoveryManagerTimer, 0.5f, false);

		if (Posture <= 0)
		{
			Posture = 0.1f;
		}

		SetAniState((int)GenichiroState::Parrying1 + ParryingCount);

		++ParryingCount;
		if (ParryingCount > 1)
		{
			ParryingCount = 0;
		}

		TSubclassOf<UDamageType> HitDamageType = UParryType::StaticClass();
		UGameplayStatics::ApplyDamage(DamageCauser, this->Power, GetController(), this, HitDamageType);
	}
	else
	{
		GetHitExecute(DamageAmount, DamageType, DamageCauser);
	}

	return Damage;
}

void ABossGenichiro::GetHitExecute(float DamageAmount, UCustomDamageTypeBase* DamageType, AActor* DamageCauser)
{
	HP -= DamageAmount * (DamageType->HPDamageMultiple);
	Posture -= DamageAmount * (DamageType->PostureDamageMultiple);

	if (HP <= 0)
	{
		ExhaustAction();
	}
	else if (Posture <= 0)
	{
		ExhaustAction();
	}
	else
	{
		if (DamageCauser == nullptr)
		{
			return;
		}

		if (HitState == MonsterHitState::SUPERARMOR)
		{
			return;
		}

		GetHitImpulseManager(DamageCauser, 1500.0f);

		SetAniState(GenichiroState::Hit);
	}
}

void ABossGenichiro::ExhaustAction()
{
	Posture = 0;

	SetAniState(GenichiroState::ExhaustStart);
}

bool ABossGenichiro::GetHitCheck()
{
	GenichiroState AniStateValue = GetAniState<GenichiroState>();

	if (AniStateValue == GenichiroState::Hit || AniStateValue == GenichiroState::MikiriCounter1
		|| AniStateValue == GenichiroState::Shock || AniStateValue == GenichiroState::Deathblow1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool ABossGenichiro::BlockCheck()
{
	GenichiroState AniStateValue = GetAniState<GenichiroState>();

	if (AniStateValue == GenichiroState::Guard
		|| AniStateValue == GenichiroState::Parrying1 || AniStateValue == GenichiroState::Parrying2)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool ABossGenichiro::IsExhaust()
{
	GenichiroState AniStateValue = GetAniState<GenichiroState>();

	if (AniStateValue == GenichiroState::ExhaustStart)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool ABossGenichiro::IsDeath()
{
	GenichiroState AniStateValue = GetAniState<GenichiroState>();

	if (AniStateValue == GenichiroState::Death)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void ABossGenichiro::AttackMove()
{
	float AttackMoveImpulse = 0.f;

	GenichiroState AniStateValue = GetAniState<GenichiroState>();

	if (AniStateValue == GenichiroState::StabAttack)
	{
		AttackMoveImpulse = 6000.0f;
	}
	else
	{
		AttackMoveImpulse = 3000.0f;
	}

	GetCharacterMovement()->AddImpulse(GetActorForwardVector() * AttackMoveImpulse, true);
}

void ABossGenichiro::Damage()
{
	// 범위에 있는 Player 콜리전 탐색
	EObjectTypeQuery ObjectType = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel1);
	TArray<AActor*> HitActor = TraceObjects(ObjectType, GetActorForwardVector(), 45.0f, 50.0f, 70.0f);

	GenichiroState AniStateValue = GetAniState<GenichiroState>();
	TSubclassOf<UDamageType> DamageType;

	if (AniStateValue == GenichiroState::BasicAttack1 || AniStateValue == GenichiroState::BasicAttack2
		|| AniStateValue == GenichiroState::BasicAttack3)
	{
		DamageType = UBasicAttackType::StaticClass();
	}
	else if (AniStateValue == GenichiroState::StabAttack)
	{
		DamageType = UStabType::StaticClass();
	}
	else if (AniStateValue == GenichiroState::TakeDownAttack)
	{
		DamageType = UTakeDownType::StaticClass();
	}
	else if (AniStateValue == GenichiroState::BottomAttack)
	{
		DamageType = UBottomType::StaticClass();
	}
	else if (AniStateValue == GenichiroState::ElectricSlash2)
	{
		DamageType = UElectricSlashType::StaticClass();

		HitActor = TraceObjects(ObjectType, GetActorForwardVector(), 45.0f, 500.0f, 100.0f);
	}
	else
	{
		DamageType = UDamageType::StaticClass();
	}

	if (HitActor.Num() == 0)
	{
		return;
	}

	for (AActor* Target : HitActor)
	{
		UGameplayStatics::ApplyDamage(Target, this->Power, GetController(), this, DamageType);
	}
}

void ABossGenichiro::MontageBlendingOut(UAnimMontage* Anim, bool _Inter)
{
	
}