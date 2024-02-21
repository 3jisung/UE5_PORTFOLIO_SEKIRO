// Fill out your copyright notice in the Description page of Project Settings.


#include "BossGenichiro.h"
#include "Global/GlobalAnimInstance.h"
#include "Global/GlobalGameInstance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/WidgetComponent.h"


ABossGenichiro::ABossGenichiro()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	WeaponEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("WeaponEffect"));
	WeaponEffect->SetRelativeLocation(FVector(0.f, -10.f, 0.f));
	WeaponEffect->SetRelativeRotation(FRotator(0.f, 0.f, 90.f));
	WeaponEffect->SetRelativeScale3D(FVector(0.4f, 0.4f, 0.45f));
	WeaponEffect->SetTemplate(nullptr);
	WeaponEffect->SetupAttachment(WeaponMesh);
}

void ABossGenichiro::BeginPlay()
{
	Super::BeginPlay();

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
	MaxDeathblowCount = MonsterData->MaxDeathblowCount;
	DeathblowCount = MaxDeathblowCount;

	MaxPostureRecoveryAmount = MaxPosture * 0.002f;

	ShockWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 160.f));

	// 비헤이비어 트리 설정(현재는 블루프린트 클래스에 미리 넣는 구조)
	BehaviorTree = MonsterData->AI;

	// 애니메이션 설정
	SetAllAnimation(MonsterData->Animations);
	GetGlobalAnimInstance()->AllAnimations = AllAnimations;

	// 초기 자세
	SetAniState(GenichiroState::Idle);

	// 항상 플레이어 락온 상태
	GetCharacterMovement()->bOrientRotationToMovement = false;
	
	GetBlackboardComponent()->SetValueAsFloat(TEXT("IdleWaitTime"), 1.f);
	GetBlackboardComponent()->SetValueAsEnum(TEXT("GenichiroState"), GetAniState());
	GetBlackboardComponent()->SetValueAsString(TEXT("TargetTag"), TEXT("Player"));
	GetBlackboardComponent()->SetValueAsFloat(TEXT("SearchRange"), 10000.f);
	GetBlackboardComponent()->SetValueAsFloat(TEXT("AttackRange"), 150.f);
}

void ABossGenichiro::Tick(float _Delta)
{
	Super::Tick(_Delta);

	GenichiroState AniStateValue = GetAniState<GenichiroState>();

	if (HP > MaxHP * 0.7f)
	{
		PostureRecoveryAmount = MaxPostureRecoveryAmount;
	}
	else if (HP > MaxHP * 0.4f)
	{
		PostureRecoveryAmount = MaxPostureRecoveryAmount * 0.5f;
	}
	else
	{
		PostureRecoveryAmount = MaxPostureRecoveryAmount * 0.25f;
	}

	if (bEnablePostureRecovery)
	{
		// 기본 자세 or 걷는 도중엔 체간 회복
		if (AniStateValue == GenichiroState::Idle
			|| AniStateValue == GenichiroState::LeftWalk || AniStateValue == GenichiroState::RightWalk)
		{
			Posture += PostureRecoveryAmount;
		}
		else if ((AniStateValue == GenichiroState::BottomAttack || AniStateValue == GenichiroState::StabAttack
			|| AniStateValue == GenichiroState::TakeDownAttack || AniStateValue == GenichiroState::ElectricSlash2)
			&& HitState == MonsterHitState::OFFGUARD)
		{
			Posture += PostureRecoveryAmount;
		}

		if (Posture > MaxPosture)
		{
			Posture = MaxPosture;
		}
	}

	// UE_LOG(LogTemp, Error, TEXT("hp : %f, posture : %f"), HP, Posture);
}

