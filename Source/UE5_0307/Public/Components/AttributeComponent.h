// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE5_0307_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UAttributeComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//血量
	float GetHealthPercent();//返回血量百分比
	void ReceiveDamage(float Damage);//根据传入的伤害 更新健康值

	bool IsAlive(); //判断角色是否死亡

//能量
	float GetEnergyPercent();//返回能量百分比
	void UseEnergy(float EnergyCost);//使用能量值
	void RecevieEnergy(float DeltaTime);//更新能量值

	FORCEINLINE float GetDodgeCost() const { return DodgeCost; }
	FORCEINLINE float GetEnerge() const { return Energy; }


	//金币和灵魂
	FORCEINLINE int32 GetGolds() const { return Golds; }
	void AddGolds(int32 GoldsNumber);
	FORCEINLINE int32 GetSouls() const { return Souls; }
	void AddSouls(int32 SoulNumber);

protected:

	virtual void BeginPlay() override;


private:
//HP
	//当前生命值
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Health = 100;
	//最大生命值
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxHealthl = 100;
//能量
	//当前能量
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Energy = 100;
	//最大能量
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxEnergy = 100;

//金币灵魂
	//金币
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 Golds = 0;
	//灵魂
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 Souls = 0;
//闪避消耗
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float DodgeCost = 14.f;
//能量回复
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float EnergyRegenRate = 8.f;
	
		
};
