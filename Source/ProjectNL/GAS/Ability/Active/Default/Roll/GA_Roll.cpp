// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Roll.h"
#include "ProjectNL/GAS/Ability/Utility/PlayMontageWithEvent.h"
#include "ProjectNL/Component/EquipComponent/EquipComponent.h"
#include "ProjectNL/Helper/EnumHelper.h"
#include "ProjectNL/Helper/LocateHelper.h"
#include "ProjectNL/Helper/StateHelper.h"

UGA_Roll::UGA_Roll(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
}

void UGA_Roll::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (ABaseCharacter* OwnerCharacter = Cast<ABaseCharacter>(ActorInfo->AvatarActor.Get()))
	{
		float Angle = FLocateHelper::GetDeltaAngle(OwnerCharacter->GetVelocity(),OwnerCharacter->GetActorForwardVector());
		const EMovementDirection CurrentDirection =
			FStateHelper::GetIsCharacterTargetMode(GetAbilitySystemComponentFromActorInfo())
						? FLocateHelper::GetDirectionByAngle(Angle) : EMovementDirection::F;
			
		if (UAnimMontage* StepAnim = OwnerCharacter->GetEquipComponent()
			->GetEvadeAnim().GetAnimationByDirection(CurrentDirection))
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

void UGA_Roll::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Roll::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true
					, false);
}

void UGA_Roll::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true
					, true);
}

