// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Treasure.h"
#include "Interface/PickupInterface.h"

void ATreasure::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IPickupInterface* PickInterface = Cast<IPickupInterface>(OtherActor);//�������AActor* OtherActorת��EchoAcharatcer����Ҫʶ�������ײ�������ǿ��ƵĽ�ɫ��
	if (PickInterface)
	{
		PickInterface->AddGolds(this);

		SpawnPickSound();
		Destroy();
	}
}
