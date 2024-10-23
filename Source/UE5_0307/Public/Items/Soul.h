// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Soul.generated.h"

class UNiagaraSystem;

UCLASS()
class UE5_0307_API ASoul : public AItem
{
	GENERATED_BODY()
	

public:
	FORCEINLINE int32 GetSouls() const { return Souls; }
	FORCEINLINE void SetSouls(int32 SoulNum) { Souls = SoulNum; }
protected:

	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

private:
	//灵魂的数量
	UPROPERTY(EditAnywhere, Category = "Souls")
	int32 Souls = 1;
	
};
