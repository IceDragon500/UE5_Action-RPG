// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BaseCharacter.h"
#include "Components/BoxComponent.h"
#include "Items/Weapons.h"
#include "Components/AttributeComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"



ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	//�����Զ�����������
	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);//���öԽ����岻��ײ
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

}

void ABaseCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	if (Attributes && Attributes->IsAlive() && Hitter)//�����ж��Ƿ�����򲥷��ܻ����������򲥷���������
	{
		//��Hitter�����괫�룬�����ܻ�����
		DirectionalHitReact(Hitter->GetActorLocation());
	}
	else
	{
		//�����ر�Ѫ��
		//HealthBarWidget->SetVisibility(false);
		//�ر�SkeletaMesh����ײ������������ʬ�廹�ǻᱻ�������У�����������������
		GetMesh()->SetGenerateOverlapEvents(false);

		Die();
	}

	PlayHitSound(ImpactPoint);
	SpawnHitParticles(ImpactPoint);
}

void ABaseCharacter::Attack()
{
	if (CombatTarget && CombatTarget->ActorHasTag(FName("Dead")))
	{
		CombatTarget = nullptr;
	}
}

void ABaseCharacter::Die()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision); //����֮�� �رս�������ײ

	Tags.Add(FName("Dead"));

	//������������
	PlayDeathMontage();

}

//void ABaseCharacter::PlayHitReactMontage(const FName& StatName)
//{
//	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();//��������ʾ��
//	if (AnimInstance && HitReactMontage)
//	{
//		AnimInstance->Montage_Play(HitReactMontage);
//		AnimInstance->Montage_JumpToSection(StatName, HitReactMontage);
//	}
//}

//��������ļ��е㣬���жϲ����ĸ�������ܻ�����
void ABaseCharacter::DirectionalHitReact(const FVector& ImpactPoint)
{
	//��ȡĿ�걻���еĵط���Ŀ����ǰ�����ļнǣ������ж�Ŀ�걻���к�Ӧ�ó�ʲô�ط�����
	const FVector Forward = GetActorForwardVector();
	//��Ϊ���еĵ���ܻ�� ���ܻ�ͣ��������ǲ���Ҫ����ImpactPoint��z�����ݣ����ǽ����滻Ϊ�����еĵ��˵�Z������
	const FVector ImpactPointLower = FVector(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);

	//������GetSafeNormal()�����ý����׼������������������Ϊ1��������������Ϊ0�����
	const FVector ToHit = (ImpactPointLower - GetActorLocation()).GetSafeNormal();



	//���DotProduct��ʹ�� ������Ϊ�����Forward��ToHit֮��ļн�
	//Forward * ToHit = |Forward||ToHit| * cos(theta)
	//|Forward|=1 |ToHit|=1, ����Forward * ToHit =cos(theta)
	const double CosTheta = FVector::DotProduct(Forward, ToHit);

	//Ȼ��ʹ��Acos��ýǶ�Theta
	double Theta = FMath::Acos(CosTheta);//�����theta���ǻ��ȣ���Ҫת�ɽǶ�
	Theta = FMath::RadiansToDegrees(Theta);

	//���CrossProduct��ʹ�ã�������Ϊ����û��е�����Ŀ�����໹���Ҳ�
	//UEʹ�����ַ���ʳָָ��Ŀ����ǰ(Forward)����ָָ����е��λ��(ToHit)��
	//���CrossProduct = Forward * ToHit
	//�����Ĵָ(CrossProduct)���ϣ�Ҳ����CrossProduct��z��ֵ����0����˵�����е����Ҳ࣬���������
	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);
	if (CrossProduct.Z < 0)
	{
		Theta *= -1.f;//����-1����Theta��ɸ��������Ǹ��ĽǶ�
	}

	/*
	//����һ��Z��ķ���
	UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + CrossProduct * 100.f, 20.f, FColor::Blue, 5.f);


	//ʹ��UKismetSystemLibrary����һ��debug��ͷ
	//������this����ʼλ�ã��յ�λ�ã���ͷֱ������ͷ��ɫ����ͷ����ʱ��
	//���Ȼ���һ��Ŀ��ķ���
	UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + Forward * 60.f, 20.f, FColor::Red, 5.f);
	//Ȼ�����һ�»��еķ���
	UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + ToHit * 60.f, 20.f, FColor::Green, 5.f);
	*/


	/*-45  45
		 \/
		 /\
	  -135 135*/

	//���ﰴ��ǰ�����ҵ�˳�򵹣�UE��Ҳ��Ҫ����ǰ�����ҵ�˳����д
	if (HitReactionMontageName.Num() > 0)
	{
		if (Theta <45 && Theta >-45)
		{
			PlayMontageSection(HitReactMontage, HitReactionMontageName[1]);//���
		}
		else if (Theta > -135 && Theta < -45)
		{
			PlayMontageSection(HitReactMontage, HitReactionMontageName[3]);//���ҵ�
		}
		else if (Theta > 45 && Theta < 135)
		{
			PlayMontageSection(HitReactMontage, HitReactionMontageName[2]);//����
		}
		else
		{
			PlayMontageSection(HitReactMontage, HitReactionMontageName[0]);//��ǰ��
		}
	}
	else {
		return;
	}
}

