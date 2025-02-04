// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "ProjectNL/GAS/NLAbilitySystemComponent.h"
#include "AT_Knockback.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAbilityKnockbackTaskCleared);
/**
 * 
 */
UCLASS()
class PROJECTNL_API UAT_Knockback : public UAbilityTask
{
	GENERATED_BODY()


public:
	 static UAT_Knockback* InitialEvent(UGameplayAbility* OwningAbility, FDamagedResponse& DamageResponse,float DamageMontageLength);
	
	virtual void Activate() override;

	UPROPERTY()
	FOnAbilityKnockbackTaskCleared OnCanceled;
protected:
	virtual void TickTask(float DeltaTime) override;
	
	FDamagedResponse DamagedResponse;

	float KnockbackDuration;
	
	/** 이미 지난 시간 누적 */
	float ElapsedTime;

	
	/** 넉백 시작 위치 */
	FVector StartLocation;

	/** 넉백 목표 위치 */
	FVector TargetLocation;

	/** 넉백 진행 여부 */
	bool bKnockbackActive = false;
};
