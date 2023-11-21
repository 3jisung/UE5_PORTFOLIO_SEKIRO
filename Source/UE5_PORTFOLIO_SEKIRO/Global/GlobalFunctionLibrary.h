// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GlobalFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class UE5_PORTFOLIO_SEKIRO_API UGlobalFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	

public:
	static FRandomStream MainRandom;

	//UFUNCTION(BlueprintCallable, Category = "ARGlobal", meta = (DefaultToSelf = "_Object"))
	//static void MainUIOnOff(ESlateVisibility _Value);

	UFUNCTION(BlueprintCallable, Category = "GlobalFunction")
	static UWorld* GetCurrentWorld();
};
