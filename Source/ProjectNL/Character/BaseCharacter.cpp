#include "BaseCharacter.h"
#include "ProjectNL/Helper/EnumHelper.h"

#include "ProjectNL/Component/EquipComponent/EquipComponent.h"
#include "ProjectNL/GAS/Attribute/BaseAttributeSet.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "ProjectNL/Component/SkillComponent/TimeRecallComponent.h"
#include "ProjectNL/Helper/AbilityHelper.h"
#include "ProjectNL/Helper/GameplayTagHelper.h"


ABaseCharacter::ABaseCharacter()
{
	EntityType = EEntityCategory::Undefined;
	EquipComponent = CreateDefaultSubobject<UEquipComponent>("Equip Component");
	TimeRecallComponent=CreateDefaultSubobject<UTimeRecallComponent>("TimeRecallComponent");
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseCharacter::Initialize()
{
	if (AbilitySystemComponent)
	{
		if (GetEquipComponent()->GetIsFirstEquipWeapon())
		{
			AbilitySystemComponent->AddLooseGameplayTag(NlGameplayTags::Status_Combat);
		}
	}
}

void ABaseCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
	if (AbilitySystemComponent)
	{
		if (GetCharacterMovement()->IsFalling())
		{
			AbilitySystemComponent->
				SetLooseGameplayTagCount(NlGameplayTags::Status_IsFalling, 1);
		} else
		{
			AbilitySystemComponent->
				SetLooseGameplayTagCount(NlGameplayTags::Status_IsFalling, 0);
		}
	}
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


void ABaseCharacter::MovementSpeedChanged(const FOnAttributeChangeData& Data)
{
	const float MovementSpeed = Data.NewValue;

	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
}

void ABaseCharacter::SetViewItemOnHand(const FItemInfoData& NewItemInfo)
{
	
}

