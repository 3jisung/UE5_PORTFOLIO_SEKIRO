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

	float TakeDamage(float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser) override;

	void GetHitExecute(float DamageAmount, UCustomDamageTypeBase* DamageType, AActor* DamageCauser);
	void ExhaustAction();
	void DeathblowAction();
	void DeathAction();

	bool GetHitCheck() override;
	bool BlockCheck() override;
	bool IsExhaust() override;
	bool IsDeath() override;

	void AttackMove() override;

	void Damage() override;

protected:
	void BeginPlay() override;
	void Tick(float _Delta) override;
	
private:
	void MontageBlendingOut(UAnimMontage* Anim, bool _Inter) override;

	int ParryingCount = 0;
};
