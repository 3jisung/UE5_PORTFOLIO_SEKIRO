// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Player/PlayerWidget/MainWidget.h"
#include "GlobalHUD.generated.h"

/**
 * 
 */
UCLASS()
class UE5_PORTFOLIO_SEKIRO_API AGlobalHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	AGlobalHUD() {};
	~AGlobalHUD() {};

	UFUNCTION(BlueprintCallable)
	UMainWidget* GetMainWidget()
	{
		return MainWidget;
	}

	UFUNCTION(BlueprintCallable)
	void SetMainWidget(UUserWidget* _MainWidget)
	{
		UMainWidget* Widget = Cast<UMainWidget>(_MainWidget);
		if (IsValid(Widget))
		{
			MainWidget = Widget;
		}
	}

protected:
	void BeginPlay() override;
	void Tick(float _Delta) override;

private:
	UMainWidget* MainWidget = nullptr;
};
