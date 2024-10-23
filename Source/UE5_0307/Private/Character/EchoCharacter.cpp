// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/EchoCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GroomComponent.h"
#include "Items/Item.h"
#include "Items/Weapons.h"
#include "Items/Soul.h"
#include "Items/Treasure.h"
#include "Animation/AnimMontage.h"
#include "Components/BoxComponent.h"
#include "HUD/MainHUD.h"
#include "HUD/MainInterface.h"
#include "Components/AttributeComponent.h"

// Sets default values
AEchoCharacter::AEchoCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;


	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);


	//��ӵ��ɱۺ�����ͷ
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 500.f;
	SpringArm->bUsePawnControlRotation = true;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(SpringArm);

	//���GroomComponent ��ͷ����üë
	GroomHair = CreateDefaultSubobject<UGroomComponent>(TEXT("GroomHair"));
	GroomHair->SetupAttachment(GetMesh());
	GroomHair->AttachmentName = FString("head");

	GroomEyebrows = CreateDefaultSubobject<UGroomComponent>(TEXT("GroomEyebrows"));
	GroomEyebrows->SetupAttachment(GetMesh());
	GroomEyebrows->AttachmentName = FString("head");

}

void AEchoCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);

	SetWeaponCollision(ECollisionEnabled::NoCollision);
	if (Attributes && Attributes->GetHealthPercent() > 0.f)
	{
		AttackState = EAttackState::EAS_HitReaction;
	}
	
}

float AEchoCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);

	//SetHUDHealth
	if (MainInterface && Attributes)
	{
		MainInterface->SetHealthProgress(Attributes->GetHealthPercent());
	}

	return DamageAmount;
}

// Called when the game starts or when spawned
void AEchoCharacter::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add(FName("Echo"));

	InitializeMainInterface();
	
}

void AEchoCharacter::MoveForward(float value)
{
	if (AttackState != EAttackState::EAS_Stay || CharState == ECharacterState::ECS_Death) return;//���������������״̬���򲻻��ƶ�
	if (Controller && value != 0.f)
	{
		//FVector Forward = GetActorForwardVector(); //��һ�ַ�����ʼ������actorǰ�ķ����ƶ�

		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		AddMovementInput(Direction, value);
	}
}

void AEchoCharacter::MoveLeftRight(float value)
{
	if (AttackState != EAttackState::EAS_Stay || CharState == ECharacterState::ECS_Death) return;
	if (Controller && value != 0.f)
	{
		//FVector Right = GetActorRightVector();

		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(Direction, value);
	}
}

void AEchoCharacter::TurnLeftRight(float value)
{
	if (Controller && value != 0.f)
	{
		AddControllerYawInput(value);
	}
}

void AEchoCharacter::LookUpDown(float value)
{
	if (Controller && value != 0.f)
	{
		AddControllerPitchInput(value);
	}
}

void AEchoCharacter::EKeyPress()
{

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();//��������ʾ��
	AWeapons* OverlappingWeapon = Cast<AWeapons>(OverlappingItem);//����ײ����ת��Ϊ������ �����м��

	//�ҵĴ���
	if (!EquippedWeapon) {
		if (OverlappingWeapon) {
			OverlappingWeapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
			CharState = ECharacterState::ECS_EquippedOneHandWeapone;
			bIsEquipped = true;
			EquippedWeapon = OverlappingWeapon;
		}
		else {
			return;
		}
	}
	else {
		if (CharState != ECharacterState::ECS_UnEquipped) {
			AnimInstance->Montage_Play(EquipMontage);
			AnimInstance->Montage_JumpToSection(FName("UnEquip"), EquipMontage);
			CharState = ECharacterState::ECS_UnEquipped;
			AttackState = EAttackState::EAS_EquippingWeapon;
		}
		else {
			AnimInstance->Montage_Play(EquipMontage);
			AnimInstance->Montage_JumpToSection(FName("Equip"), EquipMontage);
			CharState = ECharacterState::ECS_EquippedOneHandWeapone;
			AttackState = EAttackState::EAS_EquippingWeapon;
			
		}
	}
}

void AEchoCharacter::Dodge()
{
	if (Attributes && Attributes->GetEnerge() < Attributes->GetDodgeCost())
	{
		UE_LOG(LogTemp, Warning, TEXT("Dodge() OUT"));
		return;
	}

	PlayDodgeMontage();

	AttackState = EAttackState::EAS_Dodge;
	if (Attributes && MainInterface)
	{
		Attributes->UseEnergy(Attributes->GetDodgeCost());
		MainInterface->SetEnergyProgress(Attributes->GetEnergyPercent());
	}
}