float ABossGenichiro::TakeDamage(float DamageAmount,
	struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator,
	AActor* DamageCauser)
{
	float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	UGlobalGameInstance* Inst = GetGameInstance<UGlobalGameInstance>();
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

		if (Posture <= 0.f)
		{
			Posture = 0.1f;
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

		if (Posture <= 0.f)
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

		if (HP <= 0.f)
		{
			HP = 0.1f;
		}
		
		if (Posture <= 0.f)
		{
			Posture = 0.1f;
		}

		SetAniState(GenichiroState::Shock);

		return Damage;
	}
	else if (DamageEvent.DamageTypeClass == UDeathblowType::StaticClass())
	{
		DamageType = Cast<UDeathblowType>(DamageEvent.DamageTypeClass->GetDefaultObject());

		GetHitImpulseManager(DamageCauser, 2000.f);

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
	else if (CalculateAngle(DamageCauser->GetActorLocation()) > 90.f)
	{
		GetHitExecute(DamageAmount, DamageType, DamageCauser);

		return Damage;
	}
	else if (HitState == MonsterHitState::GUARD && DamageType->bEnableGuard)
	{
		Posture -= DamageAmount * (DamageType->PostureDamageMultiple);

		GetHitImpulseManager(DamageCauser, 1500.f);

		GetWorld()->GetTimerManager().ClearTimer(PostureRecoveryManagerTimerHandle);
		bEnablePostureRecovery = false;
		GetWorld()->GetTimerManager().SetTimer(PostureRecoveryManagerTimerHandle, this, &AGlobalCharacter::PostureRecoveryManagerTimer, 0.5f, false);

		USoundBase* GuardSound = Inst->GetSoundData(TEXT("Player"), TEXT("AttackGuard"));
		if (IsValid(GuardSound))
		{
			UGameplayStatics::PlaySound2D(GetWorld(), GuardSound);
		}

		UParticleSystem* Effect = Inst->GetEffect(TEXT("AttackGuard"));
		if (IsValid(Effect))
		{
			FVector EffectLocation = GetActorLocation();
			FVector AdjustLocation = GetActorRightVector() * -50.f + FVector::UpVector * 30.0;
			EffectLocation = EffectLocation + AdjustLocation;
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Effect, EffectLocation, GetActorRotation());
		}

		if (Posture <= 0.f)
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

		GetHitImpulseManager(DamageCauser, 1500.f);

		GetWorld()->GetTimerManager().ClearTimer(PostureRecoveryManagerTimerHandle);
		bEnablePostureRecovery = false;
		GetWorld()->GetTimerManager().SetTimer(PostureRecoveryManagerTimerHandle, this, &AGlobalCharacter::PostureRecoveryManagerTimer, 0.5f, false);

		if (Posture <= 0.f)
		{
			Posture = 0.1f;
		}

		USoundBase* ParrySound = Inst->GetSoundData(TEXT("Monster"), TEXT("AttackParrying"));
		if (IsValid(ParrySound))
		{
			UGameplayStatics::PlaySound2D(GetWorld(), ParrySound);
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

	// 피격 유효타 이펙트 및 효과음 처리
	if (DamageType->GetClass() != UElectricSlashType::StaticClass())
	{
		GenichiroState AniStateValue = GetAniState<GenichiroState>();
		
		FVector StartPoint = DamageCauser->GetActorLocation();
		FVector EndPoint = GetActorLocation();

		// 하단 베기가 들어올 경우 공격 상대와 본인의 오프셋 조정
		if (DamageType->GetClass() == UBottomType::StaticClass())
		{
			StartPoint.Z -= 50.f;
			EndPoint.Z -= 50.f;
		}
		// 본인이 하단 베기 도중일 경우 본인의 오프셋만 조정
		else if (AniStateValue == GenichiroState::BottomAttack)
		{
			EndPoint.Z -= 50.f;
		}

		ApplyGetHitEffect(DamageCauser, StartPoint, EndPoint);

		// 효과음 처리
		UGlobalGameInstance* Inst = GetGameInstance<UGlobalGameInstance>();
		USoundBase* HitSound = Inst->GetSoundData(TEXT("Player"), TEXT("GetHit"));
		if (IsValid(HitSound))
		{
			UGameplayStatics::PlaySound2D(GetWorld(), HitSound);
		}
	}

	if (HP <= 0.f)
	{
		HP = 0.f;
		ExhaustAction();
	}
	else if (Posture <= 0.f)
	{
		ExhaustAction();
	}
	else
	{
		if (IsValid(DamageCauser) == false)
		{
			return;
		}

		if (HitState == MonsterHitState::SUPERARMOR)
		{
			return;
		}

		GetHitImpulseManager(DamageCauser, 1500.f);

		SetAniState(GenichiroState::Hit);
	}
}

void ABossGenichiro::ExhaustAction()
{
	Posture = 0.f;

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
		AttackMoveImpulse = 6000.f;
	}
	else
	{
		AttackMoveImpulse = 3000.f;
	}

	GetCharacterMovement()->AddImpulse(GetActorForwardVector() * AttackMoveImpulse, true);
}

void ABossGenichiro::Damage()
{
	// 범위에 있는 Player 콜리전 탐색
	EObjectTypeQuery ObjectType = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel1);
	TArray<AActor*> HitActor = TraceObjects(ObjectType, GetActorForwardVector(), 45.f, 50.f, 70.f);

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

		HitActor = TraceObjects(ObjectType, GetActorForwardVector(), 45.f, 500.f, 100.f);
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