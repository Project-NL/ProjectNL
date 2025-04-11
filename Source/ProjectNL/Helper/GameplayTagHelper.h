#pragma once
#include "NativeGameplayTags.h"

struct FGameplayTag;
class UAbilitySystemComponent;

namespace NlGameplayTags
{
	PROJECTNL_API void AddGameplayTag(UAbilitySystemComponent* ASC, const FGameplayTag Tag, const int32 Count, const bool bIsReplicated = false);
	PROJECTNL_API void RemoveGameplayTag(UAbilitySystemComponent* ASC, const FGameplayTag Tag, const int32 Count, const bool bIsReplicated = false);
	PROJECTNL_API void SetGameplayTag(UAbilitySystemComponent* ASC, const FGameplayTag Tag, const int32 Count, const bool bIsReplicated = false);

	// Ability 관련 최상위 트리 태그로 하나만 들어갈 수 있음
	PROJECTNL_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability)
	PROJECTNL_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Guard)
	
	// 특정 Actor의 카테고리 관련 최상위 트리 태그로 하나만 들어갈 수 있음 (ex. 플레이어, 엔티티, 오브젝트 등)
	PROJECTNL_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Category)
	PROJECTNL_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Category_Entity)

	PROJECTNL_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data)
	PROJECTNL_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_AttackDirection)
	PROJECTNL_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_IsHitStop)
	
	// 특정 액터의 행동 상태에 대한 최상위 트리 태그로 하나만 들어갈 수 있음
	PROJECTNL_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State)
	PROJECTNL_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Idle)
	PROJECTNL_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_GrabWeapon)
	PROJECTNL_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_PutWeapon)
	PROJECTNL_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Attack)
	PROJECTNL_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Attack_Combo)
	PROJECTNL_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Attack_Heavy)
	PROJECTNL_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Attack_Jump)

	// 특정 액터의 현재 상황에 대해 나타내는 태그로 여러개가 들어갈 수 있음.
	PROJECTNL_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status)
	PROJECTNL_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Block)
	PROJECTNL_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Guard)
	PROJECTNL_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Combat)
	PROJECTNL_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Targeting)
	PROJECTNL_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_IsFalling)

	// GC에 대한 Tag 정보
	PROJECTNL_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Utility_HitStop);
}
