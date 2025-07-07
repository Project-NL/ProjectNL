#pragma once
#include "AbilitySystemComponent.h"
#include "ProjectNL/Helper/EnumHelper.h"
#include "ProjectNL/Helper/GameplayTagHelper.h"
#include "ProjectNL/Weapon/BaseWeapon.h"

class FStateHelper
{
public:
	FORCEINLINE static bool GetIsCharacterTargetMode(const UAbilitySystemComponent* Ability)
	{
		return Ability->HasMatchingGameplayTag(NlGameplayTags::Status_Targeting);
	}
	
	FORCEINLINE static EPlayerCombatWeaponState GetCharacterWeaponState(ABaseWeapon* MainWeapon, ABaseWeapon* SubWeapon)
{
		// 0. MainWeapon이 장착되지 않은 것은 비지니스 로직 상 SubWeapon도 장착되지 않는다.
		if (MainWeapon == nullptr)
		{
			return EPlayerCombatWeaponState::None;
		}

		// 1. MainWeapon이 한손 검이나 도끼 같은 일반 한손 무기의 경우
		if ((MainWeapon->GetWeaponType() == EUWeaponType::Sword || MainWeapon->
				GetWeaponType() == EUWeaponType::Axe) && MainWeapon->
			GetEquippedHandType()
			== EUEquippedHandType::OneHand)
		{
			// SubWeapon 존재 안하는 경우
			if (SubWeapon == nullptr)
			{
				return EPlayerCombatWeaponState::OnlyOneHandWeapon;
			}
			// SubWeapon으로 방패를 착용하고 있는 경우
			if (SubWeapon->GetWeaponType() == EUWeaponType::Shield)
			{
				return EPlayerCombatWeaponState::OneHandWeaponAndShield;
			}
			// SubWeapon가 도끼나 검 같은 일반 한손 무기의 경우, 위의 로직으로 인해 실드를 제외함.
			if (SubWeapon->GetEquippedHandType() == EUEquippedHandType::OneHand)
			{
				return EPlayerCombatWeaponState::DualHandWeapon;
			}
		}

		// 2. MainWeapon이 한손 단검인 경우 (Dagger는 비지니스 로직 상 두손 무기가 될 수 없다)
		if (MainWeapon->GetWeaponType() == EUWeaponType::Dagger)
		{
			// SubWeapon 존재 안하는 경우
			if (SubWeapon == nullptr)
			{
				return EPlayerCombatWeaponState::OnlyOneHandDagger;
			}
			// SubWeapon으로 방패를 착용하고 있는 경우
			if (SubWeapon->GetWeaponType() == EUWeaponType::Shield)
			{
				return EPlayerCombatWeaponState::OneHandDaggerAndShield;
			}
			// SubWeapon도 도끼나 검 같은 일반 한손 무기의 경우, 위의 로직으로 인해 실드를 제외함.
			if (SubWeapon->GetEquippedHandType() == EUEquippedHandType::OneHand)
			{
				return EPlayerCombatWeaponState::DualHandWeapon;
			}
		}

		// 3. MainWeapon이 두손 대검의 경우
		if (MainWeapon->GetWeaponType() == EUWeaponType::GreatSword && MainWeapon->
			GetEquippedHandType() == EUEquippedHandType::TwoHand)
		{
			return EPlayerCombatWeaponState::TwoHandGreatSword;
		}

		// 4. MainWeapon이 두손 창인 경우
		if (MainWeapon->GetWeaponType() == EUWeaponType::Spear && MainWeapon->
			GetEquippedHandType() == EUEquippedHandType::TwoHand)
		{
			return EPlayerCombatWeaponState::TwoHandSpear;
		}

		// 5. MainWeapon이 두손 도끼나 망치인 경우
		if ((MainWeapon->GetWeaponType() == EUWeaponType::Axe || MainWeapon->
				GetWeaponType() == EUWeaponType::Hammer) && MainWeapon->
			GetEquippedHandType() == EUEquippedHandType::TwoHand)
		{
			return EPlayerCombatWeaponState::TwoHandBlunt;
		}

		// 6. MainWeapon이 건틀릿으로 설정되어 있으면 특수 세트 (이름 미정)가 적용된 것 이기에
		// 굳이 SubWeapon나 HandType을 검증하지 않는다.
		if (MainWeapon->GetWeaponType() == EUWeaponType::Gauntlet)
		{
			return EPlayerCombatWeaponState::OnlyDualFist;
		}

		// 7. MainWeapon이 활인 경우도 특수 세트이기에 자동으로 두손 무기로 처리된다.
		if (MainWeapon->GetWeaponType() == EUWeaponType::Bow)
		{
			return EPlayerCombatWeaponState::TwoHandBow;
		}

		// 8. MainWeapon이 카타나(자동 두손 무기)인 경우
		if (MainWeapon->GetWeaponType() == EUWeaponType::Katana)
		{
			return EPlayerCombatWeaponState::TwoHandBow;
		}

		// 9. MainWeapon이 스태프인 경우
		if (MainWeapon->GetWeaponType() == EUWeaponType::Staff)
		{
			return EPlayerCombatWeaponState::TwoHandMagicStaff;
		}

		return EPlayerCombatWeaponState::None;
	}

	FORCEINLINE static bool IsPlayerIdle(const UAbilitySystemComponent* Ability)
	{
		return Ability->HasMatchingGameplayTag(NlGameplayTags::State_Idle);
	}

	FORCEINLINE static bool IsCombatMode(const UAbilitySystemComponent* Ability)
	{
		return Ability->HasMatchingGameplayTag(NlGameplayTags::Status_Combat);
	}

	FORCEINLINE static bool IsPlayerStatusGuard(const UAbilitySystemComponent* Ability)
	{
		return Ability->HasMatchingGameplayTag(NlGameplayTags::Status_GuardReady);
	}

	FORCEINLINE static void ChangePlayerState(UAbilitySystemComponent* Ability
																						, const FGameplayTag PreviousTag
																						, const FGameplayTag NewTag
																						, const bool bIsReplicated = false)
	{
		Ability->SetLooseGameplayTagCount(PreviousTag, 0);
		Ability->SetLooseGameplayTagCount(NewTag, 1);
		if (bIsReplicated)
		{
			Ability->SetReplicatedLooseGameplayTagCount(PreviousTag, 0);
			Ability->SetReplicatedLooseGameplayTagCount(NewTag, 1);
		}
	}
};
