// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "WarningWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE5_PORTFOLIO_SEKIRO_API UWarningWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void NativeConstruct() override;
	void NativeDestruct() override;
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void IconOn()
	{
		bOnIcon = true;
		bOffIcon = false;
	}

	UFUNCTION(BlueprintCallable)
	void IconOff()
	{
		bOffIcon = true;
		bOnIcon = false;
	}

	float GetIconOpacity()
	{
		return IconOpacity;
	}

	UPROPERTY(Category = "Icon", EditAnywhere, BlueprintReadWrite)
	UImage* WarningIcon = nullptr;


private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float IconOpacity = 0.0f;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bOnIcon = false;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bOffIcon = false;

	float SumTime = 0.0f;
};
