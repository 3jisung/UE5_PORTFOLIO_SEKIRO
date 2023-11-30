// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "UObject/NoExportTypes.h"
#include "PlayerMeshData.generated.h"


USTRUCT(BlueprintType)
struct UE5_PORTFOLIO_SEKIRO_API FPlayerMeshData : public FTableRowBase
{
	GENERATED_BODY()

	FPlayerMeshData() {}
	~FPlayerMeshData() {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	class UStaticMesh* Mesh;
};
