// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GlobalUI/FadeInOutWidget.h"
#include "Components/Image.h"
#include "DeathWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE5_PORTFOLIO_SEKIRO_API UDeathWidget : public UFadeInOutWidget
{
	GENERATED_BODY()
	

public:
	void NativeConstruct() override;
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void BackgroundOn()
	{
		BackgroundOpacity = 0.f;
		bOnBackground = true;
		bOffBackground = false;
	}

	UFUNCTION(BlueprintCallable)
	void BackgroundOff()
	{
		BackgroundOpacity = 1.f;
		bOffBackground = true;
		bOnBackground = false;
	}

	UFUNCTION(BlueprintCallable)
	void DeathUIOn()
	{
		DeathUIOpacity = 0.f;
		bOnDeathUI = true;
		bOffDeathUI = false;
	}

	UFUNCTION(BlueprintCallable)
	void DeathUIOff()
	{
		DeathUIOpacity = 1.f;
		bOffDeathUI = true;
		bOnDeathUI = false;
	}

	float GetBackgroundOpacity()
	{
		return BackgroundOpacity;
	}

	float GetDeathUIOpacity()
	{
		return DeathUIOpacity;
	}

	UPROPERTY(Category = "Screen", EditAnywhere, BlueprintReadWrite)
	UImage* BackgroundImage = nullptr;

	UPROPERTY(Category = "UI", EditAnywhere, BlueprintReadWrite)
	UImage* DeathUIImage = nullptr;


private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float BackgroundOpacity = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float DeathUIOpacity = 0.f;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bOnBackground = false;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bOffBackground = false;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bOnDeathUI = false;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bOffDeathUI = false;

	float BackgroundSumTime = 0.f;
	float DeathUISumTime = 0.f;
};
