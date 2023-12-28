// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "PlayerSekiro.h"
#include "Kismet/GameplayStatics.h"
#include "BossInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE5_PORTFOLIO_SEKIRO_API UBossInfoWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void GetHitEvent(float HPDifference);

	// 보스 몬스터의 HP
	UPROPERTY(Category = "HP", EditAnywhere, BlueprintReadWrite)
	UImage* BossHP = nullptr;

	// 보스가 받은 피해량을 나타내는 피격바(1초 유지)
	UPROPERTY(Category = "HP", EditAnywhere, BlueprintReadWrite)
	UImage* BossGetHitHP = nullptr;

	UPROPERTY(Category = "Name", EditAnywhere, BlueprintReadWrite)
	UTextBlock* BossName = nullptr;

protected:
	void NativeConstruct() override;
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	double MaxHPSize = 0.0;
	float CurHPValue = 0.0f;
	bool bAdjustGetHitHP = false;

	APlayerSekiro* Player = nullptr;
};
