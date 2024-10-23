// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "CharacterType.h"
#include "BaseCharacter.h"
#include "Interface/PickupInterface.h"
#include "EchoCharacter.generated.h"

class UCharacterMovementComponent;
class UGroomComponent;
class USpringArmComponent;
class UCameraComponent;
class AItem;
class ASoul;
class UAnimMontage;
class UMainInterface;

#define CANDISARM AttackState == EAttackState::EAS_Stay && CharState != ECharacterState::ECS_UnEquipped
#define CANARM AttackState == EAttackState::EAS_Stay && CharState == ECharacterState::ECS_UnEquipped && EquippedWeapon

UCLASS()
class UE5_0307_API AEchoCharacter : public ABaseCharacter , public IPickupInterface
{
	GENERATED_BODY()

public:

	AEchoCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//拾取
	virtual void SetOverlappingItem(AItem* item) override;
	virtual void AddSouls(ASoul* soul) override;
	virtual void AddGolds(ATreasure* Treasure) override;

	FORCEINLINE ECharacterState GetCharacterState() const { return CharState; }

	void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter);
	//受到伤害
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Jump() override;
	virtual void Die() override;

protected:

	virtual void BeginPlay() override;

	

	//按键功能
	void MoveForward(float value);
	void MoveLeftRight(float value);
	void TurnLeftRight(float value);
	void LookUpDown(float value);
	void EKeyPress();
	void Dodge();
	virtual void Attack() override;

	//播放动画 playAttackMontage
	//virtual void PlayAttackPlay() override; 迁移到父类了

	virtual void AttackEnd() override;//控制攻击时不会播放其他的动作，比如跑步
	virtual void DodgeEnd() override;

	UFUNCTION(BlueprintCallable)
	void HitReactEnd();//受到攻击时不会播放其他的动作

	UFUNCTION(BlueprintCallable)
	void EquipEnd();//控制在换装备时，不会播放其他的动作，比如攻击、跑步

	void PlayEquipMontage(FName SectionName);//将播放动画的功能独立出来

	//用于蓝图中触发上下装备的动画
	UFUNCTION(BlueprintCallable)
	void AttachWeaponToBack();

	//用于蓝图中触发上下装备的动画
	UFUNCTION(BlueprintCallable)
	void AttachWeaponToHand();



private:
	void InitializeMainInterface();//初始化界面上的数值
	//void SetHUDHealth();
	//void IsUnoccupied();
	// 
	//设置攻击状态
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EAttackState AttackState = EAttackState::EAS_Stay;

	//设定角色初始状态
	ECharacterState CharState = ECharacterState::ECS_UnEquipped;

	//检查当前身上是否有装备
	bool bIsEquipped = false;


//各种组件
	UPROPERTY(VisibleAnywhere, Category = Hair)
	UGroomComponent* GroomHair;

	UPROPERTY(VisibleAnywhere, Category = Hair)
	UGroomComponent* GroomEyebrows;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;

//碰撞
	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

//动画蒙太奇
	//上下武器动画切换
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* EquipMontage; 

	//视频里面的SlashOverlay
	UPROPERTY()
	UMainInterface* MainInterface;
};
