// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GlobalAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class UE5_PORTFOLIO_SEKIRO_API UGlobalAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	

public:
	UPROPERTY(Category = "GameModeValue", EditAnywhere, BlueprintReadWrite)
	int AniState = 0;

	TMap<int, class UAnimMontage*> AllAnimations;

	class UAnimMontage* CurMontage = nullptr;


protected:
	void NativeBeginPlay() override;
	void NativeUpdateAnimation(float _DeltaTime) override;
};
