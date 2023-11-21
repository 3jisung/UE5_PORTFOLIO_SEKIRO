// Fill out your copyright notice in the Description page of Project Settings.


#include "GlobalGameInstance.h"


UGlobalGameInstance::UGlobalGameInstance()
{
	UGlobalFunctionLibrary::MainRandom.GenerateNewSeed();
}