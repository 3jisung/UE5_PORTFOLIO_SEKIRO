// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerSekiro.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Global/GlobalAnimInstance.h"
#include "Global/GlobalGameInstance.h"
#include "Global/Data/PlayerAnimData.h"
#include "Buddha/Buddha.h"
#include "GameMode/GlobalGameMode.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"


APlayerSekiro::APlayerSekiro()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->TargetArmLength = 500.f;
	SpringArmComponent->bDoCollisionTest = true;
	SpringArmComponent->SetupAttachment(RootComponent);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->FieldOfView = 90.f;
	CameraComponent->SetupAttachment(SpringArmComponent);

	// 위(危) UI 컴포넌트 설정
	WarningWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WarningWidgetComponent"));
	// WarningWidgetComponent->SetWidgetClass(WarningClassPath.TryLoadClass<UWarningWidget>());
	WarningWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	WarningWidgetComponent->SetDrawSize(FVector2D(200.f, 200.f));
	WarningWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 110.f));
	WarningWidgetComponent->SetupAttachment(RootComponent);

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(GetMesh(), TEXT("hand_r_weapon"));

	GourdMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GourdMesh"));
	GourdMesh->SetupAttachment(GetMesh(), TEXT("Gourd"));
}

void APlayerSekiro::BeginPlay()
{
	Super::BeginPlay();

	// 애니메이션 종료 시 MontageEnd를 콜백한다.
	GetGlobalAnimInstance()->OnMontageBlendingOut.AddDynamic(this, &APlayerSekiro::MontageBlendingOut);

	SetAniState(SekiroState::Idle);

	// 선입력 버퍼 초기화
	// 선입력 버퍼에 들어갈 수 있는 값 : None, BasicAttack(1, 2, 3), Jump, Dash, Guard
	BufferedAction = SekiroState::None;

	Tags.Add(TEXT("Player"));

	UGlobalGameInstance* Inst = GetGameInstance<UGlobalGameInstance>();

	// 캐릭터 기본 스탯 설정
	StatData = Inst->GetPlayerStat(TEXT("Sekiro"));
	MaxHP = StatData->MaxHP;
	HP = MaxHP;
	MaxPosture = StatData->MaxPosture;
	Posture = MaxPosture;
	Power = StatData->Power;

	Speed = DefaultSpeed;
	HealCount = MaxHealCount;

	MaxPostureRecoveryAmount = MaxPosture * 0.002f;

	// 캐릭터 무기 적용
	StaticMeshArrays.Add(Inst->GetPlayerMesh(TEXT("Katana")));
	WeaponMesh->SetStaticMesh(StaticMeshArrays[0]);

	// StaticMeshArrays에 표주박 추가
	// 실제 매쉬 적용은 회복 애니메이션 진행 중에만 적용
	StaticMeshArrays.Add(Inst->GetPlayerMesh(TEXT("Gourd")));
	//GourdMesh->SetStaticMesh(StaticMeshArrays[1]);
	GourdMesh->SetStaticMesh(nullptr);

	// 캐릭터 애니메이션 설정
	FPlayerAnimData* AnimData = Inst->GetPlayerAnim(TEXT("Animations"));
	SetAllAnimation(AnimData->Animations);
	GetGlobalAnimInstance()->AllAnimations = AllAnimations;

	// 위(危) 문자 아이콘 설정
	TSubclassOf<UUserWidget> WidgetClass = Inst->GetWidgetClassData(TEXT("Player"), TEXT("Warning"));
	if (IsValid(WidgetClass))
	{
		WarningWidgetComponent->SetWidgetClass(WidgetClass);
	}

	// 락온 상태가 아닐 때 움직임을 Forward로 고정하기 위한 설정값.
	// 락온 상태일 땐 false로 변경해야 한다.
	GetCharacterMovement()->bOrientRotationToMovement = true;

	// 중력, 점프 높이 설정
	GetCharacterMovement()->GravityScale = 1.8f;
	GetCharacterMovement()->JumpZVelocity = 800.f;
}

void APlayerSekiro::Tick(float _Delta)
{
	Super::Tick(_Delta);

	SekiroState AniStateValue = GetAniState<SekiroState>();
	UCharacterMovementComponent* Move = Cast<UCharacterMovementComponent>(GetMovementComponent());

	// 이동 방향이 앞방향이 아닐 경우 이동 속도 감소
	if (AniStateValue == SekiroState::ForwardWalk || AniStateValue == SekiroState::ForwardRun)
	{
		Move->MaxWalkSpeed = Speed;
	}
	else
	{
		Move->MaxWalkSpeed = Speed * 0.85f;
	}

	if (bLockOn && LockedOnTarget != nullptr)
	{
		// 락온 범위를 벗어나면 락온 자동 해제
		FVector LockedOnLocation = LockedOnTarget->GetActorLocation();
		float Dis = (GetActorLocation() - LockedOnLocation).Size();
		if (Dis > LockOnRange)
		{
			ToggleLockOn();
			return;
		}
		else if (IsValid(Cast<AGlobalCharacter>(LockedOnTarget)) && Cast<AGlobalCharacter>(LockedOnTarget)->IsDeath())
		{
			ToggleLockOn();
			return;
		}
		else if (this->IsDeath())
		{
			ToggleLockOn();
			return;
		}

		if (IsValid(Cast<AGlobalCharacter>(LockedOnTarget)))
		{
			LockedOnLocation.Z -= 35.f;
		}
		else if (IsValid(Cast<ABuddha>(LockedOnTarget)))
		{
			LockedOnLocation.Z += 100.f;
		}

		const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), LockedOnLocation);
		FRotator InterpRotation = UKismetMathLibrary::RInterpTo(GetController()->GetControlRotation(), LookAtRotation, _Delta, 10.f);

		if (IsValid(Cast<AGlobalCharacter>(LockedOnTarget)))
		{
			InterpRotation.Pitch -= 1.5f;
		}

		// 프레임 마다 캐릭터와 카메라의 시점 최신화
		SetActorRotation(FRotator(0.f, InterpRotation.Yaw, 0.f));
		GetController()->SetControlRotation(FRotator(InterpRotation));
	}
	
	// 현재 체력에 따라 체간 회복 속도 가변
	if (HP > MaxHP * 0.7f)
	{
		PostureRecoveryAmount = MaxPostureRecoveryAmount;
	}
	else if (HP > MaxHP * 0.4f)
	{
		PostureRecoveryAmount = MaxPostureRecoveryAmount * 0.75f;
	}
	else
	{
		PostureRecoveryAmount = MaxPostureRecoveryAmount * 0.5f;
	}

	if (bEnablePostureRecovery)
	{
		// 기본 자세 or 걷는 도중엔 체간 회복
		if (AniStateValue == SekiroState::ForwardWalk || AniStateValue == SekiroState::BackwardWalk
			|| AniStateValue == SekiroState::LeftWalk || AniStateValue == SekiroState::RightWalk
			|| AniStateValue == SekiroState::Idle)
		{
			Posture += PostureRecoveryAmount;
		}
		// 가드 키다운 중엔 체간 회복 속도 2배
		else if (AniStateValue == SekiroState::Guard)
		{
			Posture += PostureRecoveryAmount * 2.f;
		}

		if (Posture > MaxPosture)
		{
			Posture = MaxPosture;
		}
	}

	// UE_LOG(LogTemp, Error, TEXT("hp : %f, posture : %f"), HP, Posture);
}

void APlayerSekiro::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

