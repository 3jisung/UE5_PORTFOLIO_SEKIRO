// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GlobalCharacter.h"
#include "GlobalEnums.h"
#include "Data/PlayerStatData.h"
#include "Monster.h"
#include "PlayerSekiro.generated.h"

/**
 * 
 */
UCLASS()
class UE5_PORTFOLIO_SEKIRO_API APlayerSekiro : public AGlobalCharacter
{
	GENERATED_BODY()
	

public:
	APlayerSekiro();

	UFUNCTION(BlueprintCallable)
	void MoveRight(float Val);

	UFUNCTION(BlueprintCallable)
	void MoveForward(float Val);

	UFUNCTION(BlueprintCallable)
	void PlayerJump();

	void Landed(const FHitResult& Hit) override;

	UFUNCTION(BlueprintCallable)
	void StartedDash();

	UFUNCTION(BlueprintCallable)
	void PlayerDash(bool ActionValue, float TriggeredSec);

	UFUNCTION(BlueprintCallable)
	void LockOnTarget();

	TArray<AActor*> TraceObjects(TArray<AActor*> _ActorsToNotTargeting);

	UFUNCTION(BlueprintCallable)
	void ResearchLockOnTarget(float Rate);

	void ToggleLockOn();

	UFUNCTION(BlueprintCallable)
	void PlayerAttackStarted();

	UFUNCTION(BlueprintCallable)
	void PlayerAttackTriggered(bool ActionValue, float TriggeredSec);

	UFUNCTION(BlueprintCallable)
	void AttackEnd();


	const struct FPlayerStatData* StatData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* WeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* GourdMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UStaticMesh*> StaticMeshArrays;


protected:
	void BeginPlay() override;
	void Tick(float _Delta) override;
	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


private:
	UFUNCTION()
	void MontageEnd(UAnimMontage* Anim, bool _Inter);

	UFUNCTION()
	void AnimNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	UFUNCTION()
	void AnimNotifyEnd(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	UPROPERTY(Category = "Components", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArmComponent;

	UPROPERTY(Category = "Components", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* CameraComponent;

	float Speed = 500.0f;

	UPROPERTY(Category = "LockOn", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bLockOn = false;

	UPROPERTY(Category = "InputWASD", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bInputWASD = false;

	float LockOnRange = 3000.0f;
	float LockOnAngle = 30.0f;
	AMonster* LockedOnTarget;
	
	bool bResearchEnable = true;

	PlayerHitState HitState = PlayerHitState::OFFGUARD;

	// 하단 무적 상태
	// 하단 무적은 다른 HitState와 중복 적용될 수 있으므로 bool 변수로 따로 관리한다.
	bool bLowInvincible = false;

	bool bDash = false;
	bool bStartedDash = false;
	float PreDashTime = 0.f;

	UPROPERTY(Category = "AttackEnable", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bAttackEnable = false;

	bool bAttackCombo = false;

	// 평타 순서 기억
	int BasicAttackCount = 0;
};
