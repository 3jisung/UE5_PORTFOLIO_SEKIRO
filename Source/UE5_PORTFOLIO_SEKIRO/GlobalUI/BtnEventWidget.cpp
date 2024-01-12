// Fill out your copyright notice in the Description page of Project Settings.


#include "BtnEventWidget.h"


void UBtnEventWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UGlobalGameInstance* Inst = GetGameInstance<UGlobalGameInstance>();
	TArray<USoundBase*> AllSound = Inst->GetSoundData(TEXT("Global"));

	if (AllSound.Num() >= 2)
	{
		SoundEffects.Add("Hover", AllSound[0]);
		SoundEffects.Add("Select", AllSound[1]);
	}

	SetFocus();

	if (BtnHoveredImage.IsValidIndex(0))
	{
		BtnHoveredImage[0]->SetOpacity(1.f);
	}
}

bool UBtnEventWidget::SetHoveredBtn(int _Index)
{
	if (HoveredIndex != _Index && BtnHoveredImage.IsValidIndex(_Index))
	{
		BtnHoveredImage[HoveredIndex]->SetOpacity(0.f);
		BtnHoveredImage[_Index]->SetOpacity(1.f);

		UGameplayStatics::PlaySound2D(GetWorld(), SoundEffects.FindRef(TEXT("Hover")));

		HoveredIndex = _Index;

		return true;
	}
	else
	{
		return false;
	}
}

void UBtnEventWidget::MenuEvent()
{
	UGameplayStatics::PlaySound2D(GetWorld(), SoundEffects.FindRef(TEXT("Select")));
}