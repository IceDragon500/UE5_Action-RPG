// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MainHUD.generated.h"

class UMainInterface;

UCLASS()
class UE5_0307_API AMainHUD : public AHUD
{
	GENERATED_BODY()

public:
	FORCEINLINE UMainInterface* GetMainInterface() const { return MainInterface; }

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = MainInterface)
	TSubclassOf<UMainInterface> MainInterfaceClass;
	UPROPERTY()
	UMainInterface* MainInterface;
	
	
};