void AEchoCharacter::Jump()
{
	if (AttackState == EAttackState::EAS_Stay && CharState != ECharacterState::ECS_Death)// IsUnoccupied()
	{
		Super::Jump();
	}
}

void AEchoCharacter::Attack()
{
	bool bCanAttack = 
		AttackState == EAttackState::EAS_Stay 
		&& CharState != ECharacterState::ECS_UnEquipped
		&& CharState != ECharacterState::ECS_Death;
	
	if(bCanAttack)
	{
		PlayAttackMontage();
		AttackState = EAttackState::EAS_Attack;
	}
}

void AEchoCharacter::PlayEquipMontage(FName SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();//��������ʾ��
	if (AnimInstance && EquipMontage) {
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
	}
}

void AEchoCharacter::AttackEnd()
{
	AttackState = EAttackState::EAS_Stay;
}

void AEchoCharacter::DodgeEnd()
{
	Super::DodgeEnd();

	AttackState = EAttackState::EAS_Stay;
}

void AEchoCharacter::HitReactEnd()
{
	AttackState = EAttackState::EAS_Stay;
}

void AEchoCharacter::EquipEnd()
{
	AttackState = EAttackState::EAS_Stay;
}

void AEchoCharacter::AttachWeaponToBack()//��ABP�е����������
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("BeibuSocket"));
	}
}

void AEchoCharacter::AttachWeaponToHand()//��ABP�е����������
{
	if (EquipMontage)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
	}
}

// Called every frame
void AEchoCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (Attributes && MainInterface)
	{
		Attributes->RecevieEnergy(DeltaTime);
		MainInterface->SetEnergyProgress(Attributes->GetEnergyPercent());
	}

}

// Called to bind functionality to input
void AEchoCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &AEchoCharacter::MoveForward);
	PlayerInputComponent->BindAxis(FName("MoveLeftRight"), this, &AEchoCharacter::MoveLeftRight);
	PlayerInputComponent->BindAxis(FName("TurnLeftRight"), this, &AEchoCharacter::TurnLeftRight);
	PlayerInputComponent->BindAxis(FName("LookUpDown"), this, &AEchoCharacter::LookUpDown);

	PlayerInputComponent->BindAction(FName("Jump"), IE_Pressed, this, &AEchoCharacter::Jump);
	PlayerInputComponent->BindAction(FName("Equip"), IE_Pressed, this, &AEchoCharacter::EKeyPress);
	PlayerInputComponent->BindAction(FName("Attack"), IE_Pressed, this, &AEchoCharacter::Attack);
	PlayerInputComponent->BindAction(FName("Dodge"), IE_Pressed, this, &AEchoCharacter::Dodge);
}

void AEchoCharacter::SetOverlappingItem(AItem* item)
{
	OverlappingItem = item;
}

void AEchoCharacter::AddSouls(ASoul* soul)
{
	if (Attributes && MainInterface)
	{
		Attributes->AddSouls(soul->GetSouls());
		UE_LOG(LogTemp, Warning, TEXT("SOULD: %d"), soul->GetSouls());
		MainInterface->SetTextSouls(Attributes->GetSouls());
	}
}

void AEchoCharacter::AddGolds(ATreasure* Treasure)
{
	if (Attributes && MainInterface)
	{
		Attributes->AddGolds(Treasure->GetGold());
		UE_LOG(LogTemp, Warning, TEXT("GOLD: %d"), Treasure->GetGold());
		MainInterface->SetTextGold(Attributes->GetGolds());
	}
}

void AEchoCharacter::InitializeMainInterface()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		AMainHUD* MainHUD = Cast<AMainHUD>(PlayerController->GetHUD());
		if (MainHUD)
		{
			MainInterface = MainHUD->GetMainInterface();
			if (MainInterface)
			{
				MainInterface->SetHealthProgress(Attributes->GetHealthPercent());
				MainInterface->SetEnergyProgress(1.f);
				MainInterface->SetTextGold(0);
				MainInterface->SetTextSouls(0);
			}
		}
	}
}

void AEchoCharacter::Die()
{
	Super::Die();

	CharState = ECharacterState::ECS_Death;
	AttackState = EAttackState::EAS_Stay;

}