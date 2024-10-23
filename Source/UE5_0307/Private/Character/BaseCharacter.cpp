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

	//设置自定义的属性组件
	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);//设置对胶囊体不碰撞
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

}

void ABaseCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	if (Attributes && Attributes->IsAlive() && Hitter)//这里判断是否存活，是则播放受击动画，否则播放死亡动画
	{
		//将Hitter的坐标传入，播放受击动画
		DirectionalHitReact(Hitter->GetActorLocation());
	}
	else
	{
		//死亡关闭血条
		//HealthBarWidget->SetVisibility(false);
		//关闭SkeletaMesh的碰撞，避免死亡后尸体还是会被武器打中，继续播放死亡动画
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
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision); //死亡之后 关闭胶囊体碰撞

	Tags.Add(FName("Dead"));

	//播放死亡动画
	PlayDeathMontage();

}

//void ABaseCharacter::PlayHitReactMontage(const FName& StatName)
//{
//	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();//创建动画示例
//	if (AnimInstance && HitReactMontage)
//	{
//		AnimInstance->Montage_Play(HitReactMontage);
//		AnimInstance->Montage_JumpToSection(StatName, HitReactMontage);
//	}
//}

//根据输入的集中点，来判断播放哪个方向的受击动画
void ABaseCharacter::DirectionalHitReact(const FVector& ImpactPoint)
{
	//求取目标被击中的地方与目标向前向量的夹角，用来判断目标被击中后，应该朝什么地方倒下
	const FVector Forward = GetActorForwardVector();
	//因为击中的点可能会高 可能会低，所以我们不需要传入ImpactPoint的z轴数据，我们将其替换为被命中的敌人的Z轴数据
	const FVector ImpactPointLower = FVector(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);

	//这里用GetSafeNormal()，是让结果标准化，就是想向量长度为1，避免向量长度为0的情况
	const FVector ToHit = (ImpactPointLower - GetActorLocation()).GetSafeNormal();



	//点积DotProduct的使用 这里是为了求得Forward和ToHit之间的夹角
	//Forward * ToHit = |Forward||ToHit| * cos(theta)
	//|Forward|=1 |ToHit|=1, 所以Forward * ToHit =cos(theta)
	const double CosTheta = FVector::DotProduct(Forward, ToHit);

	//然后使用Acos求得角度Theta
	double Theta = FMath::Acos(CosTheta);//这里的theta还是弧度，需要转成角度
	Theta = FMath::RadiansToDegrees(Theta);

	//叉积CrossProduct的使用，这里是为了求得击中点是在目标的左侧还是右侧
	//UE使用左手法则，食指指向目标向前(Forward)，中指指向击中点的位置(ToHit)，
	//叉积CrossProduct = Forward * ToHit
	//如果大拇指(CrossProduct)向上，也就是CrossProduct的z轴值大于0，则说明击中点在右侧，否则在左侧
	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);
	if (CrossProduct.Z < 0)
	{
		Theta *= -1.f;//乘以-1，将Theta变成负数，就是负的角度
	}

	/*
	//绘制一下Z轴的方向
	UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + CrossProduct * 100.f, 20.f, FColor::Blue, 5.f);


	//使用UKismetSystemLibrary绘制一个debug箭头
	//参数：this，起始位置，终点位置，箭头直径，箭头颜色，箭头持续时长
	//首先绘制一下目标的方向
	UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + Forward * 60.f, 20.f, FColor::Red, 5.f);
	//然后绘制一下击中的方向
	UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + ToHit * 60.f, 20.f, FColor::Green, 5.f);
	*/


	/*-45  45
		 \/
		 /\
	  -135 135*/

	//这里按照前后左右的顺序倒，UE上也需要按照前后左右的顺序填写
	if (HitReactionMontageName.Num() > 0)
	{
		if (Theta <45 && Theta >-45)
		{
			PlayMontageSection(HitReactMontage, HitReactionMontageName[1]);//向后倒
		}
		else if (Theta > -135 && Theta < -45)
		{
			PlayMontageSection(HitReactMontage, HitReactionMontageName[3]);//向右倒
		}
		else if (Theta > 45 && Theta < 135)
		{
			PlayMontageSection(HitReactMontage, HitReactionMontageName[2]);//向左倒
		}
		else
		{
			PlayMontageSection(HitReactMontage, HitReactionMontageName[0]);//向前倒
		}
	}
	else {
		return;
	}
}

void ABaseCharacter::PlayHitSound(const FVector& ImpactPoint)
{
	if (HitSound)//播放受击音效
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
	if (HitParticle)//播放受击特效
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
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();//创建动画示例
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
		GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Red, FString::Printf(TEXT("SectionName is NULL")));//报个错，免得崩
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