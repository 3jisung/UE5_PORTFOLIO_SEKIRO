// Fill out your copyright notice in the Description page of Project Settings.


#include "GlobalGameInstance.h"
#include "Data/PlayerStatData.h"
#include "Data/PlayerMeshData.h"
#include "Data/PlayerAnimData.h"
#include "Data/MonsterData.h"


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
		FString DataPath = TEXT("/Script/Engine.DataTable'/Game/Blueprint/Global/Data/DT_PlayerMeshData.DT_PlayerMeshData'");
		ConstructorHelpers::FObjectFinder<UDataTable> DataTable(*DataPath);

		if (DataTable.Succeeded())
		{
			PlayerMeshData = DataTable.Object;
		}
	}

	{
		FString DataPath = TEXT("/Script/Engine.DataTable'/Game/Blueprint/Global/Data/DT_PlayerAnimData.DT_PlayerAnimData'");
		ConstructorHelpers::FObjectFinder<UDataTable> DataTable(*DataPath);

		if (DataTable.Succeeded())
		{
			PlayerAnimData = DataTable.Object;
		}
	}

	{
		FString DataPath = TEXT("/Script/Engine.DataTable'/Game/Blueprint/Global/Data/DT_MonsterData.DT_MonsterData'");
		ConstructorHelpers::FObjectFinder<UDataTable> DataTable(*DataPath);

		if (DataTable.Succeeded())
		{
			MonsterData = DataTable.Object;
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

UStaticMesh* UGlobalGameInstance::GetPlayerMesh(FName _Name)
{
	if (nullptr == PlayerMeshData)
	{
		return nullptr;
	}

	FPlayerMeshData* FindTable = PlayerMeshData->FindRow<FPlayerMeshData>(_Name, _Name.ToString());

	if (nullptr == FindTable)
	{
		return nullptr;
	}

	return FindTable->Mesh;
}

FPlayerAnimData* UGlobalGameInstance::GetPlayerAnim(FName _Name)
{
	if (nullptr == PlayerAnimData)
	{
		return nullptr;
	}

	FPlayerAnimData* FindTable = PlayerAnimData->FindRow<FPlayerAnimData>(_Name, _Name.ToString());

	if (nullptr == FindTable)
	{
		return nullptr;
	}

	return FindTable;
}

FMonsterData* UGlobalGameInstance::GetMonster(FName _Name)
{
	if (nullptr == MonsterData)
	{
		return nullptr;
	}

	FMonsterData* FindTable = MonsterData->FindRow<FMonsterData>(_Name, _Name.ToString());

	if (nullptr == FindTable)
	{
		return nullptr;
	}

	return FindTable;
}