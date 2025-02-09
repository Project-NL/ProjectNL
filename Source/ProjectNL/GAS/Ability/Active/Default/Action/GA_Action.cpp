// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectNL/GAS/Ability/Active/Default/Action/GA_Action.h"

#include "ProjectNL/GAS/Ability/Active/Default/Dodge/GA_Dodge.h"
#include "ProjectNL/GAS/Ability/Utility/PlayMontageWithEvent.h"

UGA_Action::UGA_Action(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
}

void UGA_Action::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	PlayActionAnimation();
	
}

void UGA_Action::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                            const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Action::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true
    					, false);
}

void UGA_Action::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true
					, false);
}

void UGA_Action::PlayActionAnimation()
{
	ABaseCharacter* CurrentCharacter = Cast<ABaseCharacter>(
		GetAvatarActorFromActorInfo());
	UAbilitySystemComponent* TargetASC = CurrentCharacter->GetAbilitySystemComponent();
	// 조건에 따라 애니메이션 실행
	
	Task = UPlayMontageWithEvent::InitialEvent(this, NAME_None
																						, ActionAnimMontage
																						, FGameplayTagContainer());
	Task->OnCancelled.AddDynamic(this, &UGA_Action::OnCancelled);
	Task->OnInterrupted.AddDynamic(this, &UGA_Action::OnCancelled);
	Task->OnBlendOut.AddDynamic(this, &UGA_Action::OnCancelled);
	Task->OnCompleted.AddDynamic(this, &UGA_Action::OnCompleted);
	Task->ReadyForActivation();
}
