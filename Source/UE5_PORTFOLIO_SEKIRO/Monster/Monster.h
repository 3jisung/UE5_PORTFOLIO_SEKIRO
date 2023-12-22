// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Global/GlobalCharacter.h"
#include "../Global/Data/MonsterData.h"
#include "../UI/DeathblowWidget.h"
#include "Monster.generated.h"

/**
 * 
 */
UCLASS()
class UE5_PORTFOLIO_SEKIRO_API AMonster : public AGlobalCharacter
{
	GENERATED_BODY()
	

public:
	AMonster();

	FORCEINLINE class UBehaviorTree* GetBehaviorTree()
	{
		return BehaviorTree;
	}

	class UBlackboardComponent* GetBlackboardComponent();

	int GetDeathblowCount()
	{
		return DeathblowCount;
	}

	void SetDeathblowCount(int _DeathblowCount)
	{
		DeathblowCount = _DeathblowCount;
	}

	MonsterHitState GetHitState()
	{
		return HitState;
	}

	void SetHitState(MonsterHitState _HitState)
	{
		HitState = _HitState;
	}

	void LockOnIconOnOff(bool bLockOn);
	void DeathblowIconOnOff(bool bExhaust);

	UFUNCTION(BlueprintCallable)
	virtual void AttackMove() { UE_LOG(LogTemp, Error, TEXT("Monster AttackMove")); }

	UFUNCTION(BlueprintCallable)
	virtual void MontageEnd() { UE_LOG(LogTemp, Error, TEXT("Monster MontageEnd")); }

	UFUNCTION(BlueprintCallable)
	void OffGuard()
	{
		HitState = MonsterHitState::OFFGUARD;
	}

	const struct FMonsterData* MonsterData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* WeaponMesh;

	// 인살 가능 상태
	bool bEnableDeathblow = false;
	

protected:
	void BeginPlay() override;
	void Tick(float _Delta) override;

	UFUNCTION()
	virtual void MontageBlendingOut(UAnimMontage* Anim, bool _Inter) { UE_LOG(LogTemp, Error, TEXT("Monster BlendingOut")); }

	virtual bool GetHitCheck() { UE_LOG(LogTemp, Error, TEXT("Monster GetHitCheck")); return false; }
	virtual bool BlockCheck() { UE_LOG(LogTemp, Error, TEXT("Monster BlockCheck")); return false; }
	virtual bool IsExhaust() { UE_LOG(LogTemp, Error, TEXT("Monster ExhaustCheck")); return false; }
	virtual bool IsDeath() { UE_LOG(LogTemp, Error, TEXT("Monster DeathCheck")); return false; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	class UBehaviorTree* BehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	class UBlackboardComponent* BlackboardComponent;

	UPROPERTY(Category = "Monster", EditAnywhere, BlueprintReadWrite)
	int DeathblowCount;

	UPROPERTY(Category = "Components", EditAnywhere, BlueprintReadWrite)
	class UWidgetComponent* LockOnWidgetComponent;

	UPROPERTY(Category = "Components", EditAnywhere, BlueprintReadWrite)
	class UWidgetComponent* DeathblowWidgetComponent;

	TSubclassOf<UUserWidget> LockOnIconWidgetClass;
	TSubclassOf<UUserWidget> DeathblowIconWidgetClass;

	MonsterHitState HitState = MonsterHitState::OFFGUARD;

	FTimerHandle AttackMoveTimerHandle;

	// 체간 회복량
	const float MaxPostureRecoveryAmount = 0.2f;
	float PostureRecoveryAmount = 0.f;
};
