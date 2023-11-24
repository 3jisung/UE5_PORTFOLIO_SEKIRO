// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerSekiro.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GlobalAnimInstance.h"
#include "GlobalGameInstance.h"
#include "GlobalEnums.h"
#include "Data/PlayerAnimData.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Controller.h"


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

	StatData = Inst->GetPlayerStat(TEXT("Sekiro"));
	HP = StatData->HP;
	MaxHP = StatData->MaxHP;
	Posture = StatData->Posture;
	Power = StatData->Power;

	WeaponArrays.Add(Inst->GetPlayerWeapon(TEXT("Katana")));
	WeaponMesh->SetStaticMesh(WeaponArrays[0]);

	FPlayerAnimData* AnimData = Inst->GetPlayerAnim(TEXT("Animations"));
	SetAllAnimation(AnimData->Animations);
	GetGlobalAnimInstance()->AllAnimations = AllAnimations;

	// 락온 상태가 아닐 때 움직임을 Forward로 고정하기 위한 설정값.
	// 락온 상태일 땐 false로 변경해야 한다.
	GetCharacterMovement()->bOrientRotationToMovement = true;
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
		FVector LockedOnLocation = LockedOnTarget->GetActorLocation();

		// 락온 범위를 벗어나면 락온 자동 해제
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

	if (AniStateValue != SekiroState::Idle && AniStateValue != SekiroState::ForwardWalk && AniStateValue != SekiroState::BackwardWalk
		&& AniStateValue != SekiroState::LeftWalk && AniStateValue != SekiroState::RightWalk)
	{
		return;
	}

	if (Val != 0.f)
	{
		if (Controller)
		{
			// 컨트롤러는 기본적으로 charcter 하나씩 붙어 있다.
			FRotator const ControlSpaceRot = Controller->GetControlRotation();	

			AddMovementInput(FRotationMatrix(ControlSpaceRot).GetScaledAxis(EAxis::X), Val);

			if (bLockOn)
			{
				SetAniState(Val > 0.f ? SekiroState::ForwardWalk : SekiroState::BackwardWalk);
			}
			else
			{
				SetAniState(SekiroState::ForwardWalk);
			}

			return;
		}
	}
	else
	{
		if (AniStateValue == SekiroState::ForwardWalk || AniStateValue == SekiroState::BackwardWalk)
		{
			SetAniState(SekiroState::Idle);
		}
	}
}

void APlayerSekiro::MoveRight(float Val)
{
	SekiroState AniStateValue = GetAniState<SekiroState>();

	if (AniStateValue != SekiroState::Idle && AniStateValue != SekiroState::ForwardWalk && AniStateValue != SekiroState::BackwardWalk
		&& AniStateValue != SekiroState::LeftWalk && AniStateValue != SekiroState::RightWalk)
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
				SetAniState(Val > 0.f ? SekiroState::RightWalk : SekiroState::LeftWalk);
			}
			else
			{
				SetAniState(SekiroState::ForwardWalk);
			}
			
			return;
		}
	}
	else
	{
		if (AniStateValue == SekiroState::RightWalk || AniStateValue == SekiroState::LeftWalk || AniStateValue == SekiroState::ForwardWalk)
		{
			SetAniState(SekiroState::Idle);
		}
	}
}

void APlayerSekiro::LockOnTarget()
{
	if (bLockOn == false)
	{
		FVector CameraForwardVector = CameraComponent->GetForwardVector();
		
		// 락온 대상 지정
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypeToLock;
		//ObjectTypeToLock.Add(EObjectTypeQuery::ObjectTypeQuery3);
		// ECollisionChannel::ECC_Pawn == EObjectTypeQuery::ObjectTypeQuery3 (폰 관련 오브젝트 타입)
		EObjectTypeQuery ObjectType = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn);
		ObjectTypeToLock.Emplace(ObjectType);

		// 락온 제외 액터
		TArray<AActor*> ActorsToNotTargeting;
		ActorsToNotTargeting.Add(this);

		FHitResult HitResult;

		float ClosestDist = LockOnRange;
		AActor* ClosestHitActor = nullptr;

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
					ObjectTypeToLock, false, ActorsToNotTargeting, EDrawDebugTrace::None,
					HitResult, true);

				// 가장 가까운 적을 락온
				if (bIsHit && HitResult.Distance < ClosestDist)
				{
					ClosestDist = HitResult.Distance;
					ClosestHitActor = HitResult.GetActor();
				}
			}
		}

		if (ClosestHitActor)
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

void APlayerSekiro::ToggleLockOn()
{
	if (bLockOn)
	{
		GetCharacterMovement()->bOrientRotationToMovement = true;
		bLockOn = false;
	}
	else
	{
		GetCharacterMovement()->bOrientRotationToMovement = false;
		bLockOn = true;
	}
}