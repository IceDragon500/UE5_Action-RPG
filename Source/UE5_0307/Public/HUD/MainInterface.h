// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainInterface.generated.h"

/**
 * 
 */

class UProgressBar;
class UTextBlock;

UCLASS()
class UE5_0307_API UMainInterface : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetHealthProgress(float percent);
	void SetEnergyProgress(float percent);
	void SetTextGold(int Gold);
	void SetTextSouls(int Souls);

protected:

private:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* Progress_Health;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* Progress_Energy;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextGold;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextSouls;
};
