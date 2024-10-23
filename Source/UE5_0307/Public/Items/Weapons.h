// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Field/FieldSystemObjects.h"
#include "Weapons.generated.h"

class USoundBase;
class UBoxComponent;
//class UFieldSystemMetaDataFilter;
//class URadialFalloff;
class UFieldSystemComponent;
/**
 * 
 */
UCLASS()
class UE5_0307_API AWeapons : public AItem
{
	GENERATED_BODY()
public:
	AWeapons();

	void Equip(USceneComponent* InParent, FName InSocketName, AActor* NewActor, APawn* NewInstigator);
	void DeactivateEmbers();
	void DisableShpereCollision();

	void PlayEquipSound();

	void AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName);

	TArray<AActor*> IgnoreActors;

protected:

	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	bool ActorIsSameType(AActor* OtherActor);

	void ExecuteGetHit(FHitResult& OutHit);

	UFUNCTION(BlueprintImplementableEvent)  //在C++中声明（不能定义），在蓝图中重写
	void CreateFields(const FVector& FieldLocation); 

private:
	void BoxTrace(FHitResult& BoxHit);

	UPROPERTY(EditAnywhere, Category = "Weapon")
	FVector BoxTraceExtent = FVector(5.f);

	//是否显示武器的碰撞盒子
	UPROPERTY(EditAnywhere, Category = "Weapon")
	bool bShowWeaponBoxDebug = false;

	//拾取装备的声音
	UPROPERTY(EditAnywhere, Category = "Weapon")
	USoundBase* EquipSound;

	//武器的碰撞盒子
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	UBoxComponent* WeaponBox;

	//武器碰撞检测的起点
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	USceneComponent* StartPoint;

	//武器碰撞检测的终点
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	USceneComponent* EndPoint;

	//武器的伤害
	UPROPERTY(EditAnywhere, Category = "Weapon")
	float Damage = 20.f;

public:
	FORCEINLINE UBoxComponent* GetWeaponBox() const { return WeaponBox; }


};
