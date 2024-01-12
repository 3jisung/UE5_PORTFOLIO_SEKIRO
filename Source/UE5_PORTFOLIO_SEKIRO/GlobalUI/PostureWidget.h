// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Global/GlobalCharacter.h"
#include "Components/Image.h"
#include "PostureWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE5_PORTFOLIO_SEKIRO_API UPostureWidget : public UUserWidget
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


	// 현재 캐릭터의 HP
	UPROPERTY(Category = "Posture", EditAnywhere, BlueprintReadWrite)
	UImage* CharacterPosture = nullptr;


protected:
	void NativeConstruct() override;
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	AGlobalCharacter* Character = nullptr;
};
