#include "GA_Guard.h"

#include "ProjectNL/Character/Player/PlayerCharacter.h"
#include "ProjectNL/Component/EquipComponent/EquipComponent.h"
#include "ProjectNL/GAS/Ability/Utility/PlayMontageWithEvent.h"
#include "ProjectNL/GAS/Attribute/PlayerAttributeSet.h"
#include "ProjectNL/Helper/AbilityHelper.h"
#include "ProjectNL/Helper/GameplayTagHelper.h"
#include "ProjectNL/Helper/StateHelper.h"

UGA_Guard::UGA_Guard(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
	
}

bool UGA_Guard::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	if (UBaseAttributeSet* AttributeSet = Cast<UBaseAttributeSet>(AbilityHelper::GetAttribute(GetAvatarActorFromActorInfo())))
	{
		if (AttributeSet->GetStamina() < 30) return false;
	}
	return FStateHelper::IsCombatMode(GetAbilitySystemComponentFromActorInfo());
}

void UGA_Guard::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	NlGameplayTags::SetGameplayTag(GetAbilitySystemComponentFromActorInfo(),NlGameplayTags::Status_Guard, 1, true);
	
	if (ABaseCharacter* CharacterInfo = Cast<ABaseCharacter>(GetAvatarActorFromActorInfo()))
	{
		UEquipComponent* EquipComponent = CharacterInfo->GetEquipComponent();
		check(EquipComponent)
		// EquipComponent가 다 실행되기 이전에 해당 Ability가 실행되는 경우는
		// 이 액션이 중단될 수 있으니 주의하면 좋다.
		// TODO: 애니메이션 Binding에 대해 AvatarSet에서 진행해 EquipComponent에 주입하는 것도 좋아보임
		SetCurrentMontage(EquipComponent->GetBlockAnim());
		
		AnimationTask = UPlayMontageWithEvent::InitialEvent(this, NAME_None,
			GetCurrentMontage(), FGameplayTagContainer());
		AnimationTask->OnCancelled.AddDynamic(this, &ThisClass::UGA_Guard::EndBlock);
		AnimationTask->OnCompleted.AddDynamic(this, &ThisClass::UGA_Guard::EndBlock);

		if (UNLAbilitySystemComponent* ASC = Cast<UNLAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo()))
		{
			ASC->OnDamageStartedNotified.AddDynamic(this, &ThisClass::StartBlock);
		}
	}
}

void UGA_Guard::StartBlock(const FDamagedResponse& DamagedResponse)
{
	if (UBaseAttributeSet* AttributeSet = Cast<UBaseAttributeSet>(AbilityHelper::GetAttribute(GetAvatarActorFromActorInfo())))
	{
		// 플레이어의 경우 스테미나가 30 이상 있어야만 Block 스킬이 발동.
		if (AttributeSet->GetStamina() > 30)
		{
			
			if (GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(NlGameplayTags::Status_Guard))
			{
				FStateHelper::ChangePlayerState(
					GetAbilitySystemComponentFromActorInfo(),
					NlGameplayTags::Status_Guard,
					NlGameplayTags::Status_Block, true);
				AttributeSet->SetStamina(AttributeSet->GetStamina() - 30);
				AnimationTask->ReadyForActivation();
			}
		} else
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		}
	}
}

void UGA_Guard::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UGA_Guard::EndBlock(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_Guard::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	if (UNLAbilitySystemComponent* ASC = Cast<UNLAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo()))
	{
		NlGameplayTags::SetGameplayTag(ASC, NlGameplayTags::Status_Guard, 0, true);
		NlGameplayTags::SetGameplayTag(ASC, NlGameplayTags::Status_Block, 0, true);
		ASC->OnDamageStartedNotified.RemoveDynamic(this, &ThisClass::StartBlock);
	}
}

void UGA_Guard::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
	AnimationTask->ExternalCancel();
}