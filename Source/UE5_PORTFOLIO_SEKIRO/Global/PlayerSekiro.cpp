// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerSekiro.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GlobalAnimInstance.h"
#include "GlobalGameInstance.h"
#include "Data/PlayerAnimData.h"
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
}

void APlayerSekiro::BeginPlay()
{
	Super::BeginPlay();

	//GetGlobalAnimInstance()->OnMontageBlendingOut.AddDynamic(this, &APlayerSekiro::MontageEnd);
	//GetGlobalAnimInstance()->OnPlayMontageNotifyBegin.AddDynamic(this, &APlayerSekiro::AnimNotifyBegin);

	SetAniState(SekiroState::Idle);

	UGlobalGameInstance* Inst = GetGameInstance<UGlobalGameInstance>();

	// 캐릭터 기본 스탯 설정
	StatData = Inst->GetPlayerStat(TEXT("Sekiro"));
	HP = StatData->HP;
	MaxHP = StatData->MaxHP;
	Posture = StatData->Posture;
	Power = StatData->Power;

	// 캐릭터 무기 설정
	WeaponArrays.Add(Inst->GetPlayerWeapon(TEXT("Katana")));
	WeaponMesh->SetStaticMesh(WeaponArrays[0]);

	// 캐릭터 애니메이션 설정
	FPlayerAnimData* AnimData = Inst->GetPlayerAnim(TEXT("Animations"));
	SetAllAnimation(AnimData->Animations);
	GetGlobalAnimInstance()->AllAnimations = AllAnimations;

	// 애니메이션 종료 시 MontageEnd를 콜백한다.
	GetGlobalAnimInstance()->OnMontageBlendingOut.AddDynamic(this, &APlayerSekiro::MontageEnd);

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

	UCharacterMovementComponent* Move = Cast<UCharacterMovementComponent>(GetMovementComponent());

	if (GetAniState<SekiroState>() == SekiroState::ForwardWalk)
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
			// 컨트롤러는 기본적으로 character 하나씩 붙어 있다.
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
			// 현재 내 회전을 가져와서 y축에 해당하는 축벡터를 얻어온다.
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
		&& AniStateValue != SekiroState::LeftRun && AniStateValue != SekiroState::RightRun)
	{
		return;
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

	Jump();
	SetAniState(SekiroState::Jump);
}

void APlayerSekiro::StartedDash()
{
	SekiroState AniStateValue = GetAniState<SekiroState>();

	// 대쉬 시작 전 상태값이 대기, 가드, 걷기 상태일 때만 대쉬 무적 시간 적용
	if (AniStateValue != SekiroState::Idle && AniStateValue != SekiroState::Guard
		&& AniStateValue != SekiroState::ForwardWalk && AniStateValue != SekiroState::BackwardWalk
		&& AniStateValue != SekiroState::LeftWalk && AniStateValue != SekiroState::RightWalk
		&& AniStateValue != SekiroState::ForwardRun && AniStateValue != SekiroState::BackwardRun
		&& AniStateValue != SekiroState::LeftRun && AniStateValue != SekiroState::RightRun)
	{
		return;
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
	
	float delayTime = 0.3f;
	FTimerHandle myTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(myTimerHandle, FTimerDelegate::CreateLambda([&]()
		{
			if (HitState == PlayerHitState::DASHINVINCIBLE)
			{
				HitState = PlayerHitState::OFFGUARD;
			}

			GetWorld()->GetTimerManager().ClearTimer(myTimerHandle);
		}), delayTime, false);
}

void APlayerSekiro::PlayerDash(bool _bDash, float TriggeredSec)
{
	if (_bDash)
	{
		Speed = 1800.0f;
		bDash = _bDash;
	}
	else
	{
		// 대쉬 키를 눌렀다가 바로 떼어도 최소한 0.5초 동안은 대쉬 상태를 유지
		float MinDashTime = 0.5f;

		if (TriggeredSec < MinDashTime)
		{
			float delayTime = MinDashTime - TriggeredSec;
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
			bDash = _bDash;
		}
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

void APlayerSekiro::MontageEnd(UAnimMontage* Anim, bool _Inter)
{
	if (GetAnimMontage(SekiroState::Jump) == Anim)
	{
		SetAniState(SekiroState::Idle);
	}
}