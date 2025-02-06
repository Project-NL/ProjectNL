#pragma once

#include "CoreMinimal.h"
#include "ProjectNL/GAS/Ability/Utility/BaseInputTriggerAbility.h"
#include "GA_Sprint.generated.h"
class 	UAbilityTask_WaitDelay;
UCLASS()
class PROJECTNL_API UGA_Sprint : public UBaseInputTriggerAbility
{
	GENERATED_BODY()

public:
	UGA_Sprint(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	UFUNCTION()
	void OnWaitTimeFinished();

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle
	                                , const FGameplayAbilityActorInfo* ActorInfo
	                                , const FGameplayTagContainer* SourceTags
	                                , const FGameplayTagContainer* TargetTags
	                                , FGameplayTagContainer* OptionalRelevantTags)
	const override;
	
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle
														, const FGameplayAbilityActorInfo* ActorInfo
														, const FGameplayAbilityActivationInfo
														ActivationInfo
														, bool bReplicateCancelAbility) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	
private:
	UPROPERTY(EditDefaultsOnly,Category="Ability|Effect"
		, meta=(AllowPrivateAccess = true))
	float HoldExistTime;
	
	UPROPERTY(EditDefaultsOnly, Category="Ability|Effect"
		, meta=(AllowPrivateAccess = true))
	TSubclassOf<UGameplayEffect> BuffEffect;
	//구르기 태그를 부여하는 이펙트입니다.
	UPROPERTY(EditDefaultsOnly, Category="Ability|Effect"
		, meta=(AllowPrivateAccess = true))
	TSubclassOf<UGameplayEffect> RollEffect;
	
	FActiveGameplayEffectHandle ActiveHandle;

	FDateTime ActiveTime;
	
	UAbilityTask_WaitDelay* WaitDelayTask;

	UFUNCTION()
	void EndEvade(FGameplayTag EventTag, FGameplayEventData EventData);
};