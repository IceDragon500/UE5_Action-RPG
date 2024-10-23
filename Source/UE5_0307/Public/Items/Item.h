// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

class USphereComponent;
class UNiagaraComponent;
class UNiagaraSystem;

//装备当前的状态
UENUM(BlueprintType)
enum class EItemState : uint8 {
	EIS_Hovering, //在地面上未被拾取
	EIS_Equipped //已经被装备
};

UCLASS()
class UE5_0307_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	float Amplitude = 0.25f;
	float TimeConstant = 5.f;
	float TransformedSin();
	float TransformedCos();

	


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* ItemMesh;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* Sphere;

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void SpawnPickEffect();

	virtual void SpawnPickSound();

	UPROPERTY(VisibleAnywhere)
	EItemState ItemState = EItemState::EIS_Hovering;
	//装备自身的特效
	UPROPERTY(EditAnywhere, Category = "PickUp")
	UNiagaraComponent* ItemEffect;
	//拾取装备的声音
	UPROPERTY(EditAnywhere, Category = "PickUp")
	USoundBase* PickSound;
	//拾取装备的特效
	UPROPERTY(EditAnywhere, Category = "PickUp")
	UNiagaraSystem* PickupEffect;

private:
	
	float RunningTime;

};
