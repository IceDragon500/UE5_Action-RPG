// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/HitInterface.h"
#include "BreakableActor.generated.h"

class UGeometryCollectionComponent;

UCLASS()
class UE5_0307_API ABreakableActor : public AActor, public IHitInterface
{
	GENERATED_BODY()
	
public:	

	ABreakableActor();
	virtual void Tick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;

public:	

	void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UCapsuleComponent* Capsule;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UGeometryCollectionComponent* GeometryCollection;

private:


	UPROPERTY(EditAnywhere, Category = Sounds)
	USoundBase* HitSound;

//掉落功能
	UPROPERTY(EditAnywhere, Category = Treasure)
	TArray<TSubclassOf<class ATreasure>> TreasureArray;

	//掉落之后的高度
	UPROPERTY(EditAnywhere, Category = Treasure)
	double LoopHight = 10;

	bool bBroken = false;

	//UPROPERTY(EditAnywhere, Category = Treasure)
	//TSubclassOf<class ATreasure> TreasureClass;
};
