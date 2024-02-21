// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GlobalFunctionLibrary.h"
#include "Engine/DataTable.h"
#include "GlobalEnums.h"
#include "GlobalGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class UE5_PORTFOLIO_SEKIRO_API UGlobalGameInstance : public UGameInstance
{
	GENERATED_BODY()
	

public:
	UGlobalGameInstance();

	struct FPlayerStatData* GetPlayerStat(FName _Name);
	UStaticMesh* GetPlayerMesh(FName _Name);
	struct FPlayerAnimData* GetPlayerAnim(FName _Name);
	struct FMonsterData* GetMonster(FName _Name);
	USoundBase* GetSoundData(FName _RowName, FName _ColumnName);
	TSubclassOf<UUserWidget> GetWidgetClassData(FName _RowName, FName _ColumnName);
	TArray<class UTexture2D*> GetImageData(FName _Name);
	UParticleSystem* GetEffect(FName _Name);


private:
	UPROPERTY()
	UDataTable* PlayerStatData;

	UPROPERTY()
	UDataTable* PlayerMeshData;

	UPROPERTY()
	UDataTable* PlayerAnimData;

	UPROPERTY()
	UDataTable* MonsterData;

	UPROPERTY()
	UDataTable* SoundData;

	UPROPERTY()
	UDataTable* WidgetClassData;

	UPROPERTY()
	UDataTable* ImageData;

	UPROPERTY()
	UDataTable* EffectData;
};
