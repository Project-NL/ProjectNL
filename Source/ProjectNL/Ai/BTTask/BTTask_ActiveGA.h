// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

#include "BTTask_ActiveGA.generated.h"

class UGameplayAbility;
/**
 * 
 */
UCLASS()
class PROJECTNL_API UBTTask_ActiveGA : public UBTTaskNode
{
	GENERATED_BODY()
public:
	

	UBTTask_ActiveGA();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	void OnAbilityCancelCallback();

	void OnAbilityEndCallback(UGameplayAbility* GameplayAbility);
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	/** 어빌리티 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	TSubclassOf<UGameplayAbility> AbilityToActivate;

private:
	UBehaviorTreeComponent* CachedOwnerComp;

	FGameplayAbilitySpec AbilitySpec;
	
};