float APlayerSekiro::TakeDamage(float DamageAmount,
	struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator,
	AActor* DamageCauser)
{
	float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	UGlobalGameInstance* Inst = GetGameInstance<UGlobalGameInstance>();
	SekiroState AniStateValue = GetAniState<SekiroState>();

	// 버퍼 초기화
	ClearBuffer();

	// 불상 이벤트 도중 피격 시 불상 락온 해제
	if (bLockOn && IsValid(Cast<ABuddha>(LockedOnTarget)))
	{
		ToggleLockOn();
	}

	UCustomDamageTypeBase* DamageType;

	// 데미지 타입 확인
	if (DamageEvent.DamageTypeClass == UBasicAttackType::StaticClass())
	{
		DamageType = Cast<UBasicAttackType>(DamageEvent.DamageTypeClass->GetDefaultObject());
	}
	else if (DamageEvent.DamageTypeClass == UStabType::StaticClass())
	{
		DamageType = Cast<UStabType>(DamageEvent.DamageTypeClass->GetDefaultObject());
	}
	else if (DamageEvent.DamageTypeClass == UTakeDownType::StaticClass())
	{
		DamageType = Cast<UTakeDownType>(DamageEvent.DamageTypeClass->GetDefaultObject());
	}
	else if (DamageEvent.DamageTypeClass == UBottomType::StaticClass())
	{
		DamageType = Cast<UBottomType>(DamageEvent.DamageTypeClass->GetDefaultObject());
	}
	else if (DamageEvent.DamageTypeClass == UElectricSlashType::StaticClass())
	{
		DamageType = Cast<UElectricSlashType>(DamageEvent.DamageTypeClass->GetDefaultObject());
	}
	else if (DamageEvent.DamageTypeClass == UParryType::StaticClass())
	{
		DamageType = Cast<UParryType>(DamageEvent.DamageTypeClass->GetDefaultObject());

		SetAniState(SekiroState::Blocked);

		return Damage;
	}
	else
	{
		return Damage;
	}

	// 특수 피격 이벤트 처리
	if (HitState == PlayerHitState::INVINCIBLE)
	{
		return Damage;
	}
	// 대쉬 무적, 간파하기 체크
	else if (HitState == PlayerHitState::DASHINVINCIBLE
		&& (AniStateValue == SekiroState::ForwardRun || AniStateValue == SekiroState::BackwardRun
			|| AniStateValue == SekiroState::LeftRun || AniStateValue == SekiroState::RightRun))
	{
		if (AniStateValue == SekiroState::ForwardRun && DamageType->bEnableMikiri
			&& CalculateAngle(DamageCauser->GetActorLocation()) <= 45.f)
		{
			AdjustAngle(DamageCauser->GetActorLocation());

			HitState = PlayerHitState::INVINCIBLE;
			SetAniState(SekiroState::MikiriCounter);

			/* 간파 인살
			float PostureDamage = Cast<UMikiriType>(UMikiriType::StaticClass()->GetDefaultObject())->DamageMultiple * this->Power;

			if (Cast<AMonster>(DamageCauser)->GetPosture() - PostureDamage <= 0)
			{
				SetAniState(SekiroState::ReadyToDeathblowMikiri);
			}
			else
			{
				SetAniState(SekiroState::MikiriCounter);
			}
			*/

			// TSubclassOf<UDamageType> HitDamageType = UMikiriType::StaticClass();
			// UGameplayStatics::ApplyDamage(DamageCauser, this->Power, GetController(), this, HitDamageType);
		}
		// 하단 베기는 대쉬 무적으로 회피 불가능
		else if (DamageType->GetClass() == UBottomType::StaticClass())
		{
			GetHitExecute(DamageAmount, DamageType, DamageCauser);
		}

		return Damage;
	}
	else if (DamageType->bEnableJumpEvasion && bLowInvincible)
	{
		return Damage;
	}
	// 뇌격은 공중에 있으면 뇌반 가능, 그 외에는 타뢰
	else if (DamageType->bEnableLightningReversal && GetMovementComponent()->IsFalling())
	{
		HP -= (DamageAmount * 0.5f);

		if (HP <= 0.f)
		{
			DeathAction();
			return Damage;
		}

		SavedDamage = DamageAmount;

		FVector ImpulseVector = DamageCauser->GetActorForwardVector();
		ImpulseVector = ImpulseVector.RotateAngleAxis(-30.f, DamageCauser->GetActorRightVector());
		GetCharacterMovement()->AddImpulse(ImpulseVector * 100.f, true);

		SetAniState(SekiroState::LightningReversal1);

		return Damage;
	}
	// Rotation 체크(상대방의 위치와 90도 이상 차이날 경우 무적 상태가 아닌 이상 무조건 피격)
	// 즉, 가드 및 패링 불가능
	else if (CalculateAngle(DamageCauser->GetActorLocation()) > 90.f)
	{
		GetHitExecute(DamageAmount, DamageType, DamageCauser);

		return Damage;
	}
	
	// 그 외 일반적인 피격 처리
	if (HitState == PlayerHitState::GUARD && DamageType->bEnableGuard)
	{
		Posture -= DamageAmount * (DamageType->PostureDamageMultiple);

		GetHitImpulseManager(DamageCauser, DamageType->PushPower);

		// 피격 방어, 패링 시 0.5초 간 체간 회복 불가능
		// 가드만 누르면서 체간을 쉽게 회복하지 못하도록 하기 위함
		GetWorld()->GetTimerManager().ClearTimer(PostureRecoveryManagerTimerHandle);
		bEnablePostureRecovery = false;
		GetWorld()->GetTimerManager().SetTimer(PostureRecoveryManagerTimerHandle, this, &AGlobalCharacter::PostureRecoveryManagerTimer, 0.5f, false);

		USoundBase* GuardSound = Inst->GetSoundData(TEXT("Player"), TEXT("AttackGuard"));
		if (IsValid(GuardSound))
		{
			UGameplayStatics::PlaySound2D(GetWorld(), GuardSound);
		}

		TSubclassOf<UObject> GuardEffect = Inst->GetEffect(TEXT("AttackGuard"));
		if (IsValid(GuardEffect))
		{
			AActor* Effect = GetWorld()->SpawnActor<AActor>(GuardEffect);
			Effect->SetActorLocation(GetActorLocation());
			Effect->SetActorRotation(GetActorRotation());
			Effect->AddActorLocalOffset(FVector(0.f, -30.f, 30.f));

			// DestroyTime 뒤 이펙트 액터 삭제
			float DestroyTime = 1.f;
			FTimerHandle myTimerHandle;
			GetWorld()->GetTimerManager().SetTimer(myTimerHandle, FTimerDelegate::CreateLambda([&, Effect]()
				{
					Effect->Destroy();

					GetWorld()->GetTimerManager().ClearTimer(myTimerHandle);
				}), DestroyTime, false);
		}
		
		if (Posture <= 0.f)
		{
			ExhaustAction();
		}
	}
	else if (HitState == PlayerHitState::PARRYING && DamageType->bEnableParrying)
	{
		// 패링 성공 시 체간 데미지 25% 감소
		Posture -= (DamageAmount * 0.75f) * (DamageType->PostureDamageMultiple);

		GetHitImpulseManager(DamageCauser, DamageType->PushPower);

		GetWorld()->GetTimerManager().ClearTimer(PostureRecoveryManagerTimerHandle);
		bEnablePostureRecovery = false;
		GetWorld()->GetTimerManager().SetTimer(PostureRecoveryManagerTimerHandle, this, &AGlobalCharacter::PostureRecoveryManagerTimer, 0.5f, false);

		if (Posture <= 0.f)
		{
			Posture = 0.1f;
		}

		HitState = PlayerHitState::OFFGUARD;
		SetAniState((int)SekiroState::Parrying1 + ParryingCount);

		USoundBase* ParrySound = Inst->GetSoundData(TEXT("Player"), TEXT("AttackParrying"));
		if (IsValid(ParrySound))
		{
			UGameplayStatics::PlaySound2D(GetWorld(), ParrySound);
		}

		++ParryingCount;
		if (ParryingCount > 1)
		{
			ParryingCount = 0;
		}

		// 패링 시 체간 반사 데미지
		TSubclassOf<UDamageType> HitDamageType = UParryType::StaticClass();
		UGameplayStatics::ApplyDamage(DamageCauser, this->Power, GetController(), this, HitDamageType);
	}
	else
	{
		GetHitExecute(DamageAmount, DamageType, DamageCauser);
	}

	return Damage;
}

void APlayerSekiro::GetHitExecute(float DamageAmount, UCustomDamageTypeBase* DamageType, AActor* DamageCauser)
{
	HP -= DamageAmount * (DamageType->HPDamageMultiple);
	Posture -= DamageAmount * (DamageType->PostureDamageMultiple);

	UGlobalGameInstance* Inst = GetGameInstance<UGlobalGameInstance>();
	USoundBase* HitSound = Inst->GetSoundData(TEXT("Player"), TEXT("GetHit"));
	if (IsValid(HitSound) && DamageType->GetClass() != UElectricSlashType::StaticClass())
	{
		UGameplayStatics::PlaySound2D(GetWorld(), HitSound);
	}

	if (HP <= 0.f)
	{
		DeathAction();
	}
	else if (DamageType->GetClass() == UElectricSlashType::StaticClass())
	{
		SetAniState(SekiroState::Shock);
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

		GetHitImpulseManager(DamageCauser, DamageType->PushPower);

		if (DamageType->GetClass() == UStabType::StaticClass() || DamageType->GetClass() == UTakeDownType::StaticClass())
		{
			HitState = PlayerHitState::INVINCIBLE;
			SetAniState(SekiroState::Knockdown);
		}
		else
		{
			SetAniState(SekiroState::Hit);
		}
	}
}

void APlayerSekiro::ExhaustAction()
{
	Posture = MaxPosture;

	HitState = PlayerHitState::OFFGUARD;
	SetAniState(SekiroState::Exhaust);
}

void APlayerSekiro::DeathAction()
{
	HP = 0.f;
	Posture = MaxPosture;

	HitState = PlayerHitState::INVINCIBLE;
	SetAniState(SekiroState::Death);
}

void APlayerSekiro::ShowWarningIcon()
{
	UWarningWidget* WarningWidget = Cast<UWarningWidget>(WarningWidgetComponent->GetWidget());

	WarningWidget->FadeIn();

	UGlobalGameInstance* Inst = GetGameInstance<UGlobalGameInstance>();
	USoundBase* WarningSound = Inst->GetSoundData(TEXT("Player"), TEXT("Warning"));
	if (IsValid(WarningSound))
	{
		UGameplayStatics::PlaySound2D(GetWorld(), WarningSound);
	}

	float delayTime = 0.5f;
	FTimerHandle myTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(myTimerHandle, FTimerDelegate::CreateLambda([&, WarningWidget]()
		{
			WarningWidget->FadeOut();

			GetWorld()->GetTimerManager().ClearTimer(myTimerHandle);
		}), delayTime, false);
}

void APlayerSekiro::Damage()
{
	// 범위에 있는 Monster 콜리전 탐색
	EObjectTypeQuery ObjectType = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel2);
	TArray<AActor*> HitActor = TraceObjects(ObjectType, GetActorForwardVector(), 45.f, 50.f, 70.f);

	SekiroState AniStateValue = GetAniState<SekiroState>();
	TSubclassOf<UDamageType> DamageType;

	if (AniStateValue == SekiroState::BasicAttack1 || AniStateValue == SekiroState::BasicAttack2
		|| AniStateValue == SekiroState::BasicAttack3 || AniStateValue == SekiroState::DashAttack
		|| AniStateValue == SekiroState::JumpAttack)
	{
		DamageType = UBasicAttackType::StaticClass();
	}
	else if (AniStateValue == SekiroState::StabAttack2)
	{
		DamageType = UStabType::StaticClass();
	}
	else if (AniStateValue == SekiroState::MikiriCounter)
	{
		DamageType = UMikiriType::StaticClass();
	}
	else if (AniStateValue == SekiroState::LightningReversal2)
	{
		DamageType = UElectricSlashType::StaticClass();

		HitActor = TraceObjects(ObjectType, GetActorForwardVector(), 45.f, 500.f, 100.f);
	}
	else if (AniStateValue == SekiroState::Trample)
	{
		DamageType = UTrampleType::StaticClass();
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


void APlayerSekiro::MoveForward(float Val)
{
	SekiroState AniStateValue = GetAniState<SekiroState>();

	// 조건문에 명시된 플레이어 상태값이 아닐 경우 return
	if (AniStateValue != SekiroState::Idle
		&& AniStateValue != SekiroState::ForwardWalk && AniStateValue != SekiroState::BackwardWalk
		&& AniStateValue != SekiroState::LeftWalk && AniStateValue != SekiroState::RightWalk
		&& AniStateValue != SekiroState::ForwardRun && AniStateValue != SekiroState::BackwardRun
		&& AniStateValue != SekiroState::LeftRun && AniStateValue != SekiroState::RightRun)
	{
		return;
	}

	if (Val != 0.f)
	{
		if (Controller)
		{
			// 컨트롤러는 기본적으로 character 당 하나씩 붙어 있다.
			FRotator const ControlSpaceRot = Controller->GetControlRotation();	
			FVector Direction = FRotationMatrix(ControlSpaceRot).GetScaledAxis(EAxis::X);
			Direction.Z = 0.f;
			Direction.Normalize();
			AddMovementInput(Direction, Val);

			if (bLockOn)
			{
				if (bDash)
				{
					SetAniState(Val > 0.f ? SekiroState::ForwardRun : SekiroState::BackwardRun);
				}
				else
				{
					SetAniState(Val > 0.f ? SekiroState::ForwardWalk : SekiroState::BackwardWalk);
				}
			}
			else
			{
				if (bDash)
				{
					SetAniState(SekiroState::ForwardRun);
				}
				else
				{
					SetAniState(SekiroState::ForwardWalk);
				}
			}

			return;
		}
	}
	else
	{
		// 키를 떼면 IDLE로 상태 전환
		if (AniStateValue == SekiroState::ForwardWalk || AniStateValue == SekiroState::BackwardWalk
			|| AniStateValue == SekiroState::ForwardRun || AniStateValue == SekiroState::BackwardRun)
		{
			SetAniState(SekiroState::Idle);
		}
	}
}

void APlayerSekiro::MoveRight(float Val)
{
	SekiroState AniStateValue = GetAniState<SekiroState>();

	if (AniStateValue != SekiroState::Idle
		&& AniStateValue != SekiroState::ForwardWalk && AniStateValue != SekiroState::BackwardWalk
		&& AniStateValue != SekiroState::LeftWalk && AniStateValue != SekiroState::RightWalk
		&& AniStateValue != SekiroState::ForwardRun && AniStateValue != SekiroState::BackwardRun
		&& AniStateValue != SekiroState::LeftRun && AniStateValue != SekiroState::RightRun)
	{
		return;
	}

	if (Val != 0.f)
	{
		if (Controller)
		{
			FRotator const ControlSpaceRot = Controller->GetControlRotation();
			// 현재 내 회전을 가져와서 y축에 해당하는 축벡터를 얻어 온다.
			FVector Direction = FRotationMatrix(ControlSpaceRot).GetScaledAxis(EAxis::Y);
			Direction.Z = 0.f;
			Direction.Normalize();
			AddMovementInput(Direction, Val);

			if (bLockOn)
			{
				if (bDash)
				{
					SetAniState(Val > 0.f ? SekiroState::RightRun : SekiroState::LeftRun);
				}
				else
				{
					SetAniState(Val > 0.f ? SekiroState::RightWalk : SekiroState::LeftWalk);
				}
			}
			else
			{
				if (bDash)
				{
					SetAniState(SekiroState::ForwardRun);
				}
				else
				{
					SetAniState(SekiroState::ForwardWalk);
				}
			}
			
			return;
		}
	}
	else
	{
		// 락온 상태가 아닐 때 키를 떼면 마지막 상태가 ForwardWalk 또는 ForwardRun 이므로 관련 조건문 추가
		if (AniStateValue == SekiroState::RightWalk || AniStateValue == SekiroState::LeftWalk
			|| AniStateValue == SekiroState::RightRun || AniStateValue == SekiroState::LeftRun
			|| AniStateValue == SekiroState::ForwardWalk || AniStateValue == SekiroState::ForwardRun)
		{
			SetAniState(SekiroState::Idle);
		}
	}
}

void APlayerSekiro::PlayerJump()
{
	SekiroState AniStateValue = GetAniState<SekiroState>();

	// 대기, 가드, 걷기, 달리기 상태일 때만 점프 가능
	if (AniStateValue != SekiroState::Idle && AniStateValue != SekiroState::Guard
		
		&& AniStateValue != SekiroState::ForwardWalk && AniStateValue != SekiroState::BackwardWalk
		&& AniStateValue != SekiroState::LeftWalk && AniStateValue != SekiroState::RightWalk
		&& AniStateValue != SekiroState::ForwardRun && AniStateValue != SekiroState::BackwardRun
		&& AniStateValue != SekiroState::LeftRun && AniStateValue != SekiroState::RightRun
		
		&& AniStateValue != SekiroState::BasicAttack1 && AniStateValue != SekiroState::BasicAttack2
		&& AniStateValue != SekiroState::BasicAttack3 && AniStateValue != SekiroState::DashAttack
		&& AniStateValue != SekiroState::StabAttack1 && AniStateValue != SekiroState::StabAttack2)
	{
		return;
	}

	if (AniStateValue == SekiroState::BasicAttack1 || AniStateValue == SekiroState::BasicAttack2
		|| AniStateValue == SekiroState::BasicAttack3 || AniStateValue == SekiroState::DashAttack
		|| AniStateValue == SekiroState::StabAttack2)
	{
		if (bAttackCombo && BufferedAction == SekiroState::None)
		{
			// 점프 선입력
			if (bEnteredTransition == false)
			{
				BufferedAction = SekiroState::JumpStart;

				return;
			}
			else
			{
				SetAniState(SekiroState::Idle);
			}
		}
		else
		{
			return;
		}
	}

	if (AniStateValue == SekiroState::StabAttack1)
	{
		bAttackEnable = false;
	}

	// 점프 시 1.1초간 하단 무적
	// 하단 무적은 다른 HitState와 중복 적용 가능하다.(대쉬 무적 제외)
	bLowInvincible = true;

	float delayTime = 1.1f;
	FTimerHandle myTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(myTimerHandle, FTimerDelegate::CreateLambda([&]()
		{
			bLowInvincible = false;

			GetWorld()->GetTimerManager().ClearTimer(myTimerHandle);
		}), delayTime, false);

	SetAniState(SekiroState::JumpStart);
	Jump();
}

// 점프 후 착지 시 호출
void APlayerSekiro::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	SekiroState AniStateValue = GetAniState<SekiroState>();

	// 뇌격을 되받아치지 못하고 땅에 발이 닿으면 타뢰 처리
	if (AniStateValue == SekiroState::LightningReversal1)
	{
		GetHitExecute(SavedDamage, Cast<UElectricSlashType>(UElectricSlashType::StaticClass()->GetDefaultObject()), nullptr);
		SavedDamage = 0.f;
	}
	else if (AniStateValue == SekiroState::JumpStart || AniStateValue == SekiroState::JumpLoop)
	{
		SetAniState(SekiroState::JumpEnd);
	}
}

void APlayerSekiro::StartedPlayerDash()
{
	SekiroState AniStateValue = GetAniState<SekiroState>();

	// 대쉬 시작 전 상태값이 대기, 가드, 걷기 상태일 때만 대쉬 무적 및 디폴트 전방 대쉬 적용
	if (AniStateValue != SekiroState::Idle && AniStateValue != SekiroState::Guard
		
		&& AniStateValue != SekiroState::ForwardWalk && AniStateValue != SekiroState::BackwardWalk
		&& AniStateValue != SekiroState::LeftWalk && AniStateValue != SekiroState::RightWalk
		&& AniStateValue != SekiroState::ForwardRun && AniStateValue != SekiroState::BackwardRun
		&& AniStateValue != SekiroState::LeftRun && AniStateValue != SekiroState::RightRun
		
		&& AniStateValue != SekiroState::BasicAttack1 && AniStateValue != SekiroState::BasicAttack2
		&& AniStateValue != SekiroState::BasicAttack3
		&& AniStateValue != SekiroState::StabAttack1 && AniStateValue != SekiroState::StabAttack2)
	{
		return;
	}

	if (AniStateValue == SekiroState::BasicAttack1 || AniStateValue == SekiroState::BasicAttack2
		|| AniStateValue == SekiroState::BasicAttack3 || AniStateValue == SekiroState::StabAttack2)
	{
		if (bAttackCombo && BufferedAction == SekiroState::None)
		{
			// 대쉬 선입력
			if (bEnteredTransition == false)
			{
				CorrectedTime = GetWorld()->GetTimeSeconds();
				BufferedAction = SekiroState::ForwardRun;
				
				return;
			}
			else
			{
				SetAniState(SekiroState::Idle);
			}
		}
		else
		{
			return;
		}
	}

	if (AniStateValue == SekiroState::StabAttack1)
	{
		SetAniState(SekiroState::Idle);
		bAttackEnable = false;
	}
	else if (AniStateValue == SekiroState::Guard)
	{
		SetAniState(SekiroState::ForwardRun);
	}

	// 대쉬 키를 0.5초 내에 연속으로 눌렀을 경우 대쉬 무적 적용하지 않음.
	// 대쉬 키를 연타하여 대쉬 무적을 과도하게 취하지 않도록 하기 위함
	float CurDashTime = GetWorld()->GetTimeSeconds();
	float IntervalTime = CurDashTime - PreDashTime;

	if (PreDashTime != 0.f && IntervalTime <= 0.5f)
	{
		PreDashTime = CurDashTime;
		return;
	}

	PreDashTime = CurDashTime;


	// 대쉬 시작 시 0.3초간 대쉬 무적(대쉬 상태일 때만 적용)
	// 0.3초가 지나기 전에 가드를 누를 경우 가드/패링 상태로 전환되어 무적이 취소되므로 OFFGUARD로 되돌릴 필요 없다.
	HitState = PlayerHitState::DASHINVINCIBLE;
	
	float InvincibleTime = 0.3f;
	FTimerHandle InvincibleTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(InvincibleTimerHandle, FTimerDelegate::CreateLambda([&]()
		{
			if (HitState == PlayerHitState::DASHINVINCIBLE)
			{
				HitState = PlayerHitState::OFFGUARD;
			}

			GetWorld()->GetTimerManager().ClearTimer(InvincibleTimerHandle);
		}), InvincibleTime, false);


	// 방향키 입력 없이도 처음 0.5초간은 전방 대쉬가 가능하도록 구현(기술명 : 디폴트 전방 대쉬)
	// 대쉬 방향은 현재 캐릭터가 바라보고 있는 방향(컨트롤러 방향 x)
	bStartedDash = true;
	GetWorld()->GetTimerManager().SetTimer(StartedDashTimerHandle, this, &APlayerSekiro::StartedPlayerDashMove, 0.001f, true);

	float DefaultDashTime = 0.5f;
	FTimerHandle DefaultDashTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(DefaultDashTimerHandle, FTimerDelegate::CreateLambda([&]()
		{
			bStartedDash = false;
			AniStateValue = GetAniState<SekiroState>();

			if (AniStateValue == SekiroState::ForwardRun || AniStateValue == SekiroState::BackwardRun
				|| AniStateValue == SekiroState::LeftRun || AniStateValue == SekiroState::RightRun)
			{
				SetAniState(SekiroState::Idle);
			}

			GetWorld()->GetTimerManager().ClearTimer(DefaultDashTimerHandle);
		}), DefaultDashTime, false);
}

void APlayerSekiro::StartedPlayerDashMove()
{
	SekiroState AniStateValue = GetAniState<SekiroState>();
	
	if (bStartedDash)
	{
		bDash = true;

		// 첫 대쉬 가속도 보정
		if (AniStateValue == SekiroState::ForwardRun)
		{
			GetCharacterMovement()->AddImpulse(GetActorForwardVector() * 10.f, true);
		}

		// 처음 대쉬 시 방향키 입력이 없는 경우 0.5초간 전방 대쉬
		// 도중에 키 입력이 들어오면 적용하지 않음
		if (bInputWASD == false && (AniStateValue == SekiroState::Idle || AniStateValue == SekiroState::ForwardRun))
		{
			AddMovementInput(GetActorForwardVector(), 1.f);
			SetAniState(SekiroState::ForwardRun);
		}
	}
	else
	{
		Speed = DefaultSpeed;
		bDash = false;
		
		GetWorld()->GetTimerManager().ClearTimer(StartedDashTimerHandle);
	}
}

void APlayerSekiro::TriggeredPlayerDash(bool ActionValue, float TriggeredSec)
{
	if (ActionValue)
	{
		Speed = DashSpeed;
		bDash = ActionValue;
	}
	else
	{
		if (BufferedAction == SekiroState::ForwardRun && bBufferedCompletedDash == false)
		{
			bBufferedCompletedDash = true;
			return;
		}
		// 대쉬 키를 눌렀다가 바로 떼어도 최소한 0.5초 동안은 대쉬 상태를 유지
		// 단, bStartedDash 값이 true 일 때만 적용
		float MinDashTime = 0.5f;

		if (bStartedDash)
		{
			float delayTime = MinDashTime - TriggeredSec + CorrectedTime;
			CorrectedTime = 0.f;

			if (delayTime <= 0.f)
			{
				delayTime = 0.01f;
			}

			FTimerHandle myTimerHandle;
			GetWorld()->GetTimerManager().SetTimer(myTimerHandle, FTimerDelegate::CreateLambda([&]()
				{
					Speed = DefaultSpeed;
					bDash = false;

					GetWorld()->GetTimerManager().ClearTimer(myTimerHandle);
				}), delayTime, false);
		}
		else
		{
			Speed = DefaultSpeed;
			bDash = ActionValue;
		}
	}
}

