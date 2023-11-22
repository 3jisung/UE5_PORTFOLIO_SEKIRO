// Fill out your copyright notice in the Description page of Project Settings.


#include "GlobalGameInstance.h"
#include "Data/PlayerStatData.h"
#include "Data/PlayerWeaponData.h"


UGlobalGameInstance::UGlobalGameInstance()
{
	{
		FString DataPath = TEXT("/Script/Engine.DataTable'/Game/Blueprint/Global/Data/DT_PlayerStatData.DT_PlayerStatData'");
		ConstructorHelpers::FObjectFinder<UDataTable> DataTable(*DataPath);

		if (DataTable.Succeeded())
		{
			PlayerStatData = DataTable.Object;
		}
	}

	{
		FString DataPath = TEXT("/Script/Engine.DataTable'/Game/Blueprint/Global/Data/DT_PlayerWeaponData.DT_PlayerWeaponData'");
		ConstructorHelpers::FObjectFinder<UDataTable> DataTable(*DataPath);

		if (DataTable.Succeeded())
		{
			PlayerWeaponData = DataTable.Object;
		}
	}

	UGlobalFunctionLibrary::MainRandom.GenerateNewSeed();
}

FPlayerStatData* UGlobalGameInstance::GetPlayerStat(FName _Name)
{
	if (nullptr == PlayerStatData)
	{
		return nullptr;
	}

	FPlayerStatData* FindTable = PlayerStatData->FindRow<FPlayerStatData>(_Name, _Name.ToString());

	if (nullptr == FindTable)
	{
		return nullptr;
	}

	return FindTable;
}

UStaticMesh* UGlobalGameInstance::GetPlayerWeapon(FName _Name)
{
	if (nullptr == PlayerWeaponData)
	{
		return nullptr;
	}

	FPlayerWeaponData* FindTable = PlayerWeaponData->FindRow<FPlayerWeaponData>(_Name, _Name.ToString());

	if (nullptr == FindTable)
	{
		return nullptr;
	}

	return FindTable->Mesh;
}