#include "GA_Sprint.h"

#include "AbilitySystemComponent.h"
#include "ProjectNL/Component/EquipComponent/EquipComponent.h"
#include "ProjectNL/GAS/Ability/Utility/PlayMontageWithEvent.h"
#include "ProjectNL/Helper/EnumHelper.h"
#include "ProjectNL/Helper/LocateHelper.h"
#include "ProjectNL/Helper/StateHelper.h"

UGA_Sprint::UGA_Sprint(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	HoldExistTime = 1;
}

bool UGA_Sprint::CanActivateAbility(const FGameplayAbilitySpecHandle Handle
																, const FGameplayAbilityActorInfo* ActorInfo
																, const FGameplayTagContainer* SourceTags
																, const FGameplayTagContainer* TargetTags
																, FGameplayTagContainer* OptionalRelevantTags)
const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags
																, OptionalRelevantTags))
	{
		return false;
	}
	
	return true;
}

void UGA_Sprint::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ActiveTime = FDateTime::Now();
	
	if (BuffEffect)
	{
		FGameplayEffectContextHandle EffectContext = GetAbilitySystemComponentFromActorInfo()->
			MakeEffectContext();
		EffectContext.AddSourceObject(GetAvatarActorFromActorInfo());

		FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(
			BuffEffect, 1.0f, EffectContext);
		
		if (SpecHandle.IsValid())
		{
			ActiveHandle = GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(
				*SpecHandle.Data.Get());
		}
	}
}

void UGA_Sprint::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	
	if (BuffEffect)
	{
		GetAbilitySystemComponentFromActorInfo()->RemoveActiveGameplayEffect(ActiveHandle);
	}

	const FTimespan HoldDuration = FDateTime::Now() - ActiveTime;

	if (HoldDuration.GetTotalSeconds() <= HoldExistTime)
	{
		if (ABaseCharacter* OwnerCharacter = Cast<ABaseCharacter>(ActorInfo->AvatarActor.Get()))
		{
			const EMovementDirection CurrentDirection =
				FStateHelper::GetIsCharacterTargetMode(GetAbilitySystemComponentFromActorInfo())
					? FLocateHelper::GetDirectionByMovementData(OwnerCharacter->GetMovementVector()) : EMovementDirection::F;
			
			if (UAnimMontage* EvadeAnim = OwnerCharacter->GetEquipComponent()
				->GetEvadeAnim().GetAnimationByDirection(CurrentDirection))
			{
				SetCurrentMontage(EvadeAnim);
				
				UPlayMontageWithEvent* Task = UPlayMontageWithEvent::InitialEvent(this,
                				NAME_None, GetCurrentMontage(), FGameplayTagContainer());
                Task->OnCompleted.AddDynamic(this, &ThisClass::EndEvade);
                Task->ReadyForActivation();
			} else
			{
				EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo,
					true, false);
			}
		}
	} else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo,
					true, false);
	}
}

void UGA_Sprint::EndEvade(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo,
		true, false);
}

void UGA_Sprint::CancelAbility(const FGameplayAbilitySpecHandle Handle
														, const FGameplayAbilityActorInfo* ActorInfo
														, const FGameplayAbilityActivationInfo
														ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
	GetAbilitySystemComponentFromActorInfo()->
		RemoveActiveGameplayEffectBySourceEffect(BuffEffect, GetAbilitySystemComponentFromActorInfo());
}

void UGA_Sprint::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
