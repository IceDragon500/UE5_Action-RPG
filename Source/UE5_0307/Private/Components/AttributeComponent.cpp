// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AttributeComponent.h"

// Sets default values for this component's properties
UAttributeComponent::UAttributeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

bool UAttributeComponent::IsAlive()
{
	return Health > 0.f ;
}

void UAttributeComponent::AddGolds(int32 GoldsNumber)
{
	Golds += GoldsNumber;
}

void UAttributeComponent::AddSouls(int32 SoulNumber)
{
	Souls += SoulNumber;
}


// Called every frame
void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

float UAttributeComponent::GetHealthPercent()
{
	return Health / MaxHealthl;
}

void UAttributeComponent::ReceiveDamage(float Damage)
{
	//Health -= Damage;  
	//����д ��Ҫȥ���Ƿ�С��0���жϣ������ڼ������������������ʱ�򱨴���������ѡ���������ַ�ʽ

	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealthl);
	//ʹ��Clamp�����Խ����������һ��ָ���ķ�Χ������ר��ȥ���ж�

}

float UAttributeComponent::GetEnergyPercent()
{
	return Energy / MaxEnergy;
}

void UAttributeComponent::UseEnergy(float EnergyCost)
{
	Energy = FMath::Clamp(Energy - EnergyCost, 0.f, MaxEnergy);
}

void UAttributeComponent::RecevieEnergy(float DeltaTime)
{
	Energy = FMath::Clamp(Energy + EnergyRegenRate * DeltaTime, 0.f, MaxEnergy);
}

