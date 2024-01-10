// Fill out your copyright notice in the Description page of Project Settings.


#include "Buddha.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABuddha::ABuddha()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 불상 트리거 위젯 설정
	FSoftClassPath BuddhaTriggerClassPath(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Blueprint/Buddha/WBP_TriggerWidget.WBP_TriggerWidget_C'"));

	BuddhaTriggerWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("BuddhaTriggerWidgetComponent"));
	BuddhaTriggerWidgetComponent->SetWidgetClass(BuddhaTriggerClassPath.TryLoadClass<UBuddhaTriggerWidget>());
	BuddhaTriggerWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	BuddhaTriggerWidgetComponent->SetDrawSize(FVector2D(500.f, 100.f));
	BuddhaTriggerWidgetComponent->AddRelativeLocation(FVector(0.f, 0.f, 150.f));
	BuddhaTriggerWidgetComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ABuddha::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABuddha::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	EObjectTypeQuery ObjectType = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel5);
	AGlobalCharacter* PlayerCharacter = Cast<AGlobalCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	TArray<AActor*> HitActor = PlayerCharacter->TraceObjects(ObjectType, PlayerCharacter->GetActorForwardVector(), 45.f, 150.f, 70.f);

	if (HitActor.Num() == 0)
	{
		TriggerWidgetOnOff(false);
	}

	for (size_t i = 0; i < HitActor.Num(); i++)
	{
		if (HitActor[i] == this)
		{
			TriggerWidgetOnOff(true);
			break;
		}

		if (i == HitActor.Num() - 1)
		{
			TriggerWidgetOnOff(false);
		}
	}
}

void ABuddha::TriggerWidgetOnOff(bool _bTriggerEvent)
{
	UBuddhaTriggerWidget* BuddhaTriggerWidget = Cast<UBuddhaTriggerWidget>(BuddhaTriggerWidgetComponent->GetWidget());

	if (_bTriggerEvent)
	{
		BuddhaTriggerWidget->FadeIn();
		bTriggerEvent = true;
	}
	else
	{
		BuddhaTriggerWidget->FadeOut();
		bTriggerEvent = false;
	}
}