void APlayerSekiro::StartedPlayerGuard()
{
	// 가드 키를 0.3초 내에 연속으로 눌렀을 경우 점차 패링 판정 시간 감소(0.05초씩 감소)
	// 가드 키를 연타하여 패링 판정을 쉽게 취하지 못하도록 하기 위함
	float CurGuardTime = GetWorld()->GetTimeSeconds();
	float IntervalTime = CurGuardTime - PreGuardTime;

	if (PreGuardTime != 0.f && IntervalTime <= 0.3f)
	{
		ParryingValidTime -= 0.05f;

		if (ParryingValidTime <= 0.f)
		{
			ParryingValidTime = 0.f;
		}
	}
	else
	{
		ParryingValidTime = 0.2f;
	}

	PreGuardTime = CurGuardTime;

	// 이전에 돌아가는 타이머가 있을 경우 초기화
	GetWorld()->GetTimerManager().ClearTimer(ParryingTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(GuardTimerHandle);
	

	SekiroState AniStateValue = GetAniState<SekiroState>();

	if (AniStateValue != SekiroState::Idle && AniStateValue != SekiroState::Guard
		&& AniStateValue != SekiroState::JumpStart && AniStateValue != SekiroState::JumpLoop

		&& AniStateValue != SekiroState::ForwardWalk && AniStateValue != SekiroState::BackwardWalk
		&& AniStateValue != SekiroState::LeftWalk && AniStateValue != SekiroState::RightWalk
		&& AniStateValue != SekiroState::ForwardRun && AniStateValue != SekiroState::BackwardRun
		&& AniStateValue != SekiroState::LeftRun && AniStateValue != SekiroState::RightRun

		&& AniStateValue != SekiroState::BasicAttack1 && AniStateValue != SekiroState::BasicAttack2
		&& AniStateValue != SekiroState::BasicAttack3 && AniStateValue != SekiroState::DashAttack
		&& AniStateValue != SekiroState::StabAttack1 && AniStateValue != SekiroState::StabAttack2)
	{
		return;
	}

	if (AniStateValue == SekiroState::BasicAttack1 || AniStateValue == SekiroState::BasicAttack2
		|| AniStateValue == SekiroState::BasicAttack3 || AniStateValue == SekiroState::DashAttack
		|| AniStateValue == SekiroState::StabAttack2)
	{
		if (bAttackCombo && BufferedAction == SekiroState::None)
		{
			// 가드 선입력
			if (bEnteredTransition == false)
			{
				CorrectedTime = GetWorld()->GetTimeSeconds();
				BufferedAction = SekiroState::Guard;

				return;
			}
			else
			{
				SetAniState(SekiroState::Idle);
			}
		}
		else
		{
			return;
		}
	}

	if (AniStateValue == SekiroState::StabAttack1)
	{
		SetAniState(SekiroState::Idle);
		bAttackEnable = false;
	}

	bGuardTimer = true;

	// 가드 시작 시 0.2초간 패링 유효, 그 이후 가드 판정
	// 가드 연타로 인해 패링 유효 시간이 감소할 경우 그만큼 가드 유지 시간이 늘어난다.(0.4초는 그대로)
	// 애니메이션과 실제 피격 판정은 각각 다른 시간으로 관리한다.(자연스러운 연출을 위함)
	// 도중에 공격, 대쉬, 점프 등으로 인한 상태 변화가 있을 경우 패링 판정이 제거되므로 Guard로 변경할 필요가 없다.
	// Guard도 패링과 동일하게 처리
	if (ParryingValidTime != 0.f)
	{
		HitState = PlayerHitState::PARRYING;

		GetWorld()->GetTimerManager().SetTimer(ParryingTimerHandle, this, &APlayerSekiro::ManageParryingTimer, ParryingValidTime, false);
	}
	else
	{
		HitState = PlayerHitState::GUARD;

		GetWorld()->GetTimerManager().SetTimer(GuardTimerHandle, this, &APlayerSekiro::ManageGuardTimer, MaxGuardValidTime, false);
	}

	SetAniState(SekiroState::Guard);
}

void APlayerSekiro::TriggeredPlayerGuard(bool ActionValue, float TriggeredSec)
{
	SekiroState AniStateValue = GetAniState<SekiroState>();

	if (ActionValue)
	{
		if (AniStateValue == SekiroState::Idle || AniStateValue == SekiroState::Guard
			|| AniStateValue == SekiroState::JumpStart || AniStateValue == SekiroState::JumpLoop
			|| AniStateValue == SekiroState::ForwardWalk || AniStateValue == SekiroState::BackwardWalk
			|| AniStateValue == SekiroState::LeftWalk || AniStateValue == SekiroState::RightWalk)
		{
			if (HitState != PlayerHitState::PARRYING)
			{
				HitState = PlayerHitState::GUARD;
				SetAniState(SekiroState::Guard);
			}	
		}
	}
	else
	{
		if (BufferedAction == SekiroState::Guard && bBufferedCompletedGuard == false)
		{
			bBufferedCompletedGuard = true;
			return;
		}
		// 가드 키를 눌렀다가 바로 떼어도 최소한 0.2초 동안은 가드 애니메이션 유지
		// 애니메이션과 별개로 가드 판정은 최소 0.4초까지 유지될 수 있다.
		float MinGuardAnimationTime = 0.2f;
		float delayTime = MinGuardAnimationTime + CorrectedTime;
		CorrectedTime = 0.f;

		if (TriggeredSec <= delayTime)
		{
			FTimerHandle myTimerHandle;
			GetWorld()->GetTimerManager().SetTimer(myTimerHandle, FTimerDelegate::CreateLambda([&]()
				{
					AniStateValue = GetAniState<SekiroState>();

					if (AniStateValue == SekiroState::Guard)
					{
						if (GetMovementComponent()->IsFalling())
						{
							SetAniState(SekiroState::JumpLoop);
						}
						else
						{
							SetAniState(SekiroState::Idle);
						}
					}

					GetWorld()->GetTimerManager().ClearTimer(myTimerHandle);
				}), delayTime, false);
		}
		else
		{
			if (AniStateValue == SekiroState::Guard)
			{
				if (GetMovementComponent()->IsFalling())
				{
					SetAniState(SekiroState::JumpLoop);
				}
				else
				{
					SetAniState(SekiroState::Idle);
				}
			}

			if (bGuardTimer == false && HitState == PlayerHitState::GUARD)
			{
				HitState = PlayerHitState::OFFGUARD;
			}
		}
	}
}

void APlayerSekiro::ManageParryingTimer()
{
	if (HitState == PlayerHitState::PARRYING)
	{
		HitState = PlayerHitState::GUARD;

		float GuardTime = MaxGuardValidTime - ParryingValidTime;

		GetWorld()->GetTimerManager().SetTimer(GuardTimerHandle, this, &APlayerSekiro::ManageGuardTimer, GuardTime, false);
	}
}

void APlayerSekiro::ManageGuardTimer()
{
	bGuardTimer = false;

	if (HitState == PlayerHitState::GUARD)
	{
		HitState = PlayerHitState::OFFGUARD;
	}
}

void APlayerSekiro::StartedPlayerAttack()
{
	// 선입력이 있는 경우 return
	if (BufferedAction != SekiroState::None)
	{
		return;
	}

	// 연타 제한 시간
	if (bAttackValid)
	{
		bAttackValid = false;

		float delayTime = 0.1;
		FTimerHandle myTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(myTimerHandle, FTimerDelegate::CreateLambda([&]()
			{
				bAttackValid = true;

				GetWorld()->GetTimerManager().ClearTimer(myTimerHandle);
			}), delayTime, false);
	}
	else
	{
		return;
	}
	
	SekiroState AniStateValue = GetAniState<SekiroState>();

	if (AniStateValue != SekiroState::Idle && AniStateValue != SekiroState::Guard
		&& AniStateValue != SekiroState::JumpStart && AniStateValue != SekiroState::JumpLoop
		
		&& AniStateValue != SekiroState::ForwardWalk && AniStateValue != SekiroState::BackwardWalk
		&& AniStateValue != SekiroState::LeftWalk && AniStateValue != SekiroState::RightWalk
		&& AniStateValue != SekiroState::ForwardRun && AniStateValue != SekiroState::BackwardRun
		&& AniStateValue != SekiroState::LeftRun && AniStateValue != SekiroState::RightRun
		
		&& AniStateValue != SekiroState::BasicAttack1 && AniStateValue != SekiroState::BasicAttack2
		&& AniStateValue != SekiroState::BasicAttack3 && AniStateValue != SekiroState::DashAttack 
		&& AniStateValue != SekiroState::StabAttack2 && AniStateValue != SekiroState::LightningReversal1
		)
	{
		return;
	}

	// 콤보 공격 조건에 맞지 않을 시 return
	if (AniStateValue == SekiroState::BasicAttack1 || AniStateValue == SekiroState::BasicAttack2
		|| AniStateValue == SekiroState::BasicAttack3 || AniStateValue == SekiroState::DashAttack
		|| AniStateValue == SekiroState::StabAttack2
		)
	{
		if (bAttackCombo && BufferedAction == SekiroState::None)
		{
			if (AniStateValue != SekiroState::StabAttack2 && AniStateValue != SekiroState::DashAttack)
			{
				// 이전 평타 상태
				// BasicAttack1 일 경우 BasicAttackCount = 0
				BasicAttackCount = (int)GetAniState<SekiroState>() - 10;

				// 공격 상태가 지속 중일 때 평타1, 2, 3 순서대로 전환
				// 평타3 이후엔 다시 1로 전환
				++BasicAttackCount;
				if (BasicAttackCount > 2)
				{
					BasicAttackCount = 0;
				}
			}
			else
			{
				BasicAttackCount = 0;
			}

			// Transition 노티파이 이전에 키입력을 하면 선입력 버퍼에 입력
			// Transition 노티파이 이후에 키입력을 하면 공격 바로 실행
			if (bEnteredTransition == false)
			{
				CorrectedTime = GetWorld()->GetTimeSeconds();
				BufferedAction = (SekiroState)((int)SekiroState::BasicAttack1 + BasicAttackCount);
				bAttackEnable = true;

				return;
			}
		}
		else
		{
			return;
		}
	}
	else
	{
		BasicAttackCount = 0;
	}
	
	if (AniStateValue == SekiroState::LightningReversal1)
	{
		if (bEnteredTransition == false)
		{
			CorrectedTime = GetWorld()->GetTimeSeconds();
			BufferedAction = SekiroState::LightningReversal2;
		}
		else
		{
			SetAniState(SekiroState::LightningReversal2);
		}

		return;
	}
	else if (AniStateValue == SekiroState::JumpStart || AniStateValue == SekiroState::JumpLoop || GetMovementComponent()->IsFalling())
	{
		SetAniState(SekiroState::JumpAttack);

		return;
	}
	else if (AniStateValue == SekiroState::ForwardRun || AniStateValue == SekiroState::BackwardRun
		|| AniStateValue == SekiroState::LeftRun || AniStateValue == SekiroState::RightRun)
	{
		SetAniState(SekiroState::DashAttack);
		bDashAttackMove = true;

		GetWorld()->GetTimerManager().SetTimer(DashAttackMoveTimerHandle, this, &APlayerSekiro::DashAttackMove, 0.001f, true);

		return;
	}

	bAttackEnable = true;
	SetAniState(SekiroState::StabAttack1);
}

void APlayerSekiro::TriggeredPlayerAttack(bool ActionValue, float TriggeredSec)
{
	// 공격은 클릭 당 한 번만 가능
	if (bAttackEnable == false)
	{
		return;
	}

	else
	{
		if (ActionValue)
		{
			if (BufferedAction != SekiroState::None)
			{
				return;
			}

			// 선입력 시간 보정
			if (TriggeredSec > 0.6f + CorrectedTime)
			{
				SetAniState(SekiroState::StabAttack2);

				GetWorld()->GetTimerManager().SetTimer(AttackMoveTimerHandle, this, &APlayerSekiro::AttackMove, 0.1f, false);

				CorrectedTime = 0.f;
				bAttackEnable = false;
			}
		}
		else
		{
			if ((BufferedAction == SekiroState::BasicAttack1 || BufferedAction == SekiroState::BasicAttack2
				|| BufferedAction == SekiroState::BasicAttack3) && bBufferedCompletedAttack == false)
			{
				bBufferedCompletedAttack = true;
			}
			else
			{
				if (TriggeredSec <= 0.6f + CorrectedTime)
				{
					SetAniState((int)SekiroState::BasicAttack1 + BasicAttackCount);

					GetWorld()->GetTimerManager().SetTimer(AttackMoveTimerHandle, this, &APlayerSekiro::AttackMove, 0.15f, false);
				}
			}

			bAttackEnable = false;
		}
	}
}

void APlayerSekiro::DashAttackMove()
{
	if (bDashAttackMove)
	{
		AddMovementInput(GetActorForwardVector(), 1.f);
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(DashAttackMoveTimerHandle);
	}
}

// 공격 시 캐릭터가 전진하는 기능
void APlayerSekiro::AttackMove()
{
	float AttackMoveImpulse = 0.f;

	SekiroState AniStateValue = GetAniState<SekiroState>();

	if (AniStateValue == SekiroState::BasicAttack1 || AniStateValue == SekiroState::BasicAttack2
		|| AniStateValue == SekiroState::BasicAttack3)
	{
		AttackMoveImpulse = 3000.f;
	}
	else if (AniStateValue == SekiroState::StabAttack2)
	{
		AttackMoveImpulse = 6000.f;
	}

	GetCharacterMovement()->AddImpulse(GetActorForwardVector() * AttackMoveImpulse, true);
}

// 락온 가능한 대상을 찾아 카메라 시점을 고정하도록 하는 함수
void APlayerSekiro::LockOnTarget()
{
	if (IsDeath())
	{
		return;
	}

	SekiroState AniStateValue = GetAniState<SekiroState>();

	if (AniStateValue == SekiroState::SitStart || AniStateValue == SekiroState::SitEnd)
	{
		return;
	}
	
	if (bLockOn == false)
	{
		// Monster 콜리전 탐색
		EObjectTypeQuery ObjectType = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel2);
		TArray<AActor*> HitActor = TraceObjects(ObjectType, CameraComponent->GetForwardVector(), 30.f, LockOnRange, 200.f);

		float ClosestDist = LockOnRange;
		AActor* ClosestHitActor = nullptr;

		// 가장 가까운 적 탐색(Num 값이 0일 경우 자동 종료)
		for (size_t i = 0; i < HitActor.Num(); i++)
		{
			FVector LockedOnLocation = HitActor[i]->GetActorLocation();
			float Dis = (GetActorLocation() - LockedOnLocation).Size();
			
			if (Dis < ClosestDist)
			{
				ClosestDist = Dis;
				ClosestHitActor = HitActor[i];
			}
		}

		if (ClosestHitActor != nullptr)
		{
			LockedOnTarget = ClosestHitActor;
			ToggleLockOn();
		}
	}
	// 휠 키를 누를 때마다 락온 상태 토글
	else
	{
		ToggleLockOn();
	}
}

