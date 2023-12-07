// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerSekiro.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "../Global/GlobalAnimInstance.h"
#include "../Global/GlobalGameInstance.h"
#include "../Global/Data/PlayerAnimData.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"


APlayerSekiro::APlayerSekiro()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	Tags.Add(TEXT("Player"));

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->TargetArmLength = 1500.0f;
	SpringArmComponent->bDoCollisionTest = true;
	SpringArmComponent->SetupAttachment(RootComponent);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->FieldOfView = 45.0f;
	CameraComponent->SetupAttachment(SpringArmComponent);

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(GetMesh(), TEXT("hand_r_weapon"));

	GourdMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GourdMesh"));
	GourdMesh->SetupAttachment(GetMesh(), TEXT("Gourd"));
}

void APlayerSekiro::BeginPlay()
{
	Super::BeginPlay();

	// 애니메이션 종료 시 MontageEnd를 콜백한다.
	GetGlobalAnimInstance()->OnMontageBlendingOut.AddDynamic(this, &APlayerSekiro::MontageEnd);
	//GetGlobalAnimInstance()->OnPlayMontageNotifyBegin.AddDynamic(this, &APlayerSekiro::AnimNotifyBegin);
	//GetGlobalAnimInstance()->OnPlayMontageNotifyEnd.AddDynamic(this, &APlayerSekiro::AnimNotifyEnd);

	SetAniState(SekiroState::Idle);

	// 선입력 버퍼 초기화
	// 선입력 버퍼에 들어갈 수 있는 값 : None, BasicAttack(1, 2, 3), Jump, Dash, Guard
	BufferedAction = SekiroState::None;

	UGlobalGameInstance* Inst = GetGameInstance<UGlobalGameInstance>();

	// 캐릭터 기본 스탯 설정
	StatData = Inst->GetPlayerStat(TEXT("Sekiro"));
	HP = StatData->HP;
	MaxHP = StatData->MaxHP;
	Posture = StatData->Posture;
	Power = StatData->Power;

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

	// 락온 상태가 아닐 때 움직임을 Forward로 고정하기 위한 설정값.
	// 락온 상태일 땐 false로 변경해야 한다.
	GetCharacterMovement()->bOrientRotationToMovement = true;

	// 중력, 점프 높이 설정
	GetCharacterMovement()->GravityScale = 1.8f;
	GetCharacterMovement()->JumpZVelocity = 800.0f;
}

void APlayerSekiro::Tick(float _Delta)
{
	Super::Tick(_Delta);

	SekiroState AniStateValue = GetAniState<SekiroState>();
	UCharacterMovementComponent* Move = Cast<UCharacterMovementComponent>(GetMovementComponent());

	if (AniStateValue == SekiroState::ForwardWalk)
	{
		Move->MaxWalkSpeed = Speed;
	}
	else
	{
		Move->MaxWalkSpeed = Speed * 0.7f;
	}

	if (bLockOn)
	{
		// 락온 범위를 벗어나면 락온 자동 해제
		FVector LockedOnLocation = LockedOnTarget->GetActorLocation();
		float Dis = (GetActorLocation() - LockedOnLocation).Size();
		if (Dis > LockOnRange)
		{
			ToggleLockOn();
			return;
		}

		LockedOnLocation.Z -= 150.0f;

		const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), LockedOnLocation);
		const FRotator InterpRotation = UKismetMathLibrary::RInterpTo(GetController()->GetControlRotation(), LookAtRotation, _Delta, 10.f);
		
		// 프레임 마다 캐릭터와 카메라의 시점 최신화
		SetActorRotation(FRotator(0.f, InterpRotation.Yaw, 0.f));
		GetController()->SetControlRotation(InterpRotation);
	}
}

void APlayerSekiro::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
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

			AddMovementInput(FRotationMatrix(ControlSpaceRot).GetScaledAxis(EAxis::X), Val);

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
			AddMovementInput(FRotationMatrix(ControlSpaceRot).GetScaledAxis(EAxis::Y), Val);

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

	if (AniStateValue == SekiroState::JumpStart || AniStateValue == SekiroState::JumpLoop)
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
		// 첫 대쉬 가속도 보정
		if (AniStateValue == SekiroState::ForwardRun)
		{
			GetCharacterMovement()->AddImpulse(GetActorForwardVector() * 10.0f, true);
		}

		// 처음 대쉬 시 방향키 입력이 없는 경우 0.5초간 전방 대쉬
		// 도중에 키 입력이 들어오면 적용하지 않음
		if (bInputWASD == false && (AniStateValue == SekiroState::Idle || AniStateValue == SekiroState::ForwardRun))
		{
			AddMovementInput(GetActorForwardVector(), 1.0f);
			SetAniState(SekiroState::ForwardRun);
		}
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(StartedDashTimerHandle);
	}
}

