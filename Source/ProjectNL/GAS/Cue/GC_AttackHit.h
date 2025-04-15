// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "NiagaraSystem.h"
#include "GC_AttackHit.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTNL_API UGC_AttackHit : public UGameplayCueNotify_Static
{
	GENERATED_BODY()
public:
	UGC_AttackHit();

	virtual bool OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const override;

protected:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Cue")
	TObjectPtr<UNiagaraSystem> NiagaraSystem; // 변경된 파티클 타입
};
