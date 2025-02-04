// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectNL/GAS/Ability/Active/Default/Knockback/GA_Knockback.h"
#include "ProjectNL/GAS/Ability/Active/Default/Knockback/AT_Knockback.h"
UGA_Knockback::UGA_Knockback(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
	// 거리는 300, 시간은 0.5초 정도로 예시

}

void UGA_Knockback::SetDamageResponse(FDamagedResponse& DamageResponse)
{
	DamagedResponse=DamageResponse;
}

void UGA_Knockback::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	KnockbackTask = UAT_Knockback::InitialEvent(this,DamagedResponse,DamageMontageLength);
	if (KnockbackTask)
	{
		// Task 실행
		KnockbackTask->OnCanceled.AddDynamic(this, &UGA_Knockback::OnCancelled);
		KnockbackTask->ReadyForActivation();
	}
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void UGA_Knockback::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}


void UGA_Knockback::OnCancelled()
{
	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);

}
