// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectNL/GAS/NLAbilitySystemComponent.h"
#include "UObject/Interface.h"
#include "InteractionInterface.generated.h"

// 캐릭터 간의 상호 작용을 나타내는 인터페이스입니다.
UINTERFACE(MinimalAPI)
class UInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTNL_API IInteractionInterface
{
	GENERATED_BODY()


public:
	virtual void OnDamaged(const FDamagedResponse& DamagedResponse) = 0;
	virtual void OnDamagedMontageEnded(UAnimMontage* Montage, bool bInterrupted)=0;
	virtual void OnKnockback(const FDamagedResponse& DamagedResponse,float DamageMontageLength)=0;
};
