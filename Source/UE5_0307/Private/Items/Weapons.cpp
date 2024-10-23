// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/Weapons.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interface/HitInterface.h"
#include "NiagaraComponent.h"

AWeapons::AWeapons()
{
	//������������ײ��
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponBox"));
	WeaponBox->SetupAttachment(GetRootComponent());
	//������ײ�������ԣ������Ӧ��ͼ�����е���ײ����
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);//���ô����ص��¼�
	WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);//������������ײ����Ϊ���ص����
	WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);//��������pawnΪ����

	//�������Scene�������յ�
	StartPoint = CreateDefaultSubobject<USceneComponent>(TEXT("StartPoint"));
	StartPoint->SetupAttachment(GetRootComponent());
	EndPoint = CreateDefaultSubobject<USceneComponent>(TEXT("EndPoint"));
	EndPoint->SetupAttachment(GetRootComponent());
}

void AWeapons::BeginPlay()
{
	Super::BeginPlay();
	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapons::OnBoxOverlap);
}

void AWeapons::AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName)
{
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	ItemMesh->AttachToComponent(InParent, TransformRules, InSocketName);
}

void AWeapons::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	
	if (ActorIsSameType(OtherActor)) return;

	FHitResult OutHit;
	BoxTrace(OutHit);

	if (OutHit.GetActor())//���жϻ�ȡ��actor�Ƿ���Ч
	{
		//if(ActorIsSameType(OutHit.GetActor())) return;

		UGameplayStatics::ApplyDamage(
			OutHit.GetActor(),
			Damage,
			GetInstigator()->GetController(),
			this,
			UDamageType::StaticClass()
		);

		FString actorname = OtherActor->GetActorNameOrLabel();
		FString Hitstring = OutHit.ToString();

		ExecuteGetHit(OutHit);
		CreateFields(OutHit.ImpactPoint);//����ͼ�� ��������λ�õ����꣬�����괦ʵ��FieldSystem		
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("OnBoxOverlap miss"));
	}
	
}

void AWeapons::BoxTrace(FHitResult& BoxHit)
{
	const FVector Start = StartPoint->GetComponentLocation();
	const FVector End = EndPoint->GetComponentLocation();

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	ActorsToIgnore.Add(GetOwner());

	for (AActor* Actor : IgnoreActors)
	{
		ActorsToIgnore.AddUnique(Actor);
	}

	UKismetSystemLibrary::BoxTraceSingle(
		this,
		Start,
		End,
		BoxTraceExtent,
		StartPoint->GetComponentRotation(),
		//FRotator::ZeroRotator,
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		ActorsToIgnore,
		bShowWeaponBoxDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		BoxHit,
		true
	);
	IgnoreActors.AddUnique(BoxHit.GetActor());//�ж�ֻ����һ�Σ�����֮��ͽ��������ʧЧ����ActorsToIgnore

	/*UKismetSystemLibrary::LineTraceSingle(
		this,
		Start,
		End,
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		OutHit,
		true
	);*/
}

bool AWeapons::ActorIsSameType(AActor* OtherActor)
{
	return GetOwner()->ActorHasTag(TEXT("Enemy")) && OtherActor->ActorHasTag(TEXT("Enemy"));  //���ʹ������enemy ���� ��������Ҳ��enemy ������
}

void AWeapons::ExecuteGetHit(FHitResult& OutHit)
{
	IHitInterface* HitInterface = Cast<IHitInterface>(OutHit.GetActor());
	if (HitInterface)//���жϻ�ȡ�Ľӿ��Ƿ���Ч
	{
		HitInterface->Execute_GetHit(OutHit.GetActor(), OutHit.ImpactPoint, GetOwner());
	}
}

void AWeapons::Equip(USceneComponent* InParent, FName InSocketName, AActor* NewActor, APawn* NewInstigator)
{
	ItemState = EItemState::EIS_Equipped;//�����ߵ�״̬����Ϊ��װ��
	SetOwner(NewActor);//���ó���������actor
	SetInstigator(NewInstigator);//���ó���������pawn

	AttachMeshToSocket(InParent, InSocketName);

	DisableShpereCollision();
	PlayEquipSound();
	DeactivateEmbers();
}

void AWeapons::DeactivateEmbers()
{
	if (ItemEffect)
	{
		ItemEffect->Deactivate();
	}
}

void AWeapons::DisableShpereCollision()
{
	if (Sphere)
	{
		Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AWeapons::PlayEquipSound()
{
	if (EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			EquipSound,
			GetActorLocation()
		);
	}
}
