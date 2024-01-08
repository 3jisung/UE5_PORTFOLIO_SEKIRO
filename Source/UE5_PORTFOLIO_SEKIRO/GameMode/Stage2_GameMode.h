// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Components/AudioComponent.h"
#include "Stage2_GameMode.generated.h"

/**
 * 
 */
UCLASS()
class UE5_PORTFOLIO_SEKIRO_API AStage2_GameMode : public AGameMode
{
	GENERATED_BODY()
	

public:
	AStage2_GameMode();

	void ModifySound(int index, float ChangeDuration = 0.f);

	void PlaySound()
	{
		AudioComponent->Play();
		AudioComponent->FadeIn(FadeInOutDuration);
	}

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


private:
	UPROPERTY(Category = "Sound", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TArray<class USoundBase*> AllSound;

	UPROPERTY(Category = "Components", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* AudioComponent;

	int NewSoundIndex = -1;
	float FadeInOutDuration = 1.f;
};
