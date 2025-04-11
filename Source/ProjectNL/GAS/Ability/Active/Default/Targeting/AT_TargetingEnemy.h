// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "ProjectNL/GAS/Ability/Utility/PlayMontageWithEvent.h"
#include "ProjectNL/Helper/UtilHelper.h"
#include "AT_TargetingEnemy.generated.h"

class UPlayerCameraComponent;
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
	static UAT_TargetingEnemy* InitialEvent(UGameplayAbility* OwningAbility,TSubclassOf<UGameplayEffect> TargetingSpeedEffect);
	
	virtual void Activate() override;
	
	

	

	virtual void TargetNearestEnemy();

	UPROPERTY()
	FOnAbilityTaskCleared OnCanceled;

protected:
	virtual void TickTask(float DeltaTime) override;
	void TargetingNearestEnemy(float DeltaTime);
	UFUNCTION()
	void ReleaseLockOnTarget();
	UFUNCTION()
	AActor* FindNearestTarget() const;
	UFUNCTION()
	void CameraRotation(float DeltaTime);

	UFUNCTION()
	bool BacktoSquareOne(float DeltaTime);
	UFUNCTION()
	void PlayerContollerRotation(float DeltaTime);
	UFUNCTION()
	void LockOnTarget(AActor* NewTarget,float DeltaTime);
	// 스프링암 설정을 저장하는 함수
	UFUNCTION()
	void SaveSpringArmSettings(USpringArmComponent* SpringArm);
	UFUNCTION()
	void SaveCameraSettings(UPlayerCameraComponent* CameraComponent);
	UFUNCTION()
	void RestoreCameraRotation(UPlayerCameraComponent* CameraComponent,float DeltaTime);

	UFUNCTION()
	void RestoreSpringArmRotation(USpringArmComponent* SpringArm,float DeltaTime);
	bool IsActorOnScreen(AActor* Actor) const;
	bool HasLineOfSight(AActor* Actor) const;
	void CheckNearestEnemyDistance(float MaxDistance);

private:
	UPROPERTY()
	TObjectPtr<AActor> CurrentTarget;
	
	UPROPERTY()
	TObjectPtr<AActor> NearestEnemy;
	
	UPROPERTY(EditAnywhere, Category = "Targeting")
	float TargetingRange = 1000.0f;

	
	
	
	
	// 저장된 스프링암 설정
	
	FRotator SavedRotation;
	FVector SavedLocation;
	float SavedTargetArmLength;
	int8 bSavedUsePawnControlRotation;
	int8 bSavedInheritPitch;
	int8 bSavedInheritYaw;
	int8 bSavedInheritRoll;
	int8 bSavedEnableCameraLag;
	

	FVector SavedCameraWorldLocation;
	FVector SavedTargetActorWorldLocation;
	FVector SavedOwnerActorWorldLocation;
	FRotator SavedSpringArmWorldRotation;
	FRotator SavedCameraWorldRotation;
	FRotator SavedLockonActorRotation;
	


	TSubclassOf<UGameplayEffect> TargetingSpeedEffect;
	FActiveGameplayEffectHandle ActiveHandle;
	
};

