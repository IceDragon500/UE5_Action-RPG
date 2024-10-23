// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "AIController.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/AttributeComponent.h"
#include "HUD/HealthBarComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Items/Weapons.h"
#include "Items/Soul.h"

//#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	//这里使用C++的方式设置了编辑器中，细节->碰撞里面的参数
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);//设置碰撞的对象类型
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);//设置对可见的部分进行碰撞
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);//设置不对角色的摄像机碰撞
	GetMesh()->SetGenerateOverlapEvents(true);//设置生成重叠事件

	//设置血条组件 UWG
	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	//设置Enemy移动时 朝向移动方向
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	//创建PawnSensing组件
	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	PawnSensing->SightRadius = 2000;//设置观察半径
	PawnSensing->SetPeripheralVisionAngle(45.f);//设置视野角度
}


void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	//绑定PawnSeen方法，用于触发观察到之后的事件
	if (PawnSensing) PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);

	//初始化参数
	InitializeEnemy();

	Tags.Add(FName("Enemy"));
}

void AEnemy::InitializeEnemy()
{
	SetHealthBarHUD(false);//设置一开始不显示血条

	EnemyController = Cast<AAIController>(GetController());

	//设置定时器 在5秒之后，触发&AEnemy::PatrolTimerFinished方法
	const float WaitTime = FMath::RandRange(WaitMin, WaitMax);
	GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, WaitTime);

	//装备武器
	UWorld* World = GetWorld();
	if (World)
	{
		AWeapons* Weapon_RightHand = World->SpawnActor<AWeapons>(WeaponClass_RightHand);
		AWeapons* Weapon_LeftHand = World->SpawnActor<AWeapons>(WeaponClass_LeftHand);
		if (Weapon_RightHand)
		{
			Weapon_RightHand->Equip(GetMesh(), FName("WeaponSocket_Right"), this, this);
			EquippedWeapon = Weapon_RightHand;
		}
		else {
			GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Red, FString::Printf(TEXT("WeaponSocket_Right is NULL")));//报个错，免得崩
			return;
		}

		if (Weapon_LeftHand)
		{
			Weapon_RightHand->Equip(GetMesh(), FName("WeaponSocket_Left"), this, this);
		}
		else {
			GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Red, FString::Printf(TEXT("WeaponSocket_Left is NULL")));//报个错，免得崩
			return;
		}
		
	}
}


void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (EnemyState)
	{
	case EEnemyState::EES_Dead:
		return;
		break;
	case EEnemyState::EES_Patrolling:
		CheckPartolTarget();//检查巡逻目标
		break;
	case EEnemyState::EES_Attack:
	case EEnemyState::EES_chasing:
	case EEnemyState::EES_Battle:
		CheckCombatTarget();//检查战斗目标
		break;
	default:
		break;
	}
}

//检查巡逻目标
void AEnemy::CheckPartolTarget()
{
	if (InTargetRange(PatrolTarget, PatrolRadius))//如果与目标的距离在PatrolRadius范围内，则向下一个目标移动
	{
		if (PatrolArry.Num() > 0)
		{
			int32 num = FMath::RandRange(0, PatrolArry.Num() - 1);
			PatrolTarget = PatrolArry[num];
			//MoveToTarget(PatrolTarget); 这个替换成由timer来触发movetotarget方法
			GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, 5.f);
		}
	}
}

//检查战斗目标
void AEnemy::CheckCombatTarget()
{
	//判断自己与攻击者的距离，如果超过了CombatRadius，则将攻击目标CombatTarget清空，然后继续向巡逻点移动
	if (!InTargetRange(CombatTarget, CombatRadius))
	{
		GetWorldTimerManager().ClearTimer(AttackTimer);

		CombatTarget = nullptr;
		SetHealthBarHUD(false);
		if(EnemyState != EEnemyState::EES_Battle) GotoTarget(EEnemyState::EES_Patrolling, PatrolTarget, 140.f);
	}
	//如果当前不在攻击半径（在追击半径），且不在追击状态，则切换到追击状态，切换目标位战斗目标
	else if (!InTargetRange(CombatTarget, AttackRadius) && !isChasing())
	{ 
		//在追击半径内，但是不在攻击半径内，
		GetWorldTimerManager().ClearTimer(AttackTimer);
		if (EnemyState != EEnemyState::EES_Battle) GotoTarget(EEnemyState::EES_chasing, CombatTarget, 400.f);
	}
	//如果当前在攻击半径，且不在攻击状态，则切换到攻击状态，进入攻击方法
	else if(CanAttack())
	{
		StartAttackTimer();//进入攻击逻辑
	}
}

bool AEnemy::CanAttack()
{
	bool bCanAttack =
		InTargetRange(CombatTarget, AttackRadius) &&
		EnemyState != EEnemyState::EES_Attack &&
		EnemyState != EEnemyState::EES_Battle &&
		EnemyState != EEnemyState::EES_Dead;

	return bCanAttack;
}

bool AEnemy::isChasing()
{
	return EnemyState == EEnemyState::EES_chasing;
}

void AEnemy::Attack()
{
	Super::Attack();

	if (CombatTarget == nullptr) return;
	EnemyState = EEnemyState::EES_Battle;
	PlayAttackMontage();
}

