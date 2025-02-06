// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectNL/GAS/Ability/Active/Default/Targeting/GA_TargetingEnemy.h"

#include "AT_TargetingEnemy.h"
#include "ProjectNL/GAS/Ability/Utility/PlayMontageWithEvent.h"

UGA_TargetingEnemy::UGA_TargetingEnemy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UGA_TargetingEnemy::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	FGameplayTagContainer EventTag;
	TargetingEnemyTask = UAT_TargetingEnemy::InitialEvent(this,TargetingSpeedEffect);
	TargetingEnemyTask->OnCanceled.AddDynamic(this, &UGA_TargetingEnemy::OnCancelled);
	TargetingEnemyTask->ReadyForActivation();
}

void UGA_TargetingEnemy::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_TargetingEnemy::InputPressed(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
	TargetingEnemyTask->TargetNearestEnemy();
}

void UGA_TargetingEnemy::OnCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

