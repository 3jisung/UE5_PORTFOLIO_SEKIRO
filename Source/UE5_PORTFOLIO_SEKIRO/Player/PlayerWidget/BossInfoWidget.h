// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GlobalUI/HPWidget.h"
#include "Player/PlayerSekiro.h"
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
	void UpdateDeathblowUI(int NewDeathblowCount, float _Delta);

	// 보스 몬스터의 HP
	UPROPERTY(Category = "HP", EditAnywhere, BlueprintReadWrite)
	UHPWidget* HPWidget = nullptr;

	UPROPERTY(Category = "Name", EditAnywhere, BlueprintReadWrite)
	UTextBlock* BossName = nullptr;

protected:
	void NativeConstruct() override;
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	TArray<UImage*> DeathblowBackgroundImage;
	TArray<UImage*> DeathblowImage;
	int DeathblowCount = 0;

	APlayerSekiro* Player = nullptr;
};
