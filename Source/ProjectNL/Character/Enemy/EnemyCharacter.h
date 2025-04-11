// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectNL/Character/BaseCharacter.h"

#include "EnemyCharacter.generated.h"

class UBaseAttributeSet;

/**
 * 
 */
UCLASS()
class PROJECTNL_API AEnemyCharacter : public ABaseCharacter
{
	GENERATED_BODY()
public:
	AEnemyCharacter();UPROPERTY()
	TObjectPtr<UBaseAttributeSet> EnemyAttributeSet;

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnDamaged(const FDamagedResponse& DamagedResponse);
};
