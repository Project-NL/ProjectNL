// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectNL/GAS/Ability/Utility/BaseInputTriggerAbility.h"
#include "GA_Death.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTNL_API UGA_Death : public UBaseInputTriggerAbility
{
	GENERATED_BODY()
public:
	UGA_Death(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle
															, const FGameplayAbilityActorInfo* ActorInfo
															, const FGameplayAbilityActivationInfo
															ActivationInfo
															, const FGameplayEventData*
															TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle
													, const FGameplayAbilityActorInfo* ActorInfo
													, const FGameplayAbilityActivationInfo ActivationInfo
													, bool bReplicateEndAbility
													, bool bWasCancelled) override;

	TObjectPtr<class UPlayMontageWithEvent> PlayMontageWithEventTask;

	UFUNCTION()
	void OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData);
	
	UFUNCTION()
	void OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData);

	/** 서버에서 실행: 사망 처리 */
	UFUNCTION(Server, Reliable,WithValidation)
	void ServerHandleDeath(ABaseCharacter* Character);

	/** 클라이언트 동기화: Ragdoll, 충돌 해제 등 */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastHandleDeath(ABaseCharacter* Character);

};