void ABaseCharacter::PlayHitSound(const FVector& ImpactPoint)
{
	if (HitSound)//�����ܻ���Ч
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			HitSound,
			ImpactPoint
		);
	}
}

void ABaseCharacter::SpawnHitParticles(const FVector& ImpactPoint)
{
	if (HitParticle)//�����ܻ���Ч
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			HitParticle,
			ImpactPoint
		);
	}
}

void ABaseCharacter::HandleDamage(float DamageAmount)
{
	if (Attributes)
	{
		Attributes->ReceiveDamage(DamageAmount);
	}
}

void ABaseCharacter::PlayMontageSection(UAnimMontage* Montage, const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();//��������ʾ��
	if (AnimInstance && Montage)
	{
		AnimInstance->Montage_Play(Montage);
		AnimInstance->Montage_JumpToSection(SectionName, Montage);
		UE_LOG(LogTemp, Warning, TEXT("%s"), *SectionName.ToString());
	}
}

int32 ABaseCharacter::PlayRandomMontage(UAnimMontage* Montage, const TArray<FName>& SectionName)
{
	if (SectionName.Num() <= 0)
	{
		GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Red, FString::Printf(TEXT("SectionName is NULL")));//��������ñ�
		return 0;
	}
	const int32 MaxSectionIndex = SectionName.Num() - 1;
	const int32 Selection = FMath::RandRange(0, MaxSectionIndex);
	PlayMontageSection(Montage, SectionName[Selection]);
	return Selection;
}

int32 ABaseCharacter::PlayAttackMontage()
{
	return PlayRandomMontage(AttackMontage, AttackMontageName);
}

int32 ABaseCharacter::PlayDeathMontage()
{

	const int32 Selection = PlayRandomMontage(DeathMontage, DeathMontageName);

	TEnumAsByte<EDeathPose> Pose(Selection);
	if (Pose < EDeathPose::EDP_MAX)
	{
		DeathPose = Pose;
	}

	return Selection;
}

void ABaseCharacter::PlayDodgeMontage()
{
	PlayMontageSection(DodgeMontage, FName("Default"));
}

void ABaseCharacter::StopAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Stop(0.25f, AttackMontage);
	}
}

FVector ABaseCharacter::GetTranslationWarpTarget()
{
	if (CombatTarget)
	{
		FVector CombatTargetLocation = CombatTarget->GetActorLocation();
		FVector Location = GetActorLocation();

		FVector TargettoMe = (Location - CombatTargetLocation).GetSafeNormal();

		TargettoMe *= WarpTargetDistance;

		return CombatTargetLocation + TargettoMe;

	}
	else return FVector();
}

FVector ABaseCharacter::GetRotationWarpTarget()
{
	if (CombatTarget)
	{
		return CombatTarget->GetActorLocation();
	}
	return FVector();
}

bool ABaseCharacter::CanAttack()
{
	return false;
}

void ABaseCharacter::AttackEnd()
{

}

void ABaseCharacter::DodgeEnd()
{
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseCharacter::SetWeaponCollision(ECollisionEnabled::Type CollisionType)
{
	if (EquippedWeapon && EquippedWeapon->GetWeaponBox())
	{
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionType);
		EquippedWeapon->IgnoreActors.Empty();
	}
}