// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectNL/GAS/Ability/Utility/BaseInputTriggerAbility.h"
#include "GA_DrinkPotion.generated.h"

class APotionItem;
/**
 * 
 */
UCLASS()
class PROJECTNL_API UGA_DrinkPotion : public UBaseInputTriggerAbility
{
	GENERATED_BODY()
public:
	UGA_DrinkPotion(const FObjectInitializer& ObjectInitializer);

	void SetPotionItem(APotionItem* PotionItem);
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
	void OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData);
	
	UFUNCTION()
	void OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void PlayActionAnimation();
	
	TObjectPtr<class UPlayMontageWithEvent> PlayMontageWithEventTask;

private:

	UPROPERTY()
	APotionItem* PotionItem;
	
	UPROPERTY(EditDefaultsOnly, Category = "GAS")
	TObjectPtr<UAnimMontage> ActionAnimMontage;
	
	UPROPERTY(EditDefaultsOnly, Category="Ability|Effect"
		, meta=(AllowPrivateAccess = true))
	TSubclassOf<UGameplayEffect> HealEffect;
};
