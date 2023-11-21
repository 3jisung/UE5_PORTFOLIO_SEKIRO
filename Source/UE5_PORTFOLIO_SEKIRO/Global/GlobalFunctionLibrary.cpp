// Fill out your copyright notice in the Description page of Project Settings.


#include "GlobalFunctionLibrary.h"

FRandomStream UGlobalFunctionLibrary::MainRandom;

UWorld* UGlobalFunctionLibrary::GetCurrentWorld()
{
	// 언리얼 엔진 그자체에 대한 기능을 제공하는 포인터
	// GEngine->GetWorld();

	// 월드가 동시에 여러개가 존재할때가 있다.

	for (const FWorldContext& Context : GEngine->GetWorldContexts())
	{
		EWorldType::Type GetWorldType = Context.WorldType;

		if (nullptr == Context.World())
		{
			continue;
		}

		FString Name = Context.World()->GetName();

		// UE_LOG(LogTemp, Error, TEXT("%s"), *Name);

		switch (GetWorldType)
		{
		case EWorldType::None:
			break;
		case EWorldType::Game:
			break;
		case EWorldType::Editor:
			break;
		case EWorldType::PIE:
			return Context.World();
		case EWorldType::EditorPreview:
			break;
		case EWorldType::GamePreview:
			break;
		case EWorldType::GameRPC:
		case EWorldType::Inactive:
			break;
		default:
			break;
		}
	}

	return nullptr;
}