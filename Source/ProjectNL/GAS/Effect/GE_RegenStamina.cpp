// Fill out your copyright notice in the Description page of Project Settings.


#include "GE_RegenStamina.h"

#include "ProjectNL/Character/Player/PlayerCharacter.h"
#include "ProjectNL/GAS/Attribute/PlayerAttributeSet.h"
#include "ProjectNL/Helper/GameplayTagHelper.h"

UGE_RegenStamina::UGE_RegenStamina()
{
}

void UGE_RegenStamina::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(TargetASC->GetAvatarActor());


	if (TargetASC)
	{
		if (TargetASC->HasMatchingGameplayTag(NlGameplayTags::State_Attack_Combo))
		{
			return;
		}
		UPlayerAttributeSet* Attributes = PlayerCharacter->PlayerAttributeSet;
		if (Attributes && Attributes->Stamina.GetCurrentValue() < Attributes->MaxStamina.GetBaseValue())  // 80 이상이면 회복 X
		{
			OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(Attributes->GetStaminaAttribute(), EGameplayModOp::Additive, 1.0f));
		}
	}
}
