// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "ExecutionWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE5_PORTFOLIO_SEKIRO_API UExecutionWidget : public UUserWidget
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
	void ExecutionUIOn()
	{
		bOnExecutionUI = true;
		bOffExecutionUI = false;
	}

	UFUNCTION(BlueprintCallable)
	void ExecutionUIOff()
	{
		bOffExecutionUI = true;
		bOnExecutionUI = false;
	}

	float GetBackgroundOpacity()
	{
		return BackgroundOpacity;
	}

	float GetDeathUIOpacity()
	{
		return ExecutionUIOpacity;
	}

	UPROPERTY(Category = "Screen", EditAnywhere, BlueprintReadWrite)
	UImage* BackgroundImage = nullptr;

	UPROPERTY(Category = "UI", EditAnywhere, BlueprintReadWrite)
	UImage* ExecutionUIImage = nullptr;


private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float BackgroundOpacity = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float ExecutionUIOpacity = 0.0f;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bOnBackground = false;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bOffBackground = false;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bOnExecutionUI = false;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bOffExecutionUI = false;

	float BackgroundSumTime = 0.0f;
	float ExecutionUISumTime = 0.0f;
};