void APlayerSekiro::TriggeredPlayerDash(bool ActionValue, float TriggeredSec)
{
	if (ActionValue)
	{
		Speed = 1800.0f;
		bDash = ActionValue;
	}
	else
	{
		if (BufferedAction != SekiroState::None && bBufferedCompletedDash == false)
		{
			bBufferedCompletedDash = true;
			return;
		}
		// 대쉬 키를 눌렀다가 바로 떼어도 최소한 0.5초 동안은 대쉬 상태를 유지
		// 단, bStartedDash 값이 true 일 때만 적용
		float MinDashTime = 0.5f;

		if (bStartedDash)
		{
			float delayTime = MinDashTime + CorrectedTime;
			CorrectedTime = 0.f;

			FTimerHandle myTimerHandle;
			GetWorld()->GetTimerManager().SetTimer(myTimerHandle, FTimerDelegate::CreateLambda([&]()
				{
					Speed = 500.0f;
					bDash = false;

					GetWorld()->GetTimerManager().ClearTimer(myTimerHandle);
				}), delayTime, false);
		}
		else
		{
			Speed = 500.0f;
			bDash = ActionValue;
		}
	}
}

void APlayerSekiro::StartedPlayerGuard()
{
	SekiroState AniStateValue = GetAniState<SekiroState>();

	if (AniStateValue != SekiroState::Idle && AniStateValue != SekiroState::Guard
		&& AniStateValue != SekiroState::JumpLoop

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


	// 가드 키를 0.2초 내에 연속으로 눌렀을 경우 점차 패링 판정 시간 감소(0.05초씩 감소)
	// 가드 키를 연타하여 패링 판정을 쉽게 취하지 못하도록 하기 위함
	float CurGuardTime = GetWorld()->GetTimeSeconds();
	float IntervalTime = CurGuardTime - PreGuardTime;

	if (PreGuardTime != 0.f && IntervalTime <= 0.2f)
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
		if (AniStateValue == SekiroState::Idle || AniStateValue == SekiroState::Guard || AniStateValue == SekiroState::JumpLoop
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
		if (BufferedAction != SekiroState::None && bBufferedCompletedGuard == false)
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
						SetAniState(SekiroState::Idle);
					}

					GetWorld()->GetTimerManager().ClearTimer(myTimerHandle);
				}), delayTime, false);
		}
		else
		{
			if (AniStateValue == SekiroState::Guard)
			{
				SetAniState(SekiroState::Idle);
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

// 락온 가능한 대상을 찾아 카메라 시점을 고정하도록 하는 함수
void APlayerSekiro::LockOnTarget()
{
	if (bLockOn == false)
	{
		// 락온 제외 액터
		TArray<AActor*> ActorsToNotTargeting;
		ActorsToNotTargeting.Add(this);

		TArray<AActor*> HitActor = TraceObjects(ActorsToNotTargeting);

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

		if (ClosestHitActor)
		{
			LockedOnTarget = Cast<AMonster>(ClosestHitActor);
			ToggleLockOn();
		}
	}
	// 휠 키를 누를 때마다 락온 상태 토글
	else
	{
		ToggleLockOn();
	}
}

// 화면 시점 기준으로 전방의 몬스터를 찾는 함수
TArray<AActor*> APlayerSekiro::TraceObjects(TArray<AActor*> _ActorsToNotTargeting)
{
	FVector CameraForwardVector = CameraComponent->GetForwardVector();

	// 락온 대상 지정
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypeToLock;
	// Monster 콜리전 프리셋은 ECC_GameTraceChannel2 콜리전 채널을 사용하고 있다.
	// 즉 Monster 콜리전 프리셋을 사용하는 액터를 찾겠다는 의미
	EObjectTypeQuery ObjectType = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel2);
	ObjectTypeToLock.Emplace(ObjectType);

	FHitResult HitResult;
	TArray<AActor*> HitActor;

	FVector StartPoint = GetActorLocation();

	// perspective 관점으로 탐색
	for (size_t i = 0; i < LockOnAngle * 2; i += 5)
	{
		for (size_t j = 0; j < LockOnAngle; j += 5)
		{
			FVector DirectionX = CameraForwardVector.RotateAngleAxis(-LockOnAngle * 0.5 + j, FVector::LeftVector);
			FVector DirectionY = DirectionX.RotateAngleAxis(-LockOnAngle + i, FVector::UpVector);
			FVector EndPoint = StartPoint + DirectionY * LockOnRange;

			bool bIsHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
				GetWorld(), StartPoint, EndPoint, 200.f,
				ObjectTypeToLock, false, _ActorsToNotTargeting, EDrawDebugTrace::None,
				HitResult, true);

			// 탐색한 액터들은 중복값을 제외하고 HitActor에 추가
			if (bIsHit)
			{
				if (HitActor.Num() == 0)
				{
					HitActor.Add(HitResult.GetActor());
				}
				else
				{
					for (size_t k = 0; k < HitActor.Num(); k++)
					{
						if (HitActor[k] == HitResult.GetActor())
						{
							break;
						}

						if (k == HitActor.Num() - 1)
						{
							HitActor.Add(HitResult.GetActor());
						}
					}
				}
			}
		}
	}

	return HitActor;
}

// 현재의 락온 대상을 기준으로 X축 상에서 가장 가까운 적을 재탐색(실제 거리 무관, 각도로만 판단)
// 마우스의 Action Value 중 X값(Rate)을 받아와 부호에 따라 탐색 방향 결정(-, + 방향)
void APlayerSekiro::ResearchLockOnTarget(float Rate)
{
	// 마우스의 움직임이 일정 값 이상일 경우에만 함수 실행(적절한 감도 설정)
	if (FMath::Abs(Rate) < 3.0f)
	{
		return;
	}

	if (bResearchEnable)
	{
		bResearchEnable = false;

		TArray<AActor*> ActorsToNotTargeting;
		ActorsToNotTargeting.Add(this);
		ActorsToNotTargeting.Add(LockedOnTarget);

		TArray<AActor*> HitActor = TraceObjects(ActorsToNotTargeting);

		// 플레이어와 현재 타겟 사이의 단위 벡터(각도의 크기를 판단할 기준 단위 벡터)
		FVector MiddleUnitVector = (LockedOnTarget->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		float FinalAngle = 180.0f;

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

		if (ClosestHitActor)
		{
			ToggleLockOn();		// 기존 락온 해제
			LockedOnTarget = Cast<AMonster>(ClosestHitActor);
			ToggleLockOn();		// 새로운 대상에게 락온
		}

		// delayTime 뒤 bResearchEnable 값을 다시 true로 변환
		// 함수가 한 번에 여러 번 호출되지 않도록 타이머로 제한
		float delayTime = 0.2;
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

	LockedOnTarget->LockOnIconOnOff(!bLockOn);
	bLockOn = !bLockOn;
}

void APlayerSekiro::StartedPlayerAttack()
{
	// 선입력이 있는 경우 return
	if (BufferedAction != SekiroState::None)
	{
		return;
	}

	// 연타 최소 시간 0.2초로 제한
	if (bAttackValid)
	{
		bAttackValid = false;

		float delayTime = 0.2;
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

	if (AniStateValue != SekiroState::Idle && AniStateValue != SekiroState::Guard && AniStateValue != SekiroState::JumpLoop
		
		&& AniStateValue != SekiroState::ForwardWalk && AniStateValue != SekiroState::BackwardWalk
		&& AniStateValue != SekiroState::LeftWalk && AniStateValue != SekiroState::RightWalk
		&& AniStateValue != SekiroState::ForwardRun && AniStateValue != SekiroState::BackwardRun
		&& AniStateValue != SekiroState::LeftRun && AniStateValue != SekiroState::RightRun
		
		&& AniStateValue != SekiroState::BasicAttack1 && AniStateValue != SekiroState::BasicAttack2
		&& AniStateValue != SekiroState::BasicAttack3 && AniStateValue != SekiroState::DashAttack 
		&& AniStateValue != SekiroState::StabAttack2
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

	bAttackEnable = true;

	if (AniStateValue == SekiroState::JumpLoop || GetCharacterMovement()->IsFalling())
	{
		SetAniState(SekiroState::JumpAttack);
		bAttackEnable = false;

		return;
	}
	else if (AniStateValue == SekiroState::ForwardRun || AniStateValue == SekiroState::BackwardRun
		|| AniStateValue == SekiroState::LeftRun || AniStateValue == SekiroState::RightRun)
	{
		SetAniState(SekiroState::DashAttack);
		bDashAttackMove = true;
		bAttackEnable = false;

		GetWorld()->GetTimerManager().SetTimer(DashAttackMoveTimerHandle, this, &APlayerSekiro::DashAttackMove, 0.001f, true);

		return;
	}

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
			if (BufferedAction != SekiroState::None && bBufferedCompletedAttack == false)
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
		AddMovementInput(GetActorForwardVector(), 1.0f);
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
		AttackMoveImpulse = 3000.0f;
	}
	else if (AniStateValue == SekiroState::StabAttack2)
	{
		AttackMoveImpulse = 8000.0f;
	}

	GetCharacterMovement()->AddImpulse(GetActorForwardVector() * AttackMoveImpulse, true);
}

void APlayerSekiro::MontageEnd(UAnimMontage* Anim, bool _Inter)
{
	if (GetAnimMontage(SekiroState::JumpEnd) == Anim)
	{
		SetAniState(SekiroState::Idle);
	}

	else if (GetAnimMontage(SekiroState::JumpStart) == Anim)
	{
		SetAniState(SekiroState::JumpLoop);
	}

	else if (GetAnimMontage(SekiroState::JumpAttack) == Anim)
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

		BufferedAction = SekiroState::None;
	}

	bEnteredTransition = true;
}