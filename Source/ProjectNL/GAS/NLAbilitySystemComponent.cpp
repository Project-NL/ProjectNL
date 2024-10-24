#include "NLAbilitySystemComponent.h"

#include "NLAbilitySystemInitializationData.h"


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
