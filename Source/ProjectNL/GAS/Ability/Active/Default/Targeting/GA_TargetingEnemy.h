// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectNL/GAS/Ability/Utility/BaseInputTriggerAbility.h"
#include "GA_TargetingEnemy.generated.h"

class UAT_TargetingEnemy;
class UPlayMontageWithEvent;
/**
 * 
 */
UCLASS()
class PROJECTNL_API UGA_TargetingEnemy : public UBaseInputTriggerAbility
{
	GENERATED_BODY()
public:
	UGA_TargetingEnemy(const FObjectInitializer& ObjectInitializer);

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
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;


	UFUNCTION()
	void OnCancelled();
private:

	UPROPERTY()
	TObjectPtr<UAT_TargetingEnemy> TargetingEnemyTask;

};
	
