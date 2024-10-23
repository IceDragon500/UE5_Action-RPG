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

	//����ʹ��C++�ķ�ʽ�����˱༭���У�ϸ��->��ײ����Ĳ���
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);//������ײ�Ķ�������
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);//���öԿɼ��Ĳ��ֽ�����ײ
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);//���ò��Խ�ɫ���������ײ
	GetMesh()->SetGenerateOverlapEvents(true);//���������ص��¼�

	//����Ѫ����� UWG
	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	//����Enemy�ƶ�ʱ �����ƶ�����
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	//����PawnSensing���
	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	PawnSensing->SightRadius = 2000;//���ù۲�뾶
	PawnSensing->SetPeripheralVisionAngle(45.f);//������Ұ�Ƕ�
}


void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	//��PawnSeen���������ڴ����۲쵽֮����¼�
	if (PawnSensing) PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);

	//��ʼ������
	InitializeEnemy();

	Tags.Add(FName("Enemy"));
}

void AEnemy::InitializeEnemy()
{
	SetHealthBarHUD(false);//����һ��ʼ����ʾѪ��

	EnemyController = Cast<AAIController>(GetController());

	//���ö�ʱ�� ��5��֮�󣬴���&AEnemy::PatrolTimerFinished����
	const float WaitTime = FMath::RandRange(WaitMin, WaitMax);
	GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, WaitTime);

	//װ������
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
			GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Red, FString::Printf(TEXT("WeaponSocket_Right is NULL")));//��������ñ�
			return;
		}

		if (Weapon_LeftHand)
		{
			Weapon_RightHand->Equip(GetMesh(), FName("WeaponSocket_Left"), this, this);
		}
		else {
			GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Red, FString::Printf(TEXT("WeaponSocket_Left is NULL")));//��������ñ�
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
		CheckPartolTarget();//���Ѳ��Ŀ��
		break;
	case EEnemyState::EES_Attack:
	case EEnemyState::EES_chasing:
	case EEnemyState::EES_Battle:
		CheckCombatTarget();//���ս��Ŀ��
		break;
	default:
		break;
	}
}

//���Ѳ��Ŀ��
void AEnemy::CheckPartolTarget()
{
	if (InTargetRange(PatrolTarget, PatrolRadius))//�����Ŀ��ľ�����PatrolRadius��Χ�ڣ�������һ��Ŀ���ƶ�
	{
		if (PatrolArry.Num() > 0)
		{
			int32 num = FMath::RandRange(0, PatrolArry.Num() - 1);
			PatrolTarget = PatrolArry[num];
			//MoveToTarget(PatrolTarget); ����滻����timer������movetotarget����
			GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, 5.f);
		}
	}
}

//���ս��Ŀ��
void AEnemy::CheckCombatTarget()
{
	//�ж��Լ��빥���ߵľ��룬���������CombatRadius���򽫹���Ŀ��CombatTarget��գ�Ȼ�������Ѳ�ߵ��ƶ�
	if (!InTargetRange(CombatTarget, CombatRadius))
	{
		GetWorldTimerManager().ClearTimer(AttackTimer);

		CombatTarget = nullptr;
		SetHealthBarHUD(false);
		if(EnemyState != EEnemyState::EES_Battle) GotoTarget(EEnemyState::EES_Patrolling, PatrolTarget, 140.f);
	}
	//�����ǰ���ڹ����뾶����׷���뾶�����Ҳ���׷��״̬�����л���׷��״̬���л�Ŀ��λս��Ŀ��
	else if (!InTargetRange(CombatTarget, AttackRadius) && !isChasing())
	{ 
		//��׷���뾶�ڣ����ǲ��ڹ����뾶�ڣ�
		GetWorldTimerManager().ClearTimer(AttackTimer);
		if (EnemyState != EEnemyState::EES_Battle) GotoTarget(EEnemyState::EES_chasing, CombatTarget, 400.f);
	}
	//�����ǰ�ڹ����뾶���Ҳ��ڹ���״̬�����л�������״̬�����빥������
	else if(CanAttack())
	{
		StartAttackTimer();//���빥���߼�
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

// ������ѯ��Ŀ��Target�ľ���
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

//Enemy���ƶ�����
void AEnemy::MoveToTarget(AActor* Target)
{
	if (EnemyController == nullptr || Target == nullptr) return;

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);//���ñ���Ŀ��ΪPatrolTraget
	MoveRequest.SetAcceptanceRadius(AcceptanceRadius);//���ð뾶 AcceptanceRadius���������˵����������ͻ�ͣ����
	EnemyController->MoveTo(MoveRequest);//�ڶ��������Դ�Ĭ��ֵ������Ҫ��д FNavPathSharedPtr NavPath;
}

void AEnemy::GotoTarget(EEnemyState State, AActor* Target, float Speed)
{
	EnemyState = State;
	MoveToTarget(Target);
	GetCharacterMovement()->MaxWalkSpeed = Speed;
}

//�����˷���
void AEnemy::PawnSeen(APawn* SeenPawn)
{
	if (EnemyState == EEnemyState::EES_chasing || SeenPawn->ActorHasTag(FName("Dead")))  return;
	if (SeenPawn->ActorHasTag(FName("Echo")))
	{
		GetWorldTimerManager().ClearTimer(PatrolTimer);//ֹͣ��ʱ��
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


//��ʱ��������ķ���
void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);
}

//�̳���HitInterface�ӿڵ�GetHit�¼�
void AEnemy::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);

	ClearPatrolTimer();//�ر�Ѳ�߼�ʱ��
	ClearAttackTimer();//�رչ�����ʱ��

	SetWeaponCollision(ECollisionEnabled::NoCollision);//��������ڹ���ʱ�����У���Ҫ�жϵ��˵��������Ч��

	StopAttackMontage();//��������ڹ���ʱ�����У���Ҫ�жϵ��˵���ζ�������

	//��Ѫ����ʾ����
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


//�ܵ��˺�
float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);

	CombatTarget = EventInstigator->GetPawn();//������Ŀ�긳ֵ����Enemy֪����˭(Actor)����Ĺ���

	if (IsInsideAttackRadius())
	{
		EnemyState = EEnemyState::EES_Battle;
	}
	else if (IsOutsideAttackRadius())
	{
		GotoTarget(EEnemyState::EES_chasing, CombatTarget, 400.f);
	}
	
	return DamageAmount;//������Ƿ����˺�ֵ
}

//������������¼�
void AEnemy::Die()
{
	Super::Die();
	SetHealthBarHUD(true);


	bGenerateOverlapEventsDuringLevelStreaming = false;
	//GetSkeletalMeshRefTangentBasis//����֮�󣬹ر��ص��¼�
	EnemyState = EEnemyState::EES_Dead; //����״̬Ϊ����

	GetWorldTimerManager().ClearTimer(PatrolTimer);//ֹͣ��ʱ��
	GetWorldTimerManager().ClearTimer(AttackTimer);
	PatrolTarget = nullptr;//������Ŀ��ֵΪ��
	CombatTarget = nullptr;
	PawnSensing->SetSensingUpdatesEnabled(false);//�ر�Pawnseen�ĸ���
	//SetLifeSpan(DeathLifeSpan);//ɾ��Actor

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