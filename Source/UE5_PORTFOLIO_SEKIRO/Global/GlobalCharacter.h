// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GlobalAnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "Engine/DamageEvents.h"
#include "GlobalCharacter.generated.h"

UCLASS()
class UE5_PORTFOLIO_SEKIRO_API AGlobalCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGlobalCharacter();

	float GetHP()
	{
		return HP;
	}

	void SetHP(float _HP)
	{
		HP = _HP;
	}

	float GetPosture()
	{
		return Posture;
	}

	void SetPosture(float _Posture)
	{
		Posture = _Posture;
	}

	float GetPower()
	{
		return Power;
	}

	void SetPower(float _Power)
	{
		Power = _Power;
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
	void SetAllAnimation(const TMap<EnumType, class UAnimMontage*>& _Animation)
	{
		for (TPair<EnumType, UAnimMontage*> Pair : _Animation)
		{
			AllAnimations.Add(static_cast<int>(Pair.Key), Pair.Value);
		}
	}

	template<typename EnumType>
	void SetAllSound(const TMap<EnumType, class USoundBase*>& _Sound)
	{
		for (TPair<EnumType, USoundBase*> Pair : _Sound)
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


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	UPROPERTY(Category = "GlobalCharacterValue", EditAnywhere, BlueprintReadWrite)
	TMap<int, class UAnimMontage*> AllAnimations;

	UPROPERTY(Category = "GlobalChracterValue", EditAnywhere, BlueprintReadWrite)
	TMap<int, class USoundBase*> AllSound;

	UPROPERTY(Category = "GlobalCharacterValue", EditAnywhere, BlueprintReadWrite)
	float HP = 0;

	UPROPERTY(Category = "GlobalCharacterValue", EditAnywhere, BlueprintReadWrite)
	float MaxHP = 0;

	UPROPERTY(Category = "GlobalCharacterValue", EditAnywhere, BlueprintReadWrite)
	float Posture = 0;

	UPROPERTY(Category = "GlobalCharacterValue", EditAnywhere, BlueprintReadWrite)
	float MaxPosture = 0;

	UPROPERTY(Category = "GlobalCharacterValue", EditAnywhere, BlueprintReadWrite)
	float Power = 0;

	// 피격 이벤트 시 사용할 충돌 관리 변수
	bool bCollisionActor = false;


private:
	UPROPERTY(Category = "GlobalCharacterValue", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int AniState = 0;

	class UGlobalAnimInstance* GlobalAnimInstance = nullptr;

};
