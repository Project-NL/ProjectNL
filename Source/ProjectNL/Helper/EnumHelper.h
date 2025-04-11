#pragma once

class FEnumHelper
{
public:
	template <typename EnumType>
	static FString GetClassEnumKeyAsString(const EnumType EnumeratorValue)
	{
		TArray<FString> EnumKey;
		UEnum::GetValueAsString(EnumeratorValue).
			ParseIntoArray(EnumKey, TEXT("::"));

		if (EnumKey.Num() == 1)
		{
			return EnumKey[0];
		}
		return EnumKey[1];
	}
};

UENUM(BlueprintType)
enum class EEntityCategory: uint8
{
	Undefined UMETA(DisplayName = "Undefined")
	, Player UMETA(DisplayName = "Player"), Neutral UMETA(DisplayName = "Neutral")
	, Enemy UMETA(DisplayName = "Enemy"),
};

UENUM(BlueprintType)
enum class EInputIDType: uint8
{
	None UMETA(DisplayName = "None"), Jump UMETA(DisplayName = "Jump")
	, Attack UMETA(DisplayName = "Attack"), Guard UMETA(DisplayName = "Guard")
	, Sprint UMETA(DisplayName = "Sprint")
	, ToggleCombatMode UMETA(DisplayName = "ToggleCombatMode")
	, FixedCamera UMETA(DisplayName = "FixedCamera")
	, Dodge UMETA(DisplayName = "Dodge")
	, Skill1 UMETA(DisplayName = "Skill1"), Skill2 UMETA(DisplayName = "Skill2")
	, Skill3 UMETA(DisplayName = "Skill3"), Skill4 UMETA(DisplayName = "Skill4")};

UENUM(BlueprintType)
enum class EUEquippedHandType : uint8
{
	Empty UMETA(DisplayName = "Empty"), OneHand UMETA(DisplayName = "OneHand")
	, TwoHand UMETA(DisplayName = "TwoHand"),
};

UENUM(BlueprintType)
enum class EHandEquipStatus : uint8
{
	Empty UMETA(DisplayName = "Empty"), OnlyMain UMETA(DisplayName = "OnlyMain")
	, OnlySub UMETA(DisplayName = "OnlySub"), Dual UMETA(DisplayName = "Dual"),
};

UENUM(BlueprintType)
enum class EUWeaponType : uint8
{
	None UMETA(DisplayName = "None"), Axe UMETA(DisplayName = "Axe")
	, Sword UMETA(DisplayName = "Sword"), Shield UMETA(DisplayName = "Shield")
	, Spear UMETA(DisplayName = "Spear") , Dagger UMETA(DisplayName = "Dagger")
	, Hammer UMETA(DisplayName = "Hammer"), Staff UMETA(DisplayName = "Staff")
	, GreatSword UMETA(DisplayName = "GreatSword")
	, Gauntlet UMETA(DisplayName = "Gauntlet")
	, Bow UMETA(DisplayName = "Bow"), Katana UMETA(DisplayName = "Katana")};


UENUM(BlueprintType)
enum class EWeaponAttachPosition: uint8
{
	Back UMETA(DisplayName = "Back"), WaistBack UMETA(DisplayName ="WaistBack")};

UENUM(BlueprintType)
enum class EPlayerCombatWeaponState: uint8
{
	// 일반 공격 무기
	None UMETA(DisplayName = "None")
	, OnlyOneHandWeapon UMETA(DisplayName = "OnlyOneHandWeapon")
	, OneHandWeaponAndShield UMETA(DisplayName = "OneHandWeaponAndShield")
	, DualHandWeapon UMETA(DisplayName = "DualHandWeapon")
	, OnlyOneHandDagger UMETA(DisplayName = "OnlyOneHandDagger")
	, OneHandDaggerAndShield UMETA(DisplayName = "OneHandDaggerAndShield")
	, TwoHandGreatSword UMETA(DisplayName = "TwoHandGreatSword")
	, TwoHandSpear UMETA(DisplayName = "TwoHandSpear")
	, TwoHandBlunt UMETA(DisplayName = "TwoHandBlunt")
	// 특수 공격 무기
	, OnlyDualFist UMETA(DisplayName = "OnlyDualFist")
	, TwoHandBow UMETA(DisplayName = "TwoHandBow")
	, TwoHandKatana UMETA(DisplayName = "TwoHandKatana")
	, TwoHandMagicStaff UMETA(DisplayName = "TwoHandMagicStaff")};


UENUM(BlueprintType)
enum class EMovementDirection: uint8
{
	F UMETA(DisplayName = "Forward")
	, FL UMETA(DisplayName = "ForwardLeft")
	, FR UMETA(DisplayName = "ForwardRight")
	, L UMETA(DisplayName = "Left")
	, R UMETA(DisplayName = "Right")
	, B UMETA(DisplayName = "Backward")
	, BL UMETA(DisplayName = "BackwardLeft")
	, BR UMETA(DisplayName = "BackwardRight")};

UENUM(BlueprintType)
enum class ETargetHeight: uint8
{
	Low UMETA(DisplayName = "Low")
	, Middle UMETA(DisplayName = "Middle")
	, High UMETA(DisplayName = "High")};