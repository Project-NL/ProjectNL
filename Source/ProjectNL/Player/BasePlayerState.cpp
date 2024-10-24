#include "BasePlayerState.h"
#include "ProjectNL/GAS/Attribute/PlayerAttributeSet.h"

ABasePlayerState::ABasePlayerState()
{
	NetUpdateFrequency = 100.0f;

	AbilitySystemComponent = CreateDefaultSubobject<UNLAbilitySystemComponent>(
		"Ability System Component");
	AttributeSet = CreateDefaultSubobject<UPlayerAttributeSet>(
		"Player Attribute Set");
}

UAbilitySystemComponent* ABasePlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}