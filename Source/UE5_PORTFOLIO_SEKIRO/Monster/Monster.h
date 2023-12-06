// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Global/GlobalCharacter.h"
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

	void LockOnIconOnOff(bool bLockOn);


protected:
	UPROPERTY(Category = "Monster", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int DeathblowCount;


private:
	UPROPERTY(Category = "Components", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* WidgetComponent;

	TSubclassOf<UUserWidget> LockOnIconWidgetClass;
};
