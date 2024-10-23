// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Soul.h"
#include "Interface/PickupInterface.h"



void ASoul::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IPickupInterface* PickInterface = Cast<IPickupInterface>(OtherActor);//将传入的AActor* OtherActor转成EchoAcharatcer，需要识别参与碰撞的是我们控制的角色类
	if (PickInterface)
	{
		PickInterface->AddSouls(this);

		SpawnPickEffect();
		SpawnPickSound();
		
		Destroy();
	}
}
