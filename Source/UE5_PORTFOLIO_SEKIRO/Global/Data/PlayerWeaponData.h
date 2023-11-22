// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "UObject/NoExportTypes.h"
#include "PlayerWeaponData.generated.h"


USTRUCT(BlueprintType)
struct UE5_PORTFOLIO_SEKIRO_API FPlayerWeaponData : public FTableRowBase
{
	GENERATED_BODY()

	FPlayerWeaponData() {}
	~FPlayerWeaponData() {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	class UStaticMesh* Mesh;
};
