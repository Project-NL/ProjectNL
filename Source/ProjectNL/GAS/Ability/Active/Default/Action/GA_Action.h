// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionAbilityData.h"
#include "ProjectNL/GAS/Ability/Utility/BaseInputTriggerAbility.h"
#include "GA_Action.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTNL_API UGA_Action : public UBaseInputTriggerAbility
{
	GENERATED_BODY()
public:
	UGA_Action(const FObjectInitializer& ObjectInitializer);

	GETTER(FActionAbilityData*,ActionAnimData);

	FActionSequenceData* GetActionSequenceData();
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle
															, const FGameplayAbilityActorInfo* ActorInfo
															, const FGameplayAbilityActivationInfo
															ActivationInfo
															, const FGameplayEventData*
															TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle
													, const FGameplayAbilityActorInfo* ActorInfo
													, const FGameplayAbilityActivationInfo ActivationInfo
													, bool bReplicateEndAbility
													, bool bWasCancelled) override;

	UFUNCTION()
	void OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData);
	
	UFUNCTION()
	void OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData);


	/*---------------------------------------------------------------------------
	 * 시퀀스 액션 
	 */
	
	UFUNCTION()
	void PlayActionAnimation(UAnimMontage* ActionAnimMontage);

	UFUNCTION()
	void StartActionSequence();

	// 다음 액션을 재생하는 함수
	UFUNCTION()
	void PlayNextAction();

	UFUNCTION()
	void MoveAction();//움직이는 액션
	UFUNCTION()
	void DelayAction(float DelayTime);//Delay를 하는 액션
	// 애니메이션이 끝났을 때(또는 중단되었을 때) 호출
	UFUNCTION()
	void OnAnimationCompleted();
	UFUNCTION()
	void OnMoveCompleted();
	UFUNCTION()
	void OnDelayCompleted();
	UFUNCTION()
	void SetActionDataByDataTable();

	


private:
	
	UPROPERTY()
	TObjectPtr<class UPlayMontageWithEvent> PlayMontageTask;
	// 현재 실행 중인 액션 인덱스
	UPROPERTY()
	int32 CurrentActionIndex;

	UPROPERTY()
	EActionType ActionSequenceIndex;
	//액션 입력 데이터 모음
	UPROPERTY()
	TArray<FActionAbilityData> InActionSequence;

	FActionAbilityData* ActionAnimData;

	FName SkillName;
	
public:
	GETTER_SETTER(FName,SkillName);

	UPROPERTY(Category="Ability|Data", EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataTable> ActionData;
	//애니메이션 추가하기


	
};
	