// 현재의 락온 대상을 기준으로 X축 상에서 가장 가까운 적을 재탐색(실제 거리 무관, 각도로만 판단)
// 마우스의 Action Value 중 X값(Rate)을 받아와 부호에 따라 탐색 방향 결정(-, + 방향)
void APlayerSekiro::ResearchLockOnTarget(float Rate)
{
	SekiroState AniStateValue = GetAniState<SekiroState>();

	if (AniStateValue == SekiroState::SitStart || AniStateValue == SekiroState::SitEnd)
	{
		return;
	}

	// 마우스의 움직임이 일정 값 이상일 경우에만 함수 실행(적절한 감도 설정)
	if (FMath::Abs(Rate) < 3.f)
	{
		return;
	}

	if (bResearchEnable)
	{
		bResearchEnable = false;

		TArray<AActor*> ActorsToNotTargeting;
		ActorsToNotTargeting.Add(this);
		ActorsToNotTargeting.Add(LockedOnTarget);

		EObjectTypeQuery ObjectType = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel2);
		TArray<AActor*> HitActor = TraceObjects(ObjectType, ActorsToNotTargeting, CameraComponent->GetForwardVector(), 30.f, 2500.f, 200.f);

		// 플레이어와 현재 타겟 사이의 단위 벡터(각도의 크기를 판단할 기준 단위 벡터)
		FVector MiddleUnitVector = (LockedOnTarget->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		float FinalAngle = 180.f;

		AActor* ClosestHitActor = nullptr;

		for (size_t i = 0; i < HitActor.Num(); i++)
		{
			FVector HitActorUnitVector = (HitActor[i]->GetActorLocation() - GetActorLocation()).GetSafeNormal();
			float Dot = FVector::DotProduct(MiddleUnitVector, HitActorUnitVector);
			float Angle = FMath::RadiansToDegrees(FMath::Acos(Dot));							// 두 벡터 사이의 각도

			FVector crossPrdt = FVector::CrossProduct(MiddleUnitVector, HitActorUnitVector);	// Z 값 : 각도의 방향

			// MiddleUnitVector를 기준으로 Rate 방향에 따른 각도 차이가 가장 적게 나는 액터를 탐색
			// 플레이어와의 거리는 고려하지 않고, 각도로만 판단
			if (((Rate > 0.f && crossPrdt.Z > 0) || (Rate < 0.f && crossPrdt.Z < 0))
				&& Angle < FinalAngle)
			{
				FinalAngle = Angle;
				ClosestHitActor = HitActor[i];
			}
		}

		if (ClosestHitActor != nullptr)
		{
			ToggleLockOn();		// 기존 락온 해제
			LockedOnTarget = ClosestHitActor;
			ToggleLockOn();		// 새로운 대상에게 락온
		}

		// delayTime 뒤 bResearchEnable 값을 다시 true로 변환
		// 함수가 한 번에 여러 번 호출되지 않도록 타이머로 제한
		float delayTime = 0.2f;
		FTimerHandle myTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(myTimerHandle, FTimerDelegate::CreateLambda([&]()
			{
				// 내가 원하는 코드 구현
				bResearchEnable = true;

				// 타이머 초기화
				GetWorld()->GetTimerManager().ClearTimer(myTimerHandle);
			}), delayTime, false); // 반복 실행을 하고 싶으면 false 대신 true 대입
	}
}

void APlayerSekiro::ToggleLockOn()
{
	GetCharacterMovement()->bOrientRotationToMovement = bLockOn;

	if (IsValid(Cast<AMonster>(LockedOnTarget)))
	{
		Cast<AMonster>(LockedOnTarget)->LockOnIconOnOff(!bLockOn);
	}

	bLockOn = !bLockOn;
}

void APlayerSekiro::SearchDeathblowTarget()
{
	// 범위에 있는 Monster 콜리전 탐색
	EObjectTypeQuery ObjectType = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel2);
	TArray<AActor*> HitActor = TraceObjects(ObjectType, GetActorForwardVector(), 45.f, 50.f, 70.f);

	if (HitActor.Num() == 0)
	{
		return;
	}

	float ClosestDist = FLT_MAX;
	AMonster* ClosestTarget = nullptr;

	// 가장 가까운 적 탐색(Num 값이 0일 경우 자동 종료)
	for (size_t i = 0; i < HitActor.Num(); i++)
	{
		AMonster* Target = Cast<AMonster>(HitActor[i]);
		FVector LockedOnLocation = Target->GetActorLocation();
		float Dis = (GetActorLocation() - LockedOnLocation).Size();
		
		if (Target->bEnableDeathblow && Dis < ClosestDist)
		{
			ClosestDist = Dis;
			ClosestTarget = Target;
		}
	}

	if (ClosestTarget != nullptr)
	{
		ClearBuffer();
		
		AdjustAngle(ClosestTarget->GetActorLocation());
		HitState = PlayerHitState::INVINCIBLE;

		UGameplayStatics::ApplyDamage(ClosestTarget, this->Power, GetController(), this, UDeathblowType::StaticClass());

		if (ClosestTarget->ActorHasTag(TEXT("보스")))
		{
			SetAniState(SekiroState::DeathblowBoss);
		}
		else
		{
			SetAniState(SekiroState::DeathblowNormal);
		}
	}
}

void APlayerSekiro::DrinkGourd()
{
	SekiroState AniStateValue = GetAniState<SekiroState>();

	if (AniStateValue != SekiroState::Idle

		&& AniStateValue != SekiroState::ForwardWalk && AniStateValue != SekiroState::BackwardWalk
		&& AniStateValue != SekiroState::LeftWalk && AniStateValue != SekiroState::RightWalk
		&& AniStateValue != SekiroState::ForwardRun && AniStateValue != SekiroState::BackwardRun
		&& AniStateValue != SekiroState::LeftRun && AniStateValue != SekiroState::RightRun)
	{
		return;
	}

	if (HealCount <= 0)
	{
		return;
	}

	GourdMesh->SetStaticMesh(StaticMeshArrays[1]);
	SetAniState(SekiroState::Heal);
}

void APlayerSekiro::GourdHeal()
{
	if (HealCount <= 0)
	{
		return;
	}
	
	HP += MaxHP * 0.5;

	if (HP >= MaxHP)
	{
		HP = MaxHP;
	}

	HealCount -= 1;
}

void APlayerSekiro::SitDown()
{
	SekiroState AniStateValue = GetAniState<SekiroState>();

	if (AniStateValue != SekiroState::Idle

		&& AniStateValue != SekiroState::ForwardWalk && AniStateValue != SekiroState::BackwardWalk
		&& AniStateValue != SekiroState::LeftWalk && AniStateValue != SekiroState::RightWalk
		&& AniStateValue != SekiroState::ForwardRun && AniStateValue != SekiroState::BackwardRun
		&& AniStateValue != SekiroState::LeftRun && AniStateValue != SekiroState::RightRun)
	{
		return;
	}

	// 범위에 있는 Buddha 콜리전 탐색
	EObjectTypeQuery ObjectType = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel5);
	TArray<AActor*> HitActor = TraceObjects(ObjectType, GetActorForwardVector(), 45.f, 100.f, 70.f);

	if (HitActor.Num() == 0)
	{
		return;
	}

	float ClosestDist = FLT_MAX;
	ABuddha* ClosestTarget = nullptr;

	for (size_t i = 0; i < HitActor.Num(); i++)
	{
		ABuddha* Target = Cast<ABuddha>(HitActor[i]);
		FVector LockedOnLocation = Target->GetActorLocation();
		float Dis = (GetActorLocation() - LockedOnLocation).Size();

		if (Target->bTriggerEvent && Dis < ClosestDist)
		{
			ClosestDist = Dis;
			ClosestTarget = Target;
		}
	}

	if (ClosestTarget != nullptr)
	{
		ClearBuffer();

		AdjustAngle(ClosestTarget->GetActorLocation());

		if (bLockOn)
		{
			ToggleLockOn();	// 기존 락온이 있을 경우 해제
		}
		LockedOnTarget = ClosestTarget;
		ToggleLockOn();		// 새로운 대상에게 락온

		SetAniState(SekiroState::SitStart);

		float delayTime = 3.5f;
		FTimerHandle myTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(myTimerHandle, FTimerDelegate::CreateLambda([&]()
			{
				SekiroState AniStateValue = GetAniState<SekiroState>();

				// 도중에 공격 받았을 시 불상 UI 표시하지 않음
				if (AniStateValue != SekiroState::SitStart && AniStateValue != SekiroState::SitLoop)
				{
					return;
				}
				
				UGlobalGameInstance* Inst = GetGameInstance<UGlobalGameInstance>();
				TSubclassOf<UUserWidget> WidgetClass = Inst->GetWidgetClassData(TEXT("Buddha"), TEXT("Menu"));

				if (IsValid(WidgetClass))
				{
					UUserWidget* BuddhaMenuWidget = Cast<UUserWidget>(CreateWidget(GetWorld(), WidgetClass));

					if (IsValid(BuddhaMenuWidget))
					{
						BuddhaMenuWidget->AddToViewport();
					}
				}

				GetWorld()->GetTimerManager().ClearTimer(myTimerHandle);
			}), delayTime, false);
	}
}

