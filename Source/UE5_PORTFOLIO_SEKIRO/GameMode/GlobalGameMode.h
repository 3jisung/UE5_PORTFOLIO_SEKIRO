// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Components/AudioComponent.h"
#include "../Global/GlobalGameInstance.h"
#include "GlobalGameMode.generated.h"

/**
 * 
 */
UCLASS()
class UE5_PORTFOLIO_SEKIRO_API AGlobalGameMode : public AGameMode
{
	GENERATED_BODY()
	

public:
	AGlobalGameMode();

	UFUNCTION(BlueprintCallable)
	void ModifySound(int index, float ChangeDuration = 0.f);

	UFUNCTION(BlueprintCallable)
	void PlaySound()
	{
		AudioComponent->Play();
		AudioComponent->FadeIn(FadeInOutDuration);
	}

	UFUNCTION(BlueprintCallable)
	void StopSound()
	{
		AudioComponent->FadeOut(FadeInOutDuration, 0.f);

		FTimerHandle myTimerHandle;
		float delayTime = FadeInOutDuration;
		GetWorld()->GetTimerManager().SetTimer(myTimerHandle, FTimerDelegate::CreateLambda([&]()
			{
				AudioComponent->Stop();

				GetWorld()->GetTimerManager().ClearTimer(myTimerHandle);
			}), delayTime, false);
	}

protected:
	// Called when the game starts or when spawned
	void BeginPlay() override;
	void Tick(float _Delta) override;


	UPROPERTY(Category = "Sound", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TArray<class USoundBase*> AllSound;

	UPROPERTY(Category = "Components", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* AudioComponent;

	int NewSoundIndex = -1;
	float FadeInOutDuration = 1.f;
};
