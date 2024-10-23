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
	//创建武器的碰撞盒
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponBox"));
	WeaponBox->SetupAttachment(GetRootComponent());
	//设置碰撞检测的属性，这个对应蓝图设置中的碰撞设置
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);//设置触发重叠事件
	WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);//先设置所有碰撞对象都为可重叠检测
	WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);//单独设置pawn为忽略

	//创建检测Scene的起点和终点
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

	if (OutHit.GetActor())//先判断获取的actor是否有效
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
		CreateFields(OutHit.ImpactPoint);//在蓝图中 传入命中位置的坐标，在坐标处实现FieldSystem		
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
	IgnoreActors.AddUnique(BoxHit.GetActor());//判断只命中一次，命中之后就将其添加至失效名单ActorsToIgnore

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
	return GetOwner()->ActorHasTag(TEXT("Enemy")) && OtherActor->ActorHasTag(TEXT("Enemy"));  //如果使用者是enemy 而且 被攻击者也是enemy 则跳出
}

void AWeapons::ExecuteGetHit(FHitResult& OutHit)
{
	IHitInterface* HitInterface = Cast<IHitInterface>(OutHit.GetActor());
	if (HitInterface)//再判断获取的接口是否有效
	{
		HitInterface->Execute_GetHit(OutHit.GetActor(), OutHit.ImpactPoint, GetOwner());
	}
}

void AWeapons::Equip(USceneComponent* InParent, FName InSocketName, AActor* NewActor, APawn* NewInstigator)
{
	ItemState = EItemState::EIS_Equipped;//将道具的状态设置为已装备
	SetOwner(NewActor);//设置持有武器的actor
	SetInstigator(NewInstigator);//设置持有武器的pawn

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
