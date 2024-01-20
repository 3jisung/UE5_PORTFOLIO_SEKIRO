// Fill out your copyright notice in the Description page of Project Settings.


#include "GlobalCharacter.h"
#include "GlobalAnimInstance.h"
#include "Components/WidgetComponent.h"

// Sets default values
AGlobalCharacter::AGlobalCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 타뢰(打雷) 컴포넌트 설정
	ShockWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("ShockWidgetComponent"));
	ShockWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	ShockWidgetComponent->SetDrawSize(FVector2D(150.f, 300.f));
	ShockWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 130.f));
	ShockWidgetComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AGlobalCharacter::BeginPlay()
{
	Super::BeginPlay();

	GlobalAnimInstance = Cast<UGlobalAnimInstance>(GetMesh()->GetAnimInstance());

	// 타뢰(打雷) 문자 아이콘 설정
	UGlobalGameInstance* Inst = GetGameInstance<UGlobalGameInstance>();
	TSubclassOf<UUserWidget> WidgetClass = Inst->GetWidgetClassData(TEXT("Character"), TEXT("Shock"));
	if (IsValid(WidgetClass))
	{
		ShockWidgetComponent->SetWidgetClass(WidgetClass);
	}
}

// Called every frame
void AGlobalCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AGlobalCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AGlobalCharacter::ShowShockIcon()
{
	UShockWidget* ShockWidget = Cast<UShockWidget>(ShockWidgetComponent->GetWidget());

	ShockWidget->FadeIn();

	float delayTime = 1.5f;
	FTimerHandle myTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(myTimerHandle, FTimerDelegate::CreateLambda([&, ShockWidget]()
		{
			ShockWidget->FadeOut();

			GetWorld()->GetTimerManager().ClearTimer(myTimerHandle);
		}), delayTime, false);
}

void AGlobalCharacter::AdjustAngle(FVector TargetPos)
{
	FVector ThisPos = this->GetActorLocation();

	ThisPos.Z = 0.f;
	TargetPos.Z = 0.f;

	FVector TargetDir = TargetPos - ThisPos;
	TargetDir.Normalize();

	FVector OtherForward = this->GetActorForwardVector();
	OtherForward.Normalize();

	// 벡터곱을 통해 타겟과의 각도 방향을 알 수 있다.(정확히는 벡터곱의 Z값)
	FVector Cross = FVector::CrossProduct(OtherForward, TargetDir);

	float Angle0 = TargetDir.Rotation().Yaw;
	float Angle1 = OtherForward.Rotation().Yaw;

	FRotator Rot = TargetDir.Rotation();
	this->SetActorRotation(Rot);
}

void AGlobalCharacter::AdjustAngle(float DeltaSeconds, FVector TargetPos, float Angle)
{
	FVector ThisPos = this->GetActorLocation();

	ThisPos.Z = 0.f;
	TargetPos.Z = 0.f;

	FVector TargetDir = TargetPos - ThisPos;
	TargetDir.Normalize();

	FVector OtherForward = this->GetActorForwardVector();
	OtherForward.Normalize();

	// 벡터곱을 통해 타겟과의 각도 방향을 알 수 있다.(정확히는 벡터곱의 Z값)
	FVector Cross = FVector::CrossProduct(OtherForward, TargetDir);

	float Angle0 = TargetDir.Rotation().Yaw;
	float Angle1 = OtherForward.Rotation().Yaw;

	// 정면 벡터를 기준으로 타겟과 방향이 Angle 이상 어긋날 경우 회전 보간을 통한 방향 조정
	if (FMath::Abs(Angle0 - Angle1) >= Angle)
	{
		FRotator Rot = FRotator::MakeFromEuler({ 0, 0, Cross.Z * 500.f * DeltaSeconds });
		this->AddActorWorldRotation(Rot);
	}
	// Angle 이하의 각도로 차이가 날 경우 TargetDir의 Rotation으로 바로 전환(회전 보간 없이)
	else
	{
		FRotator Rot = TargetDir.Rotation();
		this->SetActorRotation(Rot);
	}
}

float AGlobalCharacter::CalculateAngle(FVector TargetPos)
{
	FVector ThisPos = this->GetActorLocation();

	ThisPos.Z = 0.f;
	TargetPos.Z = 0.f;

	FVector TargetDir = TargetPos - ThisPos;
	TargetDir.Normalize();

	FVector OtherForward = this->GetActorForwardVector();
	OtherForward.Normalize();

	FVector Cross = FVector::CrossProduct(OtherForward, TargetDir);

	float Angle0 = TargetDir.Rotation().Yaw;
	float Angle1 = OtherForward.Rotation().Yaw;

	return FMath::Abs(Angle0 - Angle1);
}

