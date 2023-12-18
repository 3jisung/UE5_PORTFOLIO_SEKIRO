// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Global/GlobalCharacter.h"
#include "../Global/Data/MonsterData.h"
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

	void LockOnIconOnOff(bool bLockOn);

	UFUNCTION(BlueprintCallable)
	virtual void AttackMove() { UE_LOG(LogTemp, Error, TEXT("Monster AttackMove")); }

	UFUNCTION(BlueprintCallable)
	virtual void MontageEnd() { UE_LOG(LogTemp, Error, TEXT("Monster MontageEnd")); }

	UFUNCTION(BlueprintCallable)
	virtual void Damage() { UE_LOG(LogTemp, Error, TEXT("Monster Damage")); }

	const struct FMonsterData* MonsterData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* WeaponMesh;
	

protected:
	void BeginPlay() override;
	void Tick(float _Delta) override;

	UFUNCTION()
	void BeginOverLap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void EndOverLap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

	virtual bool IsGetHitCheck() { UE_LOG(LogTemp, Error, TEXT("Monster GetHitCheck")); return false; }
	virtual bool IsDeathCheck() { UE_LOG(LogTemp, Error, TEXT("Monster DeathCheck")); return false; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	class UBehaviorTree* BehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	class UBlackboardComponent* BlackboardComponent;

	UPROPERTY(Category = "Monster", EditAnywhere, BlueprintReadWrite)
	int DeathblowCount;

	UPROPERTY(Category = "Components", EditAnywhere, BlueprintReadWrite)
	class UWidgetComponent* WidgetComponent;

	TSubclassOf<UUserWidget> LockOnIconWidgetClass;

	MonsterHitState HitState = MonsterHitState::OFFGUARD;

	FTimerHandle AttackMoveTimerHandle;

	// 체간 회복량
	const float MaxPostureRecoveryAmount = 0.2f;
	float PostureRecoveryAmount = 0.f;
};
