// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster.h"
#include "Components/WidgetComponent.h"


AMonster::AMonster()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	Tags.Add(TEXT("Monster"));

	// 락온 아이콘 설정
	FSoftClassPath ClassPath(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Blueprint/Global/Monster/WBP_LockOnIcon.WBP_LockOnIcon_C'"));
	LockOnIconWidgetClass = ClassPath.TryLoadClass<UUserWidget>();

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetWidgetClass(nullptr);
	WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	WidgetComponent->SetDrawSize(FVector2D(10.f, 10.f));
	WidgetComponent->AddRelativeLocation(FVector(0.f, 0.f, 30.f));
	WidgetComponent->SetupAttachment(RootComponent);
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