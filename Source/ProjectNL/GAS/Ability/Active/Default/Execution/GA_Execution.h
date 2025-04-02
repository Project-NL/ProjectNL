// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectNL/GAS/Ability/Utility/BaseInputTriggerAbility.h"
#include "GA_Execution.generated.h"

/**
 * 
 */

UCLASS()
class PROJECTNL_API UGA_Execution : public UBaseInputTriggerAbility
{
	GENERATED_BODY()
public:
	UGA_Execution(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	TObjectPtr<class UPlayMontageWithEvent> PlayMontageWithEventTask;

	UFUNCTION()
	void OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData);
	
	UFUNCTION()
	void OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData);

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	bool CanExecute(AActor* Target) const;
	void PerformExecution();
	void PlayExcutionAnimation(int8 arrNum,const TArray<UAnimMontage*> ArrExecutionAnimMontage,const ABaseCharacter* CurrentCharacter);
	void ApplyExecutionDamage(UAbilitySystemComponent* TargetASC);
	UFUNCTION()
	void OnPositionAdjusted();
	void EndExecution();
	void StartExecution();


	UPROPERTY(EditDefaultsOnly, Category = "GAS")
	TArray<TObjectPtr<UAnimMontage>> ArrPlayerExecutionAnimMontage;

	
	UPROPERTY(EditDefaultsOnly, Category = "GAS")
	TArray<TObjectPtr<UAnimMontage>> ArrEnemyExecutionAnimMontage;
	
	UPROPERTY(EditDefaultsOnly, Category="Ability|Effect"
		, meta=(AllowPrivateAccess = true))
	TSubclassOf<UGameplayEffect> ExecutionEffectClass;

		
	UPROPERTY(EditDefaultsOnly, Category="Ability|Effect")
	class AExecutionCameraActor* ExecutionCameraActor;

	

};
