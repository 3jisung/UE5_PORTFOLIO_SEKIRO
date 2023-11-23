// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GlobalCharacter.h"
#include "Data/PlayerStatData.h"
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

	bool LockOn = false;
};
