#include "GameplayTagHelper.h"

#include "AbilitySystemComponent.h"
#include "NativeGameplayTags.h"

namespace NlGameplayTags
{
	void AddGameplayTag(UAbilitySystemComponent* ASC, const FGameplayTag Tag, const int32 Count, const bool bIsReplicated)
	{
		if (bIsReplicated)
		{
			ASC->SetReplicatedLooseGameplayTagCount(Tag, Count);
		}
		ASC->SetLooseGameplayTagCount(Tag, Count);
	}

	void RemoveGameplayTag(UAbilitySystemComponent* ASC, const FGameplayTag Tag, const int32 Count, const bool bIsReplicated)
	{
		if (bIsReplicated)
		{
			ASC->SetReplicatedLooseGameplayTagCount(Tag, Count);
		}
		ASC->SetLooseGameplayTagCount(Tag, Count);
	}

	void SetGameplayTag(UAbilitySystemComponent* ASC, const FGameplayTag Tag, const int32 Count, const bool bIsReplicated)
	{
		if (bIsReplicated)
		{
			ASC->SetReplicatedLooseGameplayTagCount(Tag, Count);
		}
		ASC->SetLooseGameplayTagCount(Tag, Count);
	}

	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability, "Ability", "Ability 관련 최상위 트리 태그");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Guard, "Ability.Guard", "Ability - 기본스킬 - 방어");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Category, "Category", "특정 Actor의 카테고리 관련 최상위 트리 태그");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Category_Entity, "Category.Entity", "특정 Actor의 카테고리가 엔티티(살아있는 생물체)인 경우");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Data, "Data", "SetByCaller를 위해 전달하는 데이터 전용 태그");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Data_AttackDirection, "Data.AttackDirection", "공격 시 공격한 방향 값을 전달하는 태그");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Data_IsHitStop, "Data.IsHitStop", "HitStop을 적용 시킬 것인지를 확인하는 태그");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State, "State", "특정 액터의 행동 상태에 대한 최상위 트리 태그로 하나만 들어갈 수 있음");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Idle, "State.Idle", "특정 액터의 행동 상태가 어떠한 동작하지 않는 대기 상태인 경우 태그");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_GrabWeapon, "State.GrabWeapon", "특정 액터가 현재 무기를 뽑는 상태일 때를 나타내는 태그");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_PutWeapon, "State.PutWeapon", "특정 액터가 현재 무기를 집어넣는 상태일 때를 나타내는 태그");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Attack, "State.Attack", "특정 액터가 공격 중인 상태임을 나타내는 태그");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Attack_Combo, "State.Attack.Combo", "특정 액터가 콤보 공격 중인 상태임을 나타내는 태그");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Attack_Heavy, "State.Attack.Heavy", "특정 액터가 강공격 중인 상태임을 나타내는 태그");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Attack_Jump, "State.Attack.Jump", "특정 액터가 점프 공격 중인 상태임을 나타내는 태그");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status, "Status", "특정 액터의 현재 상황에 대해 나타내는 태그로 여러개가 들어갈 수 있음");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_Combat, "Status.Combat", "특정 액터가 현재 전투를 준비한 상황에 대한 태그");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_Block, "Status.Block", "특정 액터가 현재 방어 상태임을 의미하는 태그");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_Guard, "Status.Guard", "특정 액터가 현재 방어 준비 상태임을 의미하는 태그");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_Targeting, "Status.Targeting", "특정 캐릭터가 현재 외부 적에 대해 시점을 고정한 상태를 의미하는 태그");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_IsFalling, "Status.IsFalling", "특정 캐릭터가 현재 추락중인 상태임을 의미함");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_Utility_HitStop, "GameplayCue.Utility.HitStop", "실행 시 잠시 캐릭터들이 멈췄다가 다시 실행되는 구조");
}