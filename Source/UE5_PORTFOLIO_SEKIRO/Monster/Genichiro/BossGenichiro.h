// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Monster.h"
#include "../../Global/GlobalEnums.h"
#include "BossGenichiro.generated.h"

/**
 * 
 */
UCLASS()
class UE5_PORTFOLIO_SEKIRO_API ABossGenichiro : public AMonster
{
	GENERATED_BODY()
	

public:
	ABossGenichiro();

	void AttackMove() override;

protected:
	void BeginPlay() override;
	//void Tick(float _Delta) override;


private:
	void MontageEnd() override;
	
};
