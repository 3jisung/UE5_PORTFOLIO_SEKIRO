// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerSekiro.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GlobalAnimInstance.h"
#include "GlobalGameInstance.h"
#include "Components/StaticMeshComponent.h"


APlayerSekiro::APlayerSekiro()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->TargetArmLength = 800.0f;
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

	//SetAniState(AIAniState::Idle);

	UGlobalGameInstance* Inst = GetGameInstance<UGlobalGameInstance>();

	StatData = GetGameInstance<UGlobalGameInstance>()->GetPlayerStat(TEXT("Sekiro"));
	HP = StatData->HP;
	MaxHP = StatData->MaxHP;
	Posture = StatData->Posture;
	Power = StatData->Power;

	WeaponArrays.Add(GetGameInstance<UGlobalGameInstance>()->GetPlayerWeapon(TEXT("Katana")));
	WeaponMesh->SetStaticMesh(WeaponArrays[0]);
}

void APlayerSekiro::Tick(float _Delta)
{

}

void APlayerSekiro::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}