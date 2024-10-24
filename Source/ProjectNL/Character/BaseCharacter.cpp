#include "BaseCharacter.h"


ABaseCharacter::ABaseCharacter()
{
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::Server_ApplyGameplayEffectToSelf_Implementation(
	TSubclassOf<UGameplayEffect> Effect, const uint32 Level)
{
	if (GetLocalRole() != ROLE_Authority)
	{
		Server_ApplyGameplayEffectToSelf(Effect);
		return;
	}

	if (!IsValid(AbilitySystemComponent))
	{
		return;
	}

	AbilitySystemComponent->ApplyGameplayEffectToSelf(
		Effect.GetDefaultObject(), Level
		, AbilitySystemComponent->MakeEffectContext());
}

void
ABaseCharacter::Server_RemoveActiveGameplayEffectBySourceEffect_Implementation(
	TSubclassOf<UGameplayEffect> Effect)
{
	if (GetLocalRole() != ROLE_Authority)
	{
		Server_ApplyGameplayEffectToSelf(Effect);
		return;
	}

	if (!IsValid(AbilitySystemComponent))
	{
		return;
	}

	AbilitySystemComponent->RemoveActiveGameplayEffectBySourceEffect(
		Effect, AbilitySystemComponent);
}