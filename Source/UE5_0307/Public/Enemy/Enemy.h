// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CharacterType.h"
#include "Character/BaseCharacter.h"
#include "Enemy.generated.h"

class UHealthBarComponent;
class ASoul;

UCLASS()
class UE5_0307_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:

	AEnemy();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	//检查巡逻目标
	void CheckPartolTarget();
	//检查战斗目标
	void CheckCombatTarget();
	bool isChasing();
	// 绑定输入
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter);

	//受到伤害
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override; 

protected:
	// 在游戏开始时调用
	virtual void BeginPlay() override;

	void InitializeEnemy();

	//处理死亡相关内容
	virtual void Die() override;

	void SpawnSoul();

	//攻击
	virtual void Attack() override;
	virtual bool CanAttack() override;
	virtual	void AttackEnd() override;
	virtual void HandleDamage(float DamageAmount) override;

//敌人当前状态
//Enemy状态控制
	UPROPERTY(BlueprintReadOnly)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

	//UPROPERTY(BlueprintReadOnly)
	//EDeathPose DeathPose;


//用来查询与目标Target的距离，是否在预设的Radius距离内
	bool InTargetRange(AActor* Target, double Radius);
	bool IsInsideAttackRadius();
	bool IsOutsideAttackRadius();


//Enemy的移动方法
	void MoveToTarget(AActor* Target);

//被敌人发现
	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);//用于调用see之后的事件

private:

//游戏属性相关
	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget; //HUB 血条

	void SetHealthBarHUD(bool Open);



//navigation 导航
	void GotoTarget(EEnemyState State, AActor* Target, float  Speed);//传入目标，需要更新的状态，速度

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AcceptanceRadius = 80.f;

	UPROPERTY()
	class AAIController* EnemyController;

	//当前巡逻目标
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* PatrolTarget; 

	//到达巡逻点这个距离后就停止移动
	UPROPERTY(EditAnywhere, Category = "Combat")
	double PatrolRadius = 200.f; 

	// 巡逻目标群组
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolArry; 

//巡逻待机
	//巡逻计时器
	FTimerHandle PatrolTimer; 
	//计时器调用的方法
	void PatrolTimerFinished();
	//巡逻停滞最小时间
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float WaitMin = 5.f;
	//巡逻停滞最大时间
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float WaitMax = 10.f;

//Enemy观察 pawnsensing组件
	UPROPERTY(VisibleAnywhere)
	class UPawnSensingComponent* PawnSensing;

//攻击目标相关
	//追踪半径
	UPROPERTY(EditAnywhere, Category = "Combat")
	double CombatRadius = 500; 


//Enemy攻击
	UPROPERTY(EditAnywhere, Category = "Combat")
	double AttackRadius = 150.f;
	//攻击间隔计时器
	FTimerHandle AttackTimer; 

	void StartAttackTimer();

	UPROPERTY(EditAnywhere , Category = "Combat")
	float AttackMin = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackMax = 1.f;

//Enemy武器
	//声明一个武器类，告知编辑器只筛选出AWeapons类
	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<class AWeapons> WeaponClass_RightHand;

	//声明一个武器类，告知编辑器只筛选出AWeapons类
	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<class AWeapons> WeaponClass_LeftHand;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float DeathLifeSpan = 8.f;

	void ClearAttackTimer();
	void ClearPatrolTimer();
//掉落
	//掉落之后的高度
	UPROPERTY(EditAnywhere, Category = "Combat")
	double LoopHight = 20;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<ASoul> dropSoul;

	//UPROPERTY(EditAnywhere, Category = "Drop")
	//int32 SoulNum;
};
