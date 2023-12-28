// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "PlayerSekiro.h"
#include "Kismet/GameplayStatics.h"
#include "MainWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE5_PORTFOLIO_SEKIRO_API UMainWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void SetPlayerPostureUIOnOffSwitch()
	{
		PlayerPostureUIOnOff = PlayerPostureUIOnOff == ESlateVisibility::Hidden ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
	}

	UFUNCTION(BlueprintCallable)
	void SetBossPostureUIOnOffSwitch()
	{
		BossPostureUIOnOff = BossPostureUIOnOff == ESlateVisibility::Hidden ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
	}

	UFUNCTION(BlueprintCallable)
	void SetBossInfoUIOnOffSwitch()
	{
		BossInfoUIOnOff = BossInfoUIOnOff == ESlateVisibility::Hidden ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
	}

	void GetHitEvent(float HPDifference);

	// 현재 플레이어의 HP
	UPROPERTY(Category = "HP", EditAnywhere, BlueprintReadWrite)
	UImage* PlayerHP = nullptr;

	// 플레이어가 받은 피해량을 나타내는 피격바(1초 유지)
	UPROPERTY(Category = "HP", EditAnywhere, BlueprintReadWrite)
	UImage* PlayerGetHitHP = nullptr;


protected:
	void NativeConstruct() override;
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;


private:
	UPROPERTY(Category = "UIOnOff", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	ESlateVisibility PlayerPostureUIOnOff = ESlateVisibility::Hidden;

	UPROPERTY(Category = "UIOnOff", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	ESlateVisibility BossPostureUIOnOff = ESlateVisibility::Hidden;

	UPROPERTY(Category = "UIOnOff", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	ESlateVisibility BossInfoUIOnOff = ESlateVisibility::Hidden;

	double MaxHPSize = 0.0;
	float CurHPValue = 0.0f;
	bool bAdjustGetHitHP = false;

	APlayerSekiro* Player = nullptr;
};