TArray<AActor*> AGlobalCharacter::TraceObjects(
	EObjectTypeQuery _ObjectType,
	FVector _TraceDir,
	float _TraceAngle,
	float _TraceRange,
	float _SphereRadius
)
{
	FVector TraceDirVector = _TraceDir;

	TArray<AActor*> ActorsToNotTargeting;
	ActorsToNotTargeting.Add(this);

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypeToLock;
	EObjectTypeQuery ObjectType = _ObjectType;
	ObjectTypeToLock.Emplace(ObjectType);

	FHitResult HitResult;
	TArray<AActor*> HitActor;

	FVector StartPoint = GetActorLocation();

	// perspective 관점으로 탐색
	for (size_t i = 0; i < _TraceAngle * 2; i += 5)
	{
		for (size_t j = 0; j < _TraceAngle; j += 5)
		{
			FVector DirectionX = TraceDirVector.RotateAngleAxis(-_TraceAngle * 0.5 + j, GetActorRightVector());
			FVector DirectionY = DirectionX.RotateAngleAxis(-_TraceAngle + i, FVector::UpVector);
			FVector EndPoint = StartPoint + DirectionY * _TraceRange;

			bool bIsHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
				GetWorld(), StartPoint, EndPoint, _SphereRadius,
				ObjectTypeToLock, false, ActorsToNotTargeting, EDrawDebugTrace::None,
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

TArray<AActor*> AGlobalCharacter::TraceObjects(
	EObjectTypeQuery _ObjectType,
	TArray<AActor*> _ActorsToNotTargeting,
	FVector _TraceDir,
	float _TraceAngle,
	float _TraceRange,
	float _SphereRadius
)
{
	FVector TraceDirVector = _TraceDir;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypeToLock;
	EObjectTypeQuery ObjectType = _ObjectType;
	ObjectTypeToLock.Emplace(ObjectType);

	FHitResult HitResult;
	TArray<AActor*> HitActor;

	FVector StartPoint = GetActorLocation();

	// perspective 관점으로 탐색
	for (size_t i = 0; i < _TraceAngle * 2; i += 5)
	{
		for (size_t j = 0; j < _TraceAngle; j += 5)
		{
			FVector DirectionX = TraceDirVector.RotateAngleAxis(-_TraceAngle * 0.5 + j, GetActorRightVector());
			FVector DirectionY = DirectionX.RotateAngleAxis(-_TraceAngle + i, FVector::UpVector);
			FVector EndPoint = StartPoint + DirectionY * _TraceRange;

			bool bIsHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
				GetWorld(), StartPoint, EndPoint, _SphereRadius,
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

TArray<AActor*> AGlobalCharacter::TraceObjectsDebug(
	EObjectTypeQuery _ObjectType,
	FVector _TraceDir,
	float _TraceAngle,
	float _TraceRange,
	float _SphereRadius
)
{
	FVector TraceDirVector = _TraceDir;

	TArray<AActor*> ActorsToNotTargeting;
	ActorsToNotTargeting.Add(this);

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypeToLock;
	EObjectTypeQuery ObjectType = _ObjectType;
	ObjectTypeToLock.Emplace(ObjectType);

	FHitResult HitResult;
	TArray<AActor*> HitActor;

	FVector StartPoint = GetActorLocation();

	// perspective 관점으로 탐색
	for (size_t i = 0; i < _TraceAngle * 2; i += 5)
	{
		for (size_t j = 0; j < _TraceAngle; j += 5)
		{
			FVector DirectionX = TraceDirVector.RotateAngleAxis(-_TraceAngle * 0.5 + j, GetActorRightVector());
			FVector DirectionY = DirectionX.RotateAngleAxis(-_TraceAngle + i, FVector::UpVector);
			FVector EndPoint = StartPoint + DirectionY * _TraceRange;

			bool bIsHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
				GetWorld(), StartPoint, EndPoint, _SphereRadius,
				ObjectTypeToLock, false, ActorsToNotTargeting, EDrawDebugTrace::ForDuration,
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

void AGlobalCharacter::GetHitImpulseManager(AActor* DamageCauser, float PushPower)
{
	FVector ImpulseVector = DamageCauser->GetActorForwardVector();

	// 공중에서 맞을 경우 방향 및 power 조정
	if (GetMovementComponent()->IsFalling())
	{
		ImpulseVector = ImpulseVector.RotateAngleAxis(60.f, DamageCauser->GetActorRightVector());
		GetCharacterMovement()->AddImpulse(ImpulseVector * PushPower * 0.5f, true);
	}
	else
	{
		GetCharacterMovement()->AddImpulse(ImpulseVector * PushPower, true);
	}
}