void AEnemy::AttackEnd()
{
	EnemyState = EEnemyState::EES_NoState;
	CheckCombatTarget();
}

void AEnemy::HandleDamage(float DamageAmount)
{
	Super::HandleDamage(DamageAmount);

	if (Attributes && HealthBarWidget)
	{
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}
}

void AEnemy::StartAttackTimer()
{
	EnemyState = EEnemyState::EES_Attack;
	const float AttackTime = FMath::RandRange(AttackMin, AttackMax);
	GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
}

// 用来查询与目标Target的距离
bool AEnemy::InTargetRange(AActor* Target,  double Radius)
{
	if (Target == nullptr)  return false;
	double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
	DrawDebugLine(GetWorld(), GetActorLocation(), Target->GetActorLocation(), FColor::Blue, false, -1.f, 0, 1.f);
	return DistanceToTarget <= Radius;
}

bool AEnemy::IsInsideAttackRadius()
{
	return InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemy::IsOutsideAttackRadius()
{
	return !InTargetRange(CombatTarget, AttackRadius);
}

//Enemy的移动方法
void AEnemy::MoveToTarget(AActor* Target)
{
	if (EnemyController == nullptr || Target == nullptr) return;

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);//设置本地目标为PatrolTraget
	MoveRequest.SetAcceptanceRadius(AcceptanceRadius);//设置半径 AcceptanceRadius，这样敌人到了这个距离就会停下来
	EnemyController->MoveTo(MoveRequest);//第二个参数自带默认值，不需要填写 FNavPathSharedPtr NavPath;
}

void AEnemy::GotoTarget(EEnemyState State, AActor* Target, float Speed)
{
	EnemyState = State;
	MoveToTarget(Target);
	GetCharacterMovement()->MaxWalkSpeed = Speed;
}

//被敌人发现
void AEnemy::PawnSeen(APawn* SeenPawn)
{
	if (EnemyState == EEnemyState::EES_chasing || SeenPawn->ActorHasTag(FName("Dead")))  return;
	if (SeenPawn->ActorHasTag(FName("Echo")))
	{
		GetWorldTimerManager().ClearTimer(PatrolTimer);//停止计时器
		CombatTarget = SeenPawn;
		
		if (EnemyState != EEnemyState::EES_Attack)
		{
			GotoTarget(EEnemyState::EES_Attack, CombatTarget, 400.f);
		}		
	}
}

void AEnemy::SetHealthBarHUD(bool Open)
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(Open);
	}
}


//定时器结束后的方法
void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);
}

//继承自HitInterface接口的GetHit事件
void AEnemy::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);

	ClearPatrolTimer();//关闭巡逻计时器
	ClearAttackTimer();//关闭攻击计时器

	SetWeaponCollision(ECollisionEnabled::NoCollision);//如果敌人在攻击时被击中，需要中断敌人的这次命中效果

	StopAttackMontage();//如果敌人在攻击时被击中，需要中断敌人的这次动画播放

	//吧血条显示出来
	SetHealthBarHUD(true);
	
	if (InTargetRange(CombatTarget, AttackRadius))
	{
		if (!(EnemyState == EEnemyState::EES_Dead))  StartAttackTimer();
	}
}

void AEnemy::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(AttackTimer);
}

void AEnemy::ClearPatrolTimer()
{
	GetWorldTimerManager().ClearTimer(PatrolTimer);
}


//受到伤害
float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);

	CombatTarget = EventInstigator->GetPawn();//给攻击目标赋值，让Enemy知道是谁(Actor)发起的攻击

	if (IsInsideAttackRadius())
	{
		EnemyState = EEnemyState::EES_Battle;
	}
	else if (IsOutsideAttackRadius())
	{
		GotoTarget(EEnemyState::EES_chasing, CombatTarget, 400.f);
	}
	
	return DamageAmount;//最后我们返回伤害值
}

//处理死亡相关事件
void AEnemy::Die()
{
	Super::Die();
	SetHealthBarHUD(true);


	bGenerateOverlapEventsDuringLevelStreaming = false;
	//GetSkeletalMeshRefTangentBasis//死亡之后，关闭重叠事件
	EnemyState = EEnemyState::EES_Dead; //设置状态为死亡

	GetWorldTimerManager().ClearTimer(PatrolTimer);//停止计时器
	GetWorldTimerManager().ClearTimer(AttackTimer);
	PatrolTarget = nullptr;//将两个目标值为空
	CombatTarget = nullptr;
	PawnSensing->SetSensingUpdatesEnabled(false);//关闭Pawnseen的更新
	//SetLifeSpan(DeathLifeSpan);//删除Actor

	SpawnSoul();

}

void AEnemy::SpawnSoul()
{
	UWorld* world = GetWorld();
	if (world && dropSoul)
	{
		FVector Location = GetActorLocation();
		Location.Z += LoopHight;
		ASoul* Soul = world->SpawnActor<ASoul>(dropSoul, Location, GetActorRotation());
		if (Soul)
		{
			Soul->SetSouls(Attributes->GetSouls() );
			Soul->SetOwner(this);
		}
		
	}
}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}