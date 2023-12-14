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
	FSoftClassPath ClassPath(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Blueprint/Monster/WBP_LockOnIcon.WBP_LockOnIcon_C'"));
	LockOnIconWidgetClass = ClassPath.TryLoadClass<UUserWidget>();

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetWidgetClass(nullptr);
	WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	WidgetComponent->SetDrawSize(FVector2D(10.f, 10.f));
	WidgetComponent->AddRelativeLocation(FVector(0.f, 0.f, 30.f));
	WidgetComponent->SetupAttachment(RootComponent);

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(GetMesh(), TEXT("Weapon"));
}

void AMonster::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add(TEXT("Monster"));

	// 무기 콜리전 설정
	WeaponMesh->OnComponentBeginOverlap.AddDynamic(this, &AMonster::BeginOverLap);
	WeaponMesh->OnComponentEndOverlap.AddDynamic(this, &AMonster::EndOverLap);
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
		WidgetComponent->SetWidgetClass(LockOnIconWidgetClass);
	}
	else
	{
		WidgetComponent->SetWidgetClass(nullptr);
	}
}

void AMonster::BeginOverLap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (OtherActor->ActorHasTag(TEXT("Player")))
	{
		bCollisionActor = true;
	}
}

void AMonster::EndOverLap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex
)
{
	if (OtherActor->ActorHasTag(TEXT("Player")))
	{
		bCollisionActor = false;
	}
}