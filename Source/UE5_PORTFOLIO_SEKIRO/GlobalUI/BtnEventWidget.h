// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GlobalUI/FadeInOutWidget.h"
#include "Components/Image.h"
#include "Sound/SoundBase.h"
#include "Global/GlobalGameInstance.h"
#include "BtnEventWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE5_PORTFOLIO_SEKIRO_API UBtnEventWidget : public UFadeInOutWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct();

	UFUNCTION(BlueprintCallable)
	virtual bool SetHoveredBtn(int _Index);

	UFUNCTION(BlueprintCallable)
	virtual void MenuEvent();

	UPROPERTY(Category = "Btn", EditAnywhere, BlueprintReadWrite)
	int HoveredIndex = 0;

	TArray<UImage*> BtnHoveredImage;
	TMap<FString, USoundBase*> SoundEffects;
};
