// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GE_RegenStamina.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTNL_API UGE_RegenStamina : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UGE_RegenStamina();

protected:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, 
										FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
