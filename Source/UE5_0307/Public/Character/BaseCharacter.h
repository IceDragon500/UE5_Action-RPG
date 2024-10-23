// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/HitInterface.h"
#include "Character/CharacterType.h"
#include "BaseCharacter.generated.h"

class UAnimMontage;
class AWeapons;
class UAttributeComponent;

UCLASS()
class UE5_0307_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:

	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;
	//基础角色类不会具体实现按键输入绑定，所以这里不需要
	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//设置武器碰撞生效和关闭
	UFUNCTION(BlueprintCallable)
	void SetWeaponCollision(ECollisionEnabled::Type CollisionType);

	FORCEINLINE EDeathPose GetDeathPose() const { return DeathPose;  }

protected:

	virtual void BeginPlay() override;

	//碰撞
	//已经装备好的武器
	UPROPERTY(VisibleAnywhere, Category = "Combat")
	AWeapons* EquippedWeapon;

	//攻击
	virtual void Attack();
	virtual bool CanAttack();
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual void HandleDamage(float DamageAmount);
	virtual void PlayDodgeMontage();
	void StopAttackMontage();

	UFUNCTION(BlueprintCallable)
	FVector GetTranslationWarpTarget();
	UFUNCTION(BlueprintCallable)
	FVector GetRotationWarpTarget();

	//播放动画
	virtual int32 PlayAttackMontage();
	virtual int32 PlayDeathMontage();
	void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName);
	int32 PlayRandomMontage(UAnimMontage* Montage, const TArray<FName>& SectionName);

	//控制攻击时不会播放其他的动作，比如跑步
	UFUNCTION(BlueprintCallable)
	virtual	void AttackEnd();
	//控制翻滚后切换状态
	UFUNCTION(BlueprintCallable)
	virtual void DodgeEnd();

	//处理死亡相关内容
	virtual void Die();

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EDeathPose> DeathPose;

	//播放受击动画
	//void PlayHitReactMontage(const FName& StatName);

	//根据输入的集中点，来判断播放哪个方向的受击动画
	void DirectionalHitReact(const FVector& ImpactPoint);

	//播放击中音效
	void PlayHitSound(const FVector& ImpactPoint);
	//播放击中特效
	void SpawnHitParticles(const FVector& ImpactPoint);

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	AActor* CombatTarget; //战斗目标

	//游戏属性相关
	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes; //属性

private:

//动画蒙太奇
	//攻击动画切换
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* HitReactMontage;

	//攻击动画的序列名称
	UPROPERTY(EditAnywhere, Category = "Montages")
	TArray<FName> AttackMontageName;

	//受击动画的序列名称
	UPROPERTY(EditAnywhere, Category = "Montages")
	TArray<FName> HitReactionMontageName;

	//死亡动画的序列名称
	UPROPERTY(EditAnywhere, Category = "Montages")
	TArray<FName> DeathMontageName;
	
	//攻击动画切换
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* AttackMontage;

	//死亡动画切换
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* DeathMontage;

	//闪避动画
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* DodgeMontage;
	 
//战斗相关
	//距离目标距离
	UPROPERTY(EditAnywhere, Category = "Combat")
	double WarpTargetDistance = 75.f;

	//受击音效
	UPROPERTY(EditAnywhere, Category = "Combat")
	USoundBase* HitSound;

	//受击特效
	UPROPERTY(EditAnywhere, Category = "Combat")
	UParticleSystem* HitParticle;





};
