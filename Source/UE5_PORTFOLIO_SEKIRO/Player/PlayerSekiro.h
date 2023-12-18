// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Global/GlobalCharacter.h"
#include "../Global/GlobalEnums.h"
#include "../Global/Data/PlayerStatData.h"
#include "../Monster/Monster.h"
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

	float TakeDamage(float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser) override;

	void GetHitExecute(float DamageAmount, UCustomDamageTypeBase* DamageType, AActor* DamageCauser);
	void ExhaustAction();
	void DeathAction();

	void PostureRecoveryManagerTimer();

	UFUNCTION(BlueprintCallable)
	void MoveForward(float Val);

	UFUNCTION(BlueprintCallable)
	void MoveRight(float Val);

	UFUNCTION(BlueprintCallable)
	void PlayerJump();

	void Landed(const FHitResult& Hit) override;

	UFUNCTION(BlueprintCallable)
	void StartedPlayerDash();

	void StartedPlayerDashMove();

	UFUNCTION(BlueprintCallable)
	void TriggeredPlayerDash(bool ActionValue, float TriggeredSec);

	UFUNCTION(BlueprintCallable)
	void StartedPlayerGuard();

	UFUNCTION(BlueprintCallable)
	void TriggeredPlayerGuard(bool ActionValue, float TriggeredSec);

	void ManageParryingTimer();

	void ManageGuardTimer();

	UFUNCTION(BlueprintCallable)
	void LockOnTarget();

	TArray<AActor*> TraceObjects(TArray<AActor*> _ActorsToNotTargeting);

	UFUNCTION(BlueprintCallable)
	void ResearchLockOnTarget(float Rate);

	void ToggleLockOn();

	UFUNCTION(BlueprintCallable)
	void StartedPlayerAttack();

	UFUNCTION(BlueprintCallable)
	void TriggeredPlayerAttack(bool ActionValue, float TriggeredSec);

	void DashAttackMove();

	void AttackMove();

	UFUNCTION(BlueprintCallable)
	void MontageEnd();

	UFUNCTION(BlueprintCallable)
	void AttackBegin();

	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	UFUNCTION(BlueprintCallable)
	void AttackComboBegin();

	UFUNCTION(BlueprintCallable)
	void AttackComboEnd();

	UFUNCTION(BlueprintCallable)
	void DashAttackMoveEnd();

	UFUNCTION(BlueprintCallable)
	void CheckBufferedInput();


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


private:
	UFUNCTION()
	void MontageBlendingOut(UAnimMontage* Anim, bool _Inter);

	UPROPERTY(Category = "Components", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArmComponent;

	UPROPERTY(Category = "Components", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* CameraComponent;

	float Speed = 0.f;
	const float DefaultSpeed = 300.0f;
	const float DashSpeed = 900.0f;

	UPROPERTY(Category = "LockOn", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bLockOn = false;

	UPROPERTY(Category = "InputWASD", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bInputWASD = false;

	const float LockOnRange = 2500.0f;
	const float LockOnAngle = 30.0f;
	AMonster* LockedOnTarget;
	
	bool bResearchEnable = true;

	PlayerHitState HitState = PlayerHitState::OFFGUARD;

	// 하단 무적 상태
	// 하단 무적은 다른 HitState와 중복 적용될 수 있으므로 bool 변수로 따로 관리한다.
	bool bLowInvincible = false;

	bool bDash = false;
	bool bStartedDash = false;
	float PreDashTime = 0.f;

	bool bAttackEnable = false;
	bool bAttackCombo = false;

	// 평타 순서 기억
	int BasicAttackCount = 0;
	// 패링 순서 기억(1, 2 애니메이션 번갈아 가면서 실행)
	int ParryingCount = 0;

	bool bDashAttackMove = false;

	// 선입력 관련 변수
	SekiroState BufferedAction;
	bool bEnteredTransition = false;

	bool bBufferedCompletedAttack = false;
	bool bBufferedCompletedDash = false;
	bool bBufferedCompletedGuard = false;
	// 선입력을 고려한 TriggeredTime 보정값
	float CorrectedTime = 0.f;
	// 공격 연타 시간 제한
	bool bAttackValid = true;

	float ParryingValidTime = 0.2f;
	const float MaxGuardValidTime = 0.4f;
	float PreGuardTime = 0.f;
	bool bGuardTimer = false;

	// 뇌반 실패 시 받을 데미지 저장
	float SavedDamage = 0.f;

	// 체간 회복량
	const float MaxPostureRecoveryAmount = 0.2f;
	float PostureRecoveryAmount = 0.f;

	FTimerHandle StartedDashTimerHandle;
	FTimerHandle DashAttackMoveTimerHandle;
	FTimerHandle AttackMoveTimerHandle;
	FTimerHandle ParryingTimerHandle;
	FTimerHandle GuardTimerHandle;
};
