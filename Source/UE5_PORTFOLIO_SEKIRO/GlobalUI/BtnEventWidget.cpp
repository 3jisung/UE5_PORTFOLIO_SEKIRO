// Fill out your copyright notice in the Description page of Project Settings.


#include "BtnEventWidget.h"


void UBtnEventWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UGlobalGameInstance* Inst = GetGameInstance<UGlobalGameInstance>();
	USoundBase* Hover = Inst->GetSoundData(TEXT("Global"), TEXT("Hover"));
	USoundBase* Select = Inst->GetSoundData(TEXT("Global"), TEXT("Select"));

	if (IsValid(Hover), IsValid(Select))
	{
		SoundEffects.Add("Hover", Hover);
		SoundEffects.Add("Select", Select);
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