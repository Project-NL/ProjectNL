// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AT_Execution.generated.h"

/**
 * 
 */

class APlayerCharacter;
class AEnemyCharacter;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAdjustPositionAndRotationDelegate);

UCLASS()
class PROJECTNL_API UAT_Execution : public UAbilityTask
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAT_Execution* InitialEvent(UGameplayAbility* OwningAbility, APlayerCharacter* playerCharacter, AEnemyCharacter* targetEnemyCharacter, float Distance);

	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;

	UPROPERTY(BlueprintAssignable)
	FAdjustPositionAndRotationDelegate OnCompleted;
protected:

	void AdjustPositionAndRotation(float deltaTime);
	void DisableEnemyAI();

private:
	APlayerCharacter* PlayerCharacter;
	AEnemyCharacter* TargetEnemyCharacter;
	float Distance;
	float LerpSpeed = 5.0f;
	bool bIsFinished = false;

	

	void FinishTask();


};
