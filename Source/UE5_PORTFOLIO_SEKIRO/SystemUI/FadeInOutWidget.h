// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "FadeInOutWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE5_PORTFOLIO_SEKIRO_API UFadeInOutWidget : public UUserWidget
{
	GENERATED_BODY()
	

public:
	void NativeConstruct() override;
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void FadeIn()
	{
		FadeInState = true;
		FadeOutState = false;
		ScreenOpacity = 1.0f;
	}

	UFUNCTION(BlueprintCallable)
	void FadeOut()
	{
		FadeOutState = true;
		FadeInState = false;
		ScreenOpacity = 0.0f;
	}

	UPROPERTY(Category = "Screen", EditAnywhere, BlueprintReadWrite)
	UImage* FadeScreen = nullptr;


private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float ScreenOpacity = 0.0f;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool FadeInState = false;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool FadeOutState = false;

	float SumTime = 0.0f;
};