void APlayerSekiro::BuddhaRest()
{
	HP = MaxHP;
	HealCount = MaxHealCount;

	UGlobalGameInstance* Inst = GetGameInstance<UGlobalGameInstance>();
	USoundBase* HealSound = Inst->GetSoundData(TEXT("Player"), TEXT("Heal"));
	if (IsValid(HealSound))
	{
		UGameplayStatics::PlaySound2D(GetWorld(), HealSound);
	}

	TSubclassOf<UObject> HealEffect = Inst->GetEffect(TEXT("PlayerHeal"));
	if (IsValid(HealEffect))
	{
		AActor* Effect = GetWorld()->SpawnActor<AActor>(HealEffect);
		Effect->SetActorLocation(GetActorLocation());
		Effect->SetActorRotation(GetActorRotation());
		Effect->AddActorLocalOffset(FVector(0.f, 0.f, -100.f));

		// DestroyTime 뒤 이펙트 액터 삭제
		float DestroyTime = 3.f;
		FTimerHandle myTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(myTimerHandle, FTimerDelegate::CreateLambda([&, Effect]()
			{
				Effect->Destroy();

				GetWorld()->GetTimerManager().ClearTimer(myTimerHandle);
			}), DestroyTime, false);
	}
}

void APlayerSekiro::MontageBlendingOut(UAnimMontage* Anim, bool _Inter)
{
	bEnteredTransition = false;
	bAttackCombo = false;
	
	if (Anim == GetAnimMontage(SekiroState::JumpAttack))
	{
		// 점프 공격 후 여전히 공중이면 점프 상태로, 땅에 닿았을 경우 idle로 전환
		if (GetMovementComponent()->IsFalling())
		{
			SetAniState(SekiroState::JumpLoop);
		}
		else
		{
			SetAniState(SekiroState::Idle);
		}
	}
	else if(Anim == GetAnimMontage(SekiroState::Hit) || Anim == GetAnimMontage(SekiroState::LightningReversal2)
		|| Anim == GetAnimMontage(SekiroState::Parrying1) || Anim == GetAnimMontage(SekiroState::Parrying2))
	{
		SetAniState(SekiroState::Idle);
	}
	else if (Anim == GetAnimMontage(SekiroState::Knockdown))
	{
		SetAniState(SekiroState::Getup);
	}
	else if (Anim == GetAnimMontage(SekiroState::Getup) || Anim == GetAnimMontage(SekiroState::MikiriCounter)
		|| Anim == GetAnimMontage(SekiroState::DeathblowNormal) || Anim == GetAnimMontage(SekiroState::DeathblowBoss))
	{
		HitState = PlayerHitState::OFFGUARD;
		SetAniState(SekiroState::Idle);
	}
	else if (Anim == GetAnimMontage(SekiroState::Blocked))
	{
		float delayTime = 0.3f;
		FTimerHandle myTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(myTimerHandle, FTimerDelegate::CreateLambda([&]()
			{
				SetAniState(SekiroState::Idle);

				GetWorld()->GetTimerManager().ClearTimer(myTimerHandle);
			}), delayTime, false);
	}
	else if (Anim == GetAnimMontage(SekiroState::Heal))
	{
		GourdMesh->SetStaticMesh(nullptr);
	}
	else if (Anim == GetAnimMontage(SekiroState::SitEnd))
	{
		if (bLockOn)
		{
			ToggleLockOn();	// 기존 락온이 있을 경우 해제
		}
	}
}

void APlayerSekiro::MontageEnd()
{
	SekiroState AniStateValue = GetAniState<SekiroState>();

	if (AniStateValue == SekiroState::JumpEnd)
	{
		SetAniState(SekiroState::Idle);
	}

	else if (AniStateValue == SekiroState::JumpStart)
	{
		SetAniState(SekiroState::JumpLoop);
	}

	else if (AniStateValue == SekiroState::LightningReversal1)
	{
		SetAniState(SekiroState::Idle);
		GetHitExecute(SavedDamage, Cast<UElectricSlashType>(UElectricSlashType::StaticClass()->GetDefaultObject()), nullptr);
		SavedDamage = 0.f;
	}

	else if (AniStateValue == SekiroState::Heal)
	{
		SetAniState(SekiroState::Idle);
	}

	else if (AniStateValue == SekiroState::SitStart)
	{
		SetAniState(SekiroState::SitLoop);
	}

	else
	{
		SetAniState(SekiroState::Idle);
	}
}

void APlayerSekiro::AttackBegin()
{
	bEnteredTransition = false;
}

void APlayerSekiro::AttackEnd()
{
	SetAniState(SekiroState::Idle);
}

void APlayerSekiro::AttackComboBegin()
{
	bAttackCombo = true;
}

void APlayerSekiro::AttackComboEnd()
{
	bAttackCombo = false;
}

void APlayerSekiro::DashAttackMoveEnd()
{
	bDashAttackMove = false;
}

void APlayerSekiro::DeathblowRecover()
{
	HP += MaxHP * 0.2f;
	Posture += MaxPosture * 0.3f;

	if (HP >= MaxHP)
	{
		HP = MaxHP;
	}

	if (Posture >= MaxPosture)
	{
		Posture = MaxPosture;
	}
}

// 선입력 체크
void APlayerSekiro::CheckBufferedInput()
{
	if (BufferedAction != SekiroState::None)
	{
		// 평타 선입력
		if (BufferedAction == SekiroState::BasicAttack1 || BufferedAction == SekiroState::BasicAttack2
			|| BufferedAction == SekiroState::BasicAttack3)
		{
			// 선입력 버퍼에 있는 동안 키를 뗐을 경우 바로 평타 실행
			if (bBufferedCompletedAttack)
			{
				SetAniState(BufferedAction);

				GetWorld()->GetTimerManager().SetTimer(AttackMoveTimerHandle, this, &APlayerSekiro::AttackMove, 0.15f, false);

				CorrectedTime = 0.f;

				bBufferedCompletedAttack = false;
			}
			else
			{
				// 선입력이 있을 때 아직 키를 누르고 있는 상태일 경우 Triggered Seconds 보정
				SetAniState(SekiroState::StabAttack1);

				CorrectedTime = GetWorld()->GetTimeSeconds() - CorrectedTime;
			}
		}

		// 점프 선입력
		else if (BufferedAction == SekiroState::JumpStart)
		{
			SetAniState(SekiroState::Idle);
			PlayerJump();
		}

		// 대쉬 선입력
		else if (BufferedAction == SekiroState::ForwardRun)
		{
			SetAniState(SekiroState::Idle);
			StartedPlayerDash();

			if (bBufferedCompletedDash)
			{
				CorrectedTime = 0.f;
				TriggeredPlayerDash(false, 0.f);

				bBufferedCompletedDash = false;
			}
			else
			{
				CorrectedTime = GetWorld()->GetTimeSeconds() - CorrectedTime;
			}
		}

		// 가드 선입력
		else if (BufferedAction == SekiroState::Guard)
		{
			SetAniState(SekiroState::Idle);
			StartedPlayerGuard();

			if (bBufferedCompletedGuard)
			{
				CorrectedTime = 0.f;
				TriggeredPlayerGuard(false, 0.f);

				bBufferedCompletedGuard = false;
			}
			else
			{
				CorrectedTime = GetWorld()->GetTimeSeconds() - CorrectedTime;
			}
		}
		
		// 뇌반 선입력
		else if (BufferedAction == SekiroState::LightningReversal2)
		{
			SetAniState(SekiroState::LightningReversal2);
		}

		BufferedAction = SekiroState::None;
	}

	bEnteredTransition = true;
}

void APlayerSekiro::ClearBuffer()
{
	BufferedAction = SekiroState::None;
	CorrectedTime = 0.f;

	bAttackEnable = false;
	bDashAttackMove = false;
	Speed = DefaultSpeed;
	bDash = false;

	bBufferedCompletedAttack = false;
	bBufferedCompletedDash = false;
	bBufferedCompletedGuard = false;
}