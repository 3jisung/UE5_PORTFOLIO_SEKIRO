// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Global/GlobalCharacter.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "HPWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE5_PORTFOLIO_SEKIRO_API UHPWidget : public UUserWidget
{
	GENERATED_BODY()
	

public:
	void CharacterSetting(AGlobalCharacter* _Character)
	{
		if (IsValid(_Character))
		{
			Character = _Character;
		}
	}

	void GetHitEvent(float HPDifference);


	// 현재 캐릭터의 HP
	UPROPERTY(Category = "HP", EditAnywhere, BlueprintReadWrite)
	UImage* CharacterHP = nullptr;

	// 캐릭터가 받은 피해량을 나타내는 피격바(1초 유지)
	UPROPERTY(Category = "HP", EditAnywhere, BlueprintReadWrite)
	UImage* CharacterGetHitHP = nullptr;

	UPROPERTY(Category = "HP", EditAnywhere, BlueprintReadWrite)
	float CurHPValue = 0.f;


protected:
	void NativeConstruct() override;
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	double MaxHPSize = 0.0;
	bool bAdjustGetHitHP = false;

	AGlobalCharacter* Character = nullptr;
};
