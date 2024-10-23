#pragma once

UENUM(BlueprintType)
enum class ECharacterState : uint8 {
	ECS_UnEquipped UMETA(DisplayName = "Un_Equip"),//没有装备
	ECS_EquippedOneHandWeapone UMETA(DisplayName = "Equip_One_Hand_Weapon"),//单手武器
	ECS_EquippedTwoHandWeapon UMETA(DisplayName = "Equip_Two_Hand_Weapon"),//双手武器
	ECS_Death UMETA(DisplayName = "Death")//死亡
}; 


UENUM(BlueprintType)
enum class EAttackState : uint8 {
	EAS_Stay UMETA(DisplayName = "StayTime"),//攻击待机Unoccupied
	EAS_HitReaction UMETA(DisplayName = "HitReaction"),//受到攻击
	EAS_Attack UMETA(DisplayName = "AttackTime"),//攻击中
	EAS_EquippingWeapon UMETA(DisplayName = "Equipping"),//装备武器中
	EAS_Dodge UMETA(DisplayName = "Dodge")//闪避中
	
};

UENUM(BlueprintType)
enum EDeathPose{
	EDP_EDeathPose1 UMETA(DisplayName = "DeathPose1"),
	EDP_EDeathPose2 UMETA(DisplayName = "DeathPose2"),
	EDP_EDeathPose3 UMETA(DisplayName = "DeathPose3"),
	EDP_EDeathPose4 UMETA(DisplayName = "DeathPose4"),

	EDP_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EEnemyState : uint8 {
	EES_NoState UMETA(DisplayName = "NoState"),

	EES_Dead UMETA(DisplayName = "Dead"),
	EES_Patrolling UMETA(DisplayName = "Patrolling"),
	EES_chasing UMETA(DisplayName = "chasing"),
	EES_Attack UMETA(DisplayName = "Attack"),
	EES_Battle UMETA(DisplayName = "Battle")  //视频里面是Engaged
};