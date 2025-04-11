// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AT_Knockback.h"
#include "ProjectNL/GAS/Ability/Utility/BaseInputTriggerAbility.h"
#include "GA_Knockback.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayMontageEventNotified
																						, FGameplayTag, EventTag
																						, FGameplayEventData, EventData);
UCLASS()
class PROJECTNL_API UGA_Knockback : public UBaseInputTriggerAbility
{
	GENERATED_BODY()
public:
	UGA_Knockback(const FObjectInitializer& ObjectInitializer);


	void SetDamageResponse(const FDamagedResponse& DamageResponse);
	void SetDamageMontage(UAnimMontage* DamageMontage);
	SETTER(float,DamageMontageLength);

	FOnPlayMontageEventNotified OnPlayMontageWithEventDelegate;
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
	
	UFUNCTION()
	void OnCancelled();
	void OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData);

private:

	UPROPERTY()
	TObjectPtr<UAT_Knockback> KnockbackTask;
	
	UPROPERTY(Replicated)
	FDamagedResponse DamagedResponse;

	UPROPERTY(Replicated)
	float DamageMontageLength;

	UPROPERTY(Replicated)
	TObjectPtr<UAnimMontage> DamagedMontage;
	

};
