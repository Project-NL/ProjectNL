#pragma once

#include "CoreMinimal.h"
#include "ProjectNL/GAS/Ability/Utility/BaseInputTriggerAbility.h"
#include "GA_Guard.generated.h"

class UBlockStartNotify;
class UAT_PutDamageWithEvent;
class UPlayMontageWithEvent;

UCLASS()
class PROJECTNL_API UGA_Guard : public UBaseInputTriggerAbility
{
	GENERATED_BODY()
	
public:
	UGA_Guard(const FObjectInitializer& ObjectInitializer);
	
protected:
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	void CancelAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                   FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility);

private:
	TObjectPtr<UPlayMontageWithEvent> AnimationTask;

	TObjectPtr<UBlockStartNotify> BlockStartNotify;

	UFUNCTION()
	void StartBlock(const FDamagedResponse& DamagedResponse);

	UFUNCTION()
	void EndBlock(FGameplayTag EventTag, FGameplayEventData EventData);
};
