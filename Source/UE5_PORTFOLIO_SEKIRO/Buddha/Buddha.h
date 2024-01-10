// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "BuddhaTriggerWidget.h"
#include "../Global/GlobalCharacter.h"
#include "Buddha.generated.h"

UCLASS()
class UE5_PORTFOLIO_SEKIRO_API ABuddha : public AStaticMeshActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABuddha();

	bool bTriggerEvent = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(Category = "Components", EditAnywhere, BlueprintReadWrite)
	class UWidgetComponent* BuddhaTriggerWidgetComponent;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void TriggerWidgetOnOff(bool _bTriggerEvent);
};
