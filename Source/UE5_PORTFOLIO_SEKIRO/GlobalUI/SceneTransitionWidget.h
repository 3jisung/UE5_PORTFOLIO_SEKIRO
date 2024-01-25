// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FadeInOutWidget.h"
#include "SceneTransitionWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE5_PORTFOLIO_SEKIRO_API USceneTransitionWidget : public UFadeInOutWidget
{
	GENERATED_BODY()
	

public:
	void NativeConstruct() override;
};
