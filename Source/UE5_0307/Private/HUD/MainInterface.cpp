// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/MainInterface.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UMainInterface::SetHealthProgress(float percent)
{
	if (Progress_Health)
	{
		Progress_Health->SetPercent(percent);
	}
}

void UMainInterface::SetEnergyProgress(float percent)
{
	if (Progress_Energy)
	{
		Progress_Energy->SetPercent(percent);
	}
}

void UMainInterface::SetTextGold(int Gold)
{
	if (TextGold)
	{
		TextGold->SetText(FText::FromString(FString::FromInt(Gold)));
	}
}

void UMainInterface::SetTextSouls(int Souls)
{
	if (TextSouls)
	{
		TextSouls->SetText(FText::FromString(FString::FromInt(Souls)));
	}
}
