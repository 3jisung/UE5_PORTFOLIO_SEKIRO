// Fill out your copyright notice in the Description page of Project Settings.


#include "GlobalGameInstance.h"
#include "Data/PlayerStatData.h"
#include "Data/PlayerMeshData.h"
#include "Data/PlayerAnimData.h"
#include "Data/MonsterData.h"
#include "Data/SoundData.h"
#include "Data/WidgetClassData.h"
#include "Data/ImageData.h"


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

	{
		FString DataPath = TEXT("/Script/Engine.DataTable'/Game/Blueprint/Global/Data/DT_SoundData.DT_SoundData'");
		ConstructorHelpers::FObjectFinder<UDataTable> DataTable(*DataPath);

		if (DataTable.Succeeded())
		{
			SoundData = DataTable.Object;
		}
	}

	{
		FString DataPath = TEXT("/Script/Engine.DataTable'/Game/Blueprint/Global/Data/DT_WidgetClassData.DT_WidgetClassData'");
		ConstructorHelpers::FObjectFinder<UDataTable> DataTable(*DataPath);

		if (DataTable.Succeeded())
		{
			WidgetClassData = DataTable.Object;
		}
	}

	{
		FString DataPath = TEXT("/Script/Engine.DataTable'/Game/Blueprint/Global/Data/DT_ImageData.DT_ImageData'");
		ConstructorHelpers::FObjectFinder<UDataTable> DataTable(*DataPath);

		if (DataTable.Succeeded())
		{
			ImageData = DataTable.Object;
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

USoundBase* UGlobalGameInstance::GetSoundData(FName _RowName, FName _ColumnName)
{
	if (nullptr == SoundData)
	{
		return nullptr;
	}

	FSoundData* FindTable = SoundData->FindRow<FSoundData>(_RowName, _RowName.ToString());

	if (nullptr == FindTable)
	{
		return nullptr;
	}

	if (FindTable->GameSound.Contains(_ColumnName))
	{
		return FindTable->GameSound.FindRef(_ColumnName);
	}
	else
	{
		return nullptr;
	}
}

TSubclassOf<UUserWidget> UGlobalGameInstance::GetWidgetClassData(FName _RowName, FName _ColumnName)
{
	if (nullptr == WidgetClassData)
	{
		return nullptr;
	}

	FWidgetClassData* FindTable = WidgetClassData->FindRow<FWidgetClassData>(_RowName, _RowName.ToString());

	if (nullptr == FindTable)
	{
		return nullptr;
	}

	if (FindTable->GameWidgetClass.Contains(_ColumnName))
	{
		return FindTable->GameWidgetClass.FindRef(_ColumnName);
	}
	else
	{
		return nullptr;
	}
}

TArray<class UTexture2D*> UGlobalGameInstance::GetImageData(FName _Name)
{
	TArray<class UTexture2D*> EmptyTArray;

	if (nullptr == ImageData)
	{
		return EmptyTArray;
	}

	FImageData* FindTable = ImageData->FindRow<FImageData>(_Name, _Name.ToString());

	if (nullptr == FindTable)
	{
		return EmptyTArray;
	}

	return FindTable->GameImage;
}