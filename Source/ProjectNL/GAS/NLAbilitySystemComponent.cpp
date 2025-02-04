#include "NLAbilitySystemComponent.h"

#include "AbilitySystemGlobals.h"
#include "NLAbilitySystemInitializationData.h"
#include "Ability/Utility/BaseInputTriggerAbility.h"
#include "ProjectNL/Helper/GameplayTagHelper.h"
#include "ProjectNL/Helper/StateHelper.h"


UNLAbilitySystemComponent::UNLAbilitySystemComponent()
{
	ReplicationMode = EGameplayEffectReplicationMode::Mixed;
	SetIsReplicatedByDefault(true);
}

void UNLAbilitySystemComponent::InitializeAbilitySystem(
	const FNLAbilitySystemInitializationData& InitData)
{
	if (GetIsInitialized())
	{
		return;
	}

	if (GetOwnerRole() == ROLE_Authority)
	{
		if (!InitData.GameplayAbilities.IsEmpty())
		{
			for (TSubclassOf<UBaseInputTriggerAbility> Ability : InitData.
					GameplayAbilities)
			{
				UBaseInputTriggerAbility* InputAbility = Ability->GetDefaultObject<
					UBaseInputTriggerAbility>();

				GiveAbility(FGameplayAbilitySpec(
					Ability, InputAbility->GetAbilityLevel()
					, static_cast<uint32>(InputAbility->GetInputID()), this));
			}
		}
		
		if (IsValid(InitData.FirstNameAbility))
		{
			UGameplayAbility* Ability = InitData.FirstNameAbility->GetDefaultObject<
				UGameplayAbility>();
			GiveAbility(FGameplayAbilitySpec(
				Ability, Ability->GetAbilityLevel(), -1, this));
		}

		if (IsValid(InitData.SecondNameAbility))
		{
			UGameplayAbility* Ability = InitData.SecondNameAbility->GetDefaultObject<
				UGameplayAbility>();
			GiveAbility(FGameplayAbilitySpec(
				Ability, Ability->GetAbilityLevel(), -1, this));
		}

		if (IsValid(InitData.ThirdNameAbility))
		{
			UGameplayAbility* Ability = InitData.ThirdNameAbility->GetDefaultObject<
				UGameplayAbility>();
			GiveAbility(FGameplayAbilitySpec(
				Ability, Ability->GetAbilityLevel(), -1, this));
		}
		if (IsValid(InitData.KnockAbility))
		{
			UGameplayAbility* Ability = InitData.KnockAbility->GetDefaultObject<
				UGameplayAbility>();
			GiveAbility(FGameplayAbilitySpec(
				Ability, Ability->GetAbilityLevel(), -1, this));
		}
		
		if (!InitData.GameplayEffects.IsEmpty())
		{
			for (TSubclassOf<UGameplayEffect> Effect : InitData.GameplayEffects)
			{
				ApplyGameplayEffectToSelf(Effect.GetDefaultObject(), 1
																	, MakeEffectContext());
			}
		}
	}

	if (!InitData.GameplayTags.IsEmpty())
	{
		AddLooseGameplayTags(InitData.GameplayTags);
	}
	
	SetIsInitialized(true);
}

void UNLAbilitySystemComponent::ReceiveDamage(const FDamagedResponse& DamagedResponse) const
{
	OnDamageStartedNotified.Broadcast(DamagedResponse);

	if (HasMatchingGameplayTag(NlGameplayTags::Status_Block))
	{
		const_cast<UNLAbilitySystemComponent*>(this)->
			RemoveLooseGameplayTag(NlGameplayTags::Status_Block);
		return;
	}
	// TODO: 추후 데미지 제공한 Causer도 같이 전송해도 무방할 듯
	OnDamageReactNotified.Broadcast(DamagedResponse);
	if (DamagedResponse.IsHitStop)
	{
		FGameplayCueParameters Param;
		Param.AbilityLevel = DamagedResponse.Damage / LevelByDamaged;
		Param.EffectCauser = DamagedResponse.SourceActor;
		const_cast<UNLAbilitySystemComponent*>(this)->ExecuteGameplayCue(NlGameplayTags::GameplayCue_Utility_HitStop, Param);
	}
}
