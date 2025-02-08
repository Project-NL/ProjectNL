// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectNL/GAS/Ability/Active/Default/Action/GA_Action.h"

#include "AT_MoveAlongCurve.h"
#include "ProjectNL/GAS/Ability/Active/Default/Action/ActionAbilityData.h"
#include "ProjectNL/GAS/Ability/Active/Default/Dodge/GA_Dodge.h"
#include "Abilities/Tasks/AbilityTask_MoveToLocation.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "ProjectNL/GAS/Ability/Utility/PlayMontageWithEvent.h"

struct FCombatAnimationData;

UGA_Action::UGA_Action(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
}

FActionSequenceData* UGA_Action::GetActionSequenceData()
{
	return &ActionAnimData->ArrActionSequenceDatas[CurrentActionIndex];
}

void UGA_Action::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                 const FGameplayAbilityActivationInfo ActivationInfo,
                                 const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	//PlayActionAnimation();

	SetActionDataByDataTable(); //데이터 테이블을 읽어오는 함수
	PlayNextAction();
}

void UGA_Action::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                            const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
                            bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Action::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	if (CurrentActionIndex < ActionAnimData->ArrActionSequenceDatas.Num())
	{
		OnAnimationCompleted(); //다음 액션으로 넘어갑니다.
	}
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true
		           , false);
	}
}

void UGA_Action::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true
	           , false);
}

void UGA_Action::StartActionSequence()
{
	CurrentActionIndex = 0;
	ActionSequenceIndex = EActionType::Animation;
	if (ActionAnimData->ArrActionSequenceDatas.Num() == 0)
	{
		//스킬별 액션이 없으면 바로 종료
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true
		           , false);
		return;
	}

	// 첫 액션 시작
	PlayNextAction();
}

void UGA_Action::PlayActionAnimation(UAnimMontage* ActionAnimMontage) //몽타주 실행 함수
{
	ABaseCharacter* CurrentCharacter = Cast<ABaseCharacter>(
		GetAvatarActorFromActorInfo());
	UAbilitySystemComponent* TargetASC = CurrentCharacter->GetAbilitySystemComponent();
	// 조건에 따라 애니메이션 실행
	if (!ActionAnimMontage)
	{
		return;
	}
	PlayMontageTask = UPlayMontageWithEvent::InitialEvent(this, NAME_None
	                                                      , ActionAnimMontage
	                                                      , FGameplayTagContainer());
	PlayMontageTask->OnCancelled.AddDynamic(this, &UGA_Action::OnCompleted);
	PlayMontageTask->OnInterrupted.AddDynamic(this, &UGA_Action::OnCancelled);
	PlayMontageTask->OnBlendOut.AddDynamic(this, &UGA_Action::OnCompleted);
	PlayMontageTask->OnCompleted.AddDynamic(this, &UGA_Action::OnCompleted);
	PlayMontageTask->ReadyForActivation();
}

void UGA_Action::PlayNextAction()
{
	//현재 액션 데이터테이블 배열 위치를 증가시킵니다.

	if (CurrentActionIndex >= ActionAnimData->ArrActionSequenceDatas.Num())
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true
		           , false);
		return;
	}
	switch (ActionSequenceIndex)
	{
	case EActionType::Animation:
		{
			ActionSequenceIndex = EActionType::Move; //다음은 이동으로 바꿈
			FString DelayMessage = FString::Printf(TEXT("CurrentActionIndex for %.2d seconds."), CurrentActionIndex);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, DelayMessage);
			if (ActionAnimData->ArrActionSequenceDatas[CurrentActionIndex].ArrActionAnim)
			{
				
				PlayActionAnimation(ActionAnimData->ArrActionSequenceDatas[CurrentActionIndex].ArrActionAnim);
				//액션 애니메이션을 진행합니다.
			}
			else
			{
				PlayNextAction();
			}
		}
		break;
	case EActionType::Move:
		{
			//다음은 딜레이로 바꿈
			ActionSequenceIndex = EActionType::Delay;
			if (ActionAnimData->ArrActionSequenceDatas[CurrentActionIndex].ArrMovementDirection !=
				EMovementDirection::NONE)
			{
				MoveAction(); //방향 이동	
			}
			else
			{
				PlayNextAction();
			}
			
		}
		break;
	case EActionType::Delay:
		{
			ActionSequenceIndex = EActionType::Animation;
			float DelayTime = ActionAnimData->ArrActionSequenceDatas[CurrentActionIndex].DelayTime;

			CurrentActionIndex++;
			if (DelayTime > 0)
			{
				DelayAction(DelayTime); //방향 이동	
			}
			else
			{
				PlayNextAction();
			}
			
		}
		break;
	}
}


void UGA_Action::MoveAction()
{
	UCurveVector* MyCurve = ActionAnimData->ArrActionSequenceDatas[CurrentActionIndex].CurveVector;
	UAT_MoveAlongCurve* MoveTask = UAT_MoveAlongCurve::InitialEvent(
		this,
		FName("MoveAlongCurve"),
		MyCurve,
		10.0f // 3초 동안 이동
	);
	if (MoveTask)
	{
		MoveTask->OnMovementFinished.AddDynamic(this, &ThisClass::OnMoveCompleted);
		MoveTask->ReadyForActivation();
	}
}

void UGA_Action::DelayAction(float DelayTime)
{
	// UAbilityTask_WaitDelay 포인터 변수 선언 및 초기화
	UAbilityTask_WaitDelay* WaitDelayTask = UAbilityTask_WaitDelay::WaitDelay(this, DelayTime);

	if (WaitDelayTask)
	{
		// 작업을 시작
		WaitDelayTask->OnFinish.AddDynamic(this, &UGA_Action::OnDelayCompleted);
		WaitDelayTask->ReadyForActivation();
	}
}

void UGA_Action::OnAnimationCompleted()
{
	PlayNextAction();
}

void UGA_Action::OnMoveCompleted()
{
	PlayNextAction();
}

void UGA_Action::OnDelayCompleted()
{
	PlayNextAction();
}

void UGA_Action::SetActionDataByDataTable()
{
	if (!ActionData)
	{
		UE_LOG(LogTemp, Error, TEXT("ActionData is nullptr! Make sure the DataTable is assigned."));
		return;
	}

	// DataTable에서 Row 검색
	FActionAbilityData* FoundData = ActionData->FindRow<FActionAbilityData>(SkillName, TEXT("Looking for Action Data"));

	if (!FoundData)
	{
		UE_LOG(LogTemp, Error, TEXT("No row found in DataTable for RowName: %s"), *SkillName.ToString());
		return;
	}

	// ActionAnimData에 값을 설정
	ActionAnimData = FoundData;

	UE_LOG(LogTemp, Log, TEXT("Successfully set ActionAnimData for RowName: %s"), *SkillName.ToString());
}
