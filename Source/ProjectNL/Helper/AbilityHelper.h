#pragma once

#include "GameplayAbilitySpec.h"
#include "AbilitySystemComponent.h"
class UAttributeSet;
class AbilityHelper
{
public:
	static UAttributeSet* GetAttribute(AActor* Owner);

	template <typename T>
	static T* FindActiveGAAbility(UAbilitySystemComponent* ASC);
};

template<typename T>
inline T* AbilityHelper::FindActiveGAAbility(UAbilitySystemComponent* ASC)
{
	if (!ASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("AbilityHelper::FindActiveGAAbility - Invalid parameters!"));
		return nullptr;
	}

	TArray<FGameplayAbilitySpecHandle> ActiveAbilities;
	ASC->GetAllAbilities(ActiveAbilities);

	for (const FGameplayAbilitySpecHandle& AbilityHandle : ActiveAbilities)
	{
		FGameplayAbilitySpec* AbilitySpec = ASC->FindAbilitySpecFromHandle(AbilityHandle);
		if (AbilitySpec && AbilitySpec->IsActive())
		{
			UGameplayAbility* AbilityInstance = AbilitySpec->GetPrimaryInstance();
			if (AbilityInstance && AbilityInstance->GetClass()->IsChildOf(T::StaticClass()))
			{
				return Cast<T>(AbilityInstance);
			}
		}
	}

	return nullptr;
}
