// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GlobalUI/HPWidget.h"
#include "Player/PlayerSekiro.h"
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


	UPROPERTY(Category = "Gourd", EditAnywhere, BlueprintReadWrite)
	UImage* GourdImage = nullptr;

	UPROPERTY(Category = "Gourd", EditAnywhere, BlueprintReadWrite)
	UTextBlock* HealCount = nullptr;

	UPROPERTY(Category = "HP", EditAnywhere, BlueprintReadWrite)
	UHPWidget* HPWidget = nullptr;

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

	APlayerSekiro* Player = nullptr;
};
