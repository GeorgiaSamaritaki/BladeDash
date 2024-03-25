#pragma once

UENUM(BlueprintType)
enum class ECharacterState : uint8 {
	ECS_Unequipped UMETA(DisplayName = "Unequipped"),
	ECS_EquippedOneHandedWeapon UMETA(DisplayName = "Equipped One-Handed Weapon"),
	ECS_EquippedTwoHandedWeapon UMETA(DisplayName = "Equipped Two-Handed Weapon")
};

UENUM(BlueprintType)
enum class EActionState : uint8 {
	EAS_Unoccupied UMETA(DisplayName = "Unequipped"),
	EAS_Attacking UMETA(DisplayName = "Attacking"),
	EAS_EquippingWeapon UMETA(DisplayName = "EquippingWeapon")
};

UENUM(BlueprintType)
enum class EDeathPose : uint8 {
	EDP_Alive UMETA(DisplayName = "Alive"),
	EDP_Dead1 UMETA(DisplayName = "Dead1"),
	EDP_Dead2 UMETA(DisplayName = "Dead2"),
	EDP_Dead3 UMETA(DisplayName = "Dead3"),
	EDP_Dead4 UMETA(DisplayName = "Dead4"),
	EDP_Dead5 UMETA(DisplayName = "Dead5")
};

UENUM(BlueprintType)
enum class EEnemyState : uint8 {
	EES_Patrolling UMETA(DisplayName = "Patrolling"),
	EES_Chasing UMETA(DisplayName = "Chasing"),
	EAS_Attacking UMETA(DisplayName = "Attacking")
};