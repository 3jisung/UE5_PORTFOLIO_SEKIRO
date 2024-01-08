// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "DeathWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE5_PORTFOLIO_SEKIRO_API UDeathWidget : public UUserWidget
{
	GENERATED_BODY()
	

public:
	void NativeConstruct() override;
	void NativeDestruct() override;
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void BackgroundOn()
	{
		bOnBackground = true;
		bOffBackground = false;
	}

	UFUNCTION(BlueprintCallable)
	void BackgroundOff()
	{
		bOffBackground = true;
		bOnBackground = false;
	}

	UFUNCTION(BlueprintCallable)
	void DeathUIOn()
	{
		bOnDeathUI = true;
		bOffDeathUI = false;
	}

	UFUNCTION(BlueprintCallable)
	void DeathUIOff()
	{
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
	float BackgroundOpacity = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float DeathUIOpacity = 0.0f;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bOnBackground = false;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bOffBackground = false;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bOnDeathUI = false;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bOffDeathUI = false;

	float SumTime = 0.0f;
};
