// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster.h"
#include "Components/WidgetComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AICon.h"


AMonster::AMonster()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	// 락온 아이콘 설정
	FSoftClassPath LockOnIconClassPath(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Blueprint/Monster/WBP_LockOnIcon.WBP_LockOnIcon_C'"));
	LockOnIconWidgetClass = LockOnIconClassPath.TryLoadClass<UUserWidget>();

	LockOnWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("LockOnWidgetComponent"));
	LockOnWidgetComponent->SetWidgetClass(nullptr);
	LockOnWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	LockOnWidgetComponent->SetDrawSize(FVector2D(10.f, 10.f));
	LockOnWidgetComponent->AddRelativeLocation(FVector(0.f, 0.f, 10.0f));
	LockOnWidgetComponent->SetupAttachment(RootComponent);

	// 인살 아이콘 설정
	FSoftClassPath DeathblowIconClassPath(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Blueprint/Monster/WBP_DeathblowIcon.WBP_DeathblowIcon_C'"));
	DeathblowIconWidgetClass = DeathblowIconClassPath.TryLoadClass<UDeathblowWidget>();

	DeathblowWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("DeathblowWidgetComponent"));
	DeathblowWidgetComponent->SetWidgetClass(DeathblowIconWidgetClass);
	DeathblowWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	DeathblowWidgetComponent->SetDrawSize(FVector2D(150.f, 150.f));
	DeathblowWidgetComponent->AddRelativeLocation(FVector(0.f, 0.f, -30.0f));
	DeathblowWidgetComponent->SetupAttachment(RootComponent);

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(GetMesh(), TEXT("Weapon"));
}

void AMonster::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add(TEXT("Monster"));
}

void AMonster::Tick(float _Delta)
{
	Super::Tick(_Delta);
}

UBlackboardComponent* AMonster::GetBlackboardComponent()
{
	if (nullptr == BlackboardComponent)
	{
		AAICon* AiCon = GetController<AAICon>();

		if (nullptr == AiCon)
		{
			return nullptr;
		}

		BlackboardComponent = AiCon->GetBlackboardComponent();
	}

	return BlackboardComponent;
}

void AMonster::LockOnIconOnOff(bool bLockOn)
{
	if (bLockOn)
	{
		LockOnWidgetComponent->SetWidgetClass(LockOnIconWidgetClass);
	}
	else
	{
		LockOnWidgetComponent->SetWidgetClass(nullptr);
	}
}

void AMonster::DeathblowIconOnOff(bool bExhaust)
{
	UDeathblowWidget* DeathblowWidget = Cast<UDeathblowWidget>(DeathblowWidgetComponent->GetWidget());

	if (bExhaust)
	{
		DeathblowWidget->IconOn();
		bEnableDeathblow = true;
	}
	else
	{
		DeathblowWidget->IconOff();
		bEnableDeathblow = false;
	}
}