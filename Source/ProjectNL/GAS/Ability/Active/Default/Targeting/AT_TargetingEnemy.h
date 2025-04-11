// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "ProjectNL/GAS/Ability/Utility/PlayMontageWithEvent.h"
#include "ProjectNL/Helper/UtilHelper.h"
#include "AT_TargetingEnemy.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAbilityTaskCleared);
class USpringArmComponent;
UCLASS()
class PROJECTNL_API UAT_TargetingEnemy : public UAbilityTask
{
	GENERATED_BODY()


public:
	static UAT_TargetingEnemy* InitialEvent(UGameplayAbility* OwningAbility);
	
	virtual void Activate() override;
	
	

	GETTER_SETTER(int8,NearestEnemyCheck);

	virtual void TargetNearestEnemy();

	UPROPERTY()
	FOnAbilityTaskCleared OnCanceled;

protected:
	virtual void TickTask(float DeltaTime) override;
	UFUNCTION()
	void ReleaseLockOnTarget();
	UFUNCTION()
	AActor* FindNearestTarget() const;
	UFUNCTION()
	void CameraRotation(float DeltaTime);
	UFUNCTION()
	void LockOnTarget(AActor* NewTarget);
	// 스프링암 설정을 저장하는 함수
	UFUNCTION()
	void SaveSpringArmSettings(USpringArmComponent* SpringArm);

	// 스프링암 설정을 복원하는 함수
	UFUNCTION()
	void RestoreSpringArmSettings(USpringArmComponent* SpringArm);

private:
	UPROPERTY()
	TObjectPtr<AActor> CurrentTarget;
	
	UPROPERTY()
	TObjectPtr<AActor> NearestEnemy;
	
	UPROPERTY(EditAnywhere, Category = "Targeting")
	float TargetingRange = 1000.0f;

	
	float OriginalCharacterSpeed;

	int8 NearestEnemyCheck=false;
	
	// 저장된 스프링암 설정
	
	FRotator SavedRotation;
	FVector SavedLocation;
	float SavedTargetArmLength;
	int8 bSavedUsePawnControlRotation;
	int8 bSavedInheritPitch;
	int8 bSavedInheritYaw;
	int8 bSavedInheritRoll;
	int8 bSavedEnableCameraLag;
	

};
