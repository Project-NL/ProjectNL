#include "BasePlayerState.h"

#include "ProjectNL/GAS/NLAbilitySystemComponent.h"
#include "ProjectNL/GAS/Attribute/PlayerAttributeSet.h"

ABasePlayerState::ABasePlayerState()
{
	SetNetUpdateFrequency(100.f);

	AbilitySystemComponent = CreateDefaultSubobject<UNLAbilitySystemComponent>(
		"Ability System Component");
	AttributeSet = CreateDefaultSubobject<UPlayerAttributeSet>(
		"Player Attribute Set");
}

UAbilitySystemComponent* ABasePlayerState::GetAbilitySystemComponent() const
{
	return Cast<UNLAbilitySystemComponent>(AbilitySystemComponent.Get());
}