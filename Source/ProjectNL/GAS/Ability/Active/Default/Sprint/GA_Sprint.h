#pragma once

#include "CoreMinimal.h"
#include "ProjectNL/GAS/Ability/Utility/BaseInputTriggerAbility.h"
#include "GA_Sprint.generated.h"

UCLASS()
class PROJECTNL_API UGA_Sprint : public UBaseInputTriggerAbility
{
	GENERATED_BODY()

public:
	UGA_Sprint(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
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

	FActiveGameplayEffectHandle ActiveHandle;

	FDateTime ActiveTime;

	UFUNCTION()
	void EndEvade(FGameplayTag EventTag, FGameplayEventData EventData);
};