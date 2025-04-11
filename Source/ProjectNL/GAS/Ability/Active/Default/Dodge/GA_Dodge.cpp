#include "GA_Dodge.h"

#include "ProjectNL/GAS/Ability/Utility/PlayMontageWithEvent.h"
#include "ProjectNL/Component/EquipComponent/EquipComponent.h"
#include "ProjectNL/Helper/EnumHelper.h"
#include "ProjectNL/Helper/LocateHelper.h"
#include "ProjectNL/Helper/StateHelper.h"

UGA_Dodge::UGA_Dodge(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
}

void UGA_Dodge::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (ABaseCharacter* OwnerCharacter = Cast<ABaseCharacter>(ActorInfo->AvatarActor.Get()))
	{
		const EMovementDirection CurrentDirection =
			FStateHelper::GetIsCharacterTargetMode(GetAbilitySystemComponentFromActorInfo())
				? FLocateHelper::GetDirectionByMovementData(OwnerCharacter->GetMovementVector()) : EMovementDirection::F;
			
		if (UAnimMontage* StepAnim = OwnerCharacter->GetEquipComponent()
			->GetStepAnim().GetAnimationByDirection(CurrentDirection))
		{
			SetCurrentMontage(StepAnim);
				
			UPlayMontageWithEvent* Task = UPlayMontageWithEvent::InitialEvent(this,
							NAME_None, GetCurrentMontage(), FGameplayTagContainer());
			Task->OnCancelled.AddDynamic(this, &ThisClass::OnCancelled);
			Task->OnCompleted.AddDynamic(this, &ThisClass::OnCompleted);
			Task->ReadyForActivation();
		} else
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo,
				true, false);
		}
	} else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo,
			true, false);
	}
}

void UGA_Dodge::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Dodge::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true
					, false);
}

void UGA_Dodge::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true
					, true);
}

