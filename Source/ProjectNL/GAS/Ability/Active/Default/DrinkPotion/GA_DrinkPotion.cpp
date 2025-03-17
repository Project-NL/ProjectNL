// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectNL/GAS/Ability/Active/Default/DrinkPotion/GA_DrinkPotion.h"

#include "ProjectNL/GAS/Ability/Utility/PlayMontageWithEvent.h"
#include "ProjectNL/Item/PotionItem.h"

UGA_DrinkPotion::UGA_DrinkPotion(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	
}

void UGA_DrinkPotion::SetPotionItem(APotionItem* potionItem)
{
	PotionItem = potionItem;
}

void UGA_DrinkPotion::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                      const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	PlayActionAnimation();
}

void UGA_DrinkPotion::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	PotionItem->Destroy();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_DrinkPotion::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true
						, false);
	if (HealEffect)
	{
		UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
		if (!AbilitySystemComponent)
		{
			return;
		}
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->
			MakeEffectContext();
		EffectContext.AddSourceObject(GetAvatarActorFromActorInfo());

		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(
			HealEffect, 1.0f, EffectContext);

		if (SpecHandle.IsValid())
		{
			AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(
				*SpecHandle.Data.Get());
		}
	}
}

void UGA_DrinkPotion::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true
						, false);
	
}

void UGA_DrinkPotion::PlayActionAnimation()
{
	ABaseCharacter* CurrentCharacter = Cast<ABaseCharacter>(
	GetAvatarActorFromActorInfo());
	UAbilitySystemComponent* TargetASC = CurrentCharacter->GetAbilitySystemComponent();
	// 조건에 따라 애니메이션 실행
	
	PlayMontageWithEventTask = UPlayMontageWithEvent::InitialEvent(this, NAME_None
																						, ActionAnimMontage
																						, FGameplayTagContainer());
	PlayMontageWithEventTask->OnCancelled.AddDynamic(this, &UGA_DrinkPotion::OnCancelled);
	PlayMontageWithEventTask->OnInterrupted.AddDynamic(this, &UGA_DrinkPotion::OnCancelled);
	PlayMontageWithEventTask->OnBlendOut.AddDynamic(this, &UGA_DrinkPotion::OnCompleted);
	PlayMontageWithEventTask->OnCompleted.AddDynamic(this, &UGA_DrinkPotion::OnCompleted);
	PlayMontageWithEventTask->ReadyForActivation();
}
