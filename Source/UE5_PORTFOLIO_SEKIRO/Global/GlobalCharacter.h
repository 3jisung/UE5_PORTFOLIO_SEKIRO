// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GlobalAnimInstance.h"
#include "GlobalCharacter.generated.h"

UCLASS()
class UE5_PORTFOLIO_SEKIRO_API AGlobalCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGlobalCharacter();

	void SetHP(int _HP)
	{
		HP -= _HP;
	}

	void AddHP(int _HP)
	{
		HP += _HP;
	}

	int GetHP()
	{
		return HP;
	}

	int GetAniState()
	{
		return AniState;
	}

	template<typename EnumType>
	EnumType GetAniState()
	{
		return static_cast<EnumType>(AniState);
	}

	template<typename EnumType>
	void SetAniState(EnumType _AniState)
	{
		AniState = static_cast<int>(_AniState);
	}

	void SetAniState(int _AniState)
	{
		AniState = _AniState;
	}

	class UGlobalAnimInstance* GetGlobalAnimInstance()
	{
		return GlobalAnimInstance;
	}

	template<typename EnumType>
	class UAnimMontage* GetAnimMontage(EnumType _Index)
	{
		return GetAnimMontage(static_cast<int>(_Index));
	}

	class UAnimMontage* GetAnimMontage(int _Index)
	{
		if (false == AllAnimations.Contains(_Index))
		{
			return nullptr;
		}

		return AllAnimations[_Index];
	}

	template<typename EnumType>
	void SetAllAnimation(const TMap<EnumType, class UAnimMontage*>& _MapAnimation)
	{
		for (TPair<EnumType, UAnimMontage*> Pair : _MapAnimation)
		{
			AllAnimations.Add(static_cast<int>(Pair.Key), Pair.Value);
		}
	}

	template<typename EnumType>
	void SetAllSound(const TMap<EnumType, class USoundBase*>& _MapSound)
	{
		for (TPair<EnumType, USoundBase*> Pair : _MapSound)
		{
			AllSound.Add(static_cast<int>(Pair.Key), Pair.Value);
		}
	}

	template<typename EnumType>
	class USoundBase* GetSound(EnumType _Index)
	{
		if (false == AllSound.Contains(static_cast<int>(_Index)))
		{
			return nullptr;
		}

		return AllSound[static_cast<int>(_Index)];
	}

	void SetHitState();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	UPROPERTY(Category = "GlobalCharacterValue", EditAnywhere, BlueprintReadWrite)
	int HP = 0;

	UPROPERTY(Category = "GlobalCharacterValue", EditAnywhere, BlueprintReadWrite)
	int MaxHP = 0;

	UPROPERTY(Category = "GlobalCharacterValue", EditAnywhere, BlueprintReadWrite)
	int Posture = 0;

	UPROPERTY(Category = "GlobalCharacterValue", EditAnywhere, BlueprintReadWrite)
	float Power = 0;

	// 피격 가능 상태
	bool HitState = true;

	// 피격 무적 시간
	float HitIgnoreTime = 0.3;


private:
	UPROPERTY(Category = "GlobalCharacterValue", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int AniState = 0;

	UPROPERTY(Category = "GlobalCharacterValue", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TMap<int, class UAnimMontage*> AllAnimations;

	class UGlobalAnimInstance* GlobalAnimInstance = nullptr;

	UPROPERTY(Category = "GlobalChracterValue", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TMap<int, class USoundBase*> AllSound;
};
