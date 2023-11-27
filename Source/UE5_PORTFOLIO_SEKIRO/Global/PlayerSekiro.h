// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GlobalCharacter.h"
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
	void LockOnTarget();

	TArray<AActor*> TraceObjects(TArray<AActor*> _ActorsToNotTargeting);

	UFUNCTION(BlueprintCallable)
	void ResearchLockOnTarget(float Rate);

	void ToggleLockOn();
	

	const struct FPlayerStatData* StatData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* WeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UStaticMesh*> WeaponArrays;


protected:
	void BeginPlay() override;
	void Tick(float _Delta) override;
	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


private:
	UPROPERTY(Category = "Components", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArmComponent;

	UPROPERTY(Category = "Components", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* CameraComponent;

	float Speed = 500.0f;

	UPROPERTY(Category = "LockOn", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bLockOn = false;

	float LockOnRange = 3000.0f;
	float LockOnAngle = 30.0f;
	AMonster* LockedOnTarget;
	
	bool bResearchEnable = true;
};
