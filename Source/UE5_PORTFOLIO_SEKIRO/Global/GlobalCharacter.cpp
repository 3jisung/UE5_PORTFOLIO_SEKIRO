// Fill out your copyright notice in the Description page of Project Settings.


#include "GlobalCharacter.h"
#include "GlobalAnimInstance.h"

// Sets default values
AGlobalCharacter::AGlobalCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGlobalCharacter::BeginPlay()
{
	Super::BeginPlay();

	GlobalAnimInstance = Cast<UGlobalAnimInstance>(GetMesh()->GetAnimInstance());
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

void AGlobalCharacter::AdjustAngle(float DeltaSeconds, FVector TargetPos, float Angle)
{
	FVector ThisPos = this->GetActorLocation();

	ThisPos.Z = 0.0f;
	TargetPos.Z = 0.0f;

	FVector TargetDir = TargetPos - ThisPos;
	TargetDir.Normalize();

	FVector OtherForward = this->GetActorForwardVector();
	OtherForward.Normalize();

	// 벡터곱을 통해 타겟과의 각도 방향을 알 수 있다.(정확히는 벡터곱의 Z값)
	FVector Cross = FVector::CrossProduct(OtherForward, TargetDir);

	float Angle0 = TargetDir.Rotation().Yaw;
	float Angle1 = OtherForward.Rotation().Yaw;

	// 정면 벡터를 기준으로 타겟과 방향이 Angle 이상 어긋날 경우 방향 조정
	if (FMath::Abs(Angle0 - Angle1) >= Angle)
	{
		FRotator Rot = FRotator::MakeFromEuler({ 0, 0, Cross.Z * 500.0f * DeltaSeconds });
		this->AddActorWorldRotation(Rot);
	}
	// 그 외에는 방향값 그대로 유지
	else
	{
		FRotator Rot = TargetDir.Rotation();
		this->SetActorRotation(Rot);
	}
}

bool AGlobalCharacter::CheckAngle(FVector TargetPos, float Angle)
{
	FVector ThisPos = this->GetActorLocation();

	ThisPos.Z = 0.0f;
	TargetPos.Z = 0.0f;

	FVector TargetDir = TargetPos - ThisPos;
	TargetDir.Normalize();

	FVector OtherForward = this->GetActorForwardVector();
	OtherForward.Normalize();

	FVector Cross = FVector::CrossProduct(OtherForward, TargetDir);

	float Angle0 = TargetDir.Rotation().Yaw;
	float Angle1 = OtherForward.Rotation().Yaw;

	if (FMath::Abs(Angle0 - Angle1) >= Angle)
	{
		return true;
	}
	else
	{
		return false;
	}
}