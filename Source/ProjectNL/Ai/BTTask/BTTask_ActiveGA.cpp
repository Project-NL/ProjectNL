// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectNL/Ai/BTTask/BTTask_ActiveGA.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Actor.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Logging/LogMacros.h"
#include "ProjectNL/Character/BaseCharacter.h"
#include "ProjectNL/GAS/Ability/Active/Default/Action/GA_Action.h"


// 로그 카테고리 정의
DEFINE_LOG_CATEGORY_STATIC(LogPRBTTaskPlayAbility, Log, All);

UBTTask_ActiveGA::UBTTask_ActiveGA()
{
	bNotifyTick = true;  // Tick 함수를 활성화하여 작업 진행 중에도 업데이트할 수 있도록 설정
}

EBTNodeResult::Type UBTTask_ActiveGA::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// 기본 클래스 ExecuteTask 호출
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
	CachedOwnerComp = &OwnerComp;
	// AI 컨트롤러가 조종하는 포를 가져옴
	APawn* ControlledPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControlledPawn)
	{
		UE_LOG(LogPRBTTaskPlayAbility, Error, TEXT("Controlled Pawn is null."));
		return EBTNodeResult::Failed;
	}
	ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(ControlledPawn);
	// 조종하는 포에서 어빌리티 시스템 컴포넌트를 가져옴
	if(!BaseCharacter)
	{
		UE_LOG(LogPRBTTaskPlayAbility, Error, TEXT("BaseCharacter  is null."));
	}
		UAbilitySystemComponent* TargetASC = BaseCharacter->GetAbilitySystemComponent();
	
	if (!TargetASC)
	{
		UE_LOG(LogPRBTTaskPlayAbility, Error, TEXT("Ability System Component is null."));
		return EBTNodeResult::Failed;
	}

	// 어빌리티 클래스가 유효한지 확인
	if (!AbilityToActivate)
	{
		UE_LOG(LogPRBTTaskPlayAbility, Error, TEXT("Ability to activate is null."));
		return EBTNodeResult::Failed;
	}




	// 어빌리티를 실행 시도
	AbilitySpec = TargetASC->BuildAbilitySpecFromClass(AbilityToActivate, 1, INDEX_NONE);
	if(!AbilitySpec.Ability)
	{
		UE_LOG(LogPRBTTaskPlayAbility, Error, TEXT("AbilitySpec is null."));
		return EBTNodeResult::Failed;
	}

	TargetASC->GetActivatableAbilities();
	
	FGameplayAbilitySpecHandle AbilityHandle = TargetASC->GiveAbility(AbilitySpec);
	UGameplayAbility* ActivatedAbility = TargetASC->FindAbilitySpecFromHandle(AbilityHandle)->GetPrimaryInstance();
	if (ActivatedAbility)
	{
		// 이후 OnDestroy 등에서 종료를 위해 멤버 변수에 할당
		ActivatedAbilityInstance = ActivatedAbility;
		// 어빌리티 종료 시 호출될 델리게이트 바인딩
		ActivatedAbility->OnGameplayAbilityEnded.AddUObject(this, &UBTTask_ActiveGA::OnAbilityEndCallback);
		ActivatedAbility->OnGameplayAbilityCancelled.AddUObject(this, &UBTTask_ActiveGA::OnAbilityCancelCallback);
		UE_LOG(LogPRBTTaskPlayAbility, Log, TEXT("Successfully activated ability: %s"), *AbilityToActivate->GetName());
	}
	UGA_Action* ActionAbility = Cast<UGA_Action>(ActivatedAbility);
	if (ActionAbility)
	{
		ActionAbility->SetSkillName(SkillName);
	}
	
	bool bActivated = TargetASC->TryActivateAbility(AbilityHandle);
	if (!bActivated)
	{
		UE_LOG(LogPRBTTaskPlayAbility, Warning, TEXT("Failed to activate ability: %s"), *AbilityToActivate->GetName());
		return EBTNodeResult::Failed;
	}
	
	// 어빌리티가 성공적으로 활성화된 경우, 어빌리티가 끝날 때까지 대기
	UE_LOG(LogPRBTTaskPlayAbility, Log, TEXT("Successfully activated ability: %s"), *AbilityToActivate->GetName());

	return EBTNodeResult::InProgress;
}

void UBTTask_ActiveGA::OnAbilityCancelCallback()
{
	UE_LOG(LogTemp, Log, TEXT("BTTask_ActiveGA Cancelled"));

	FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Aborted);

}

void UBTTask_ActiveGA::OnAbilityEndCallback(UGameplayAbility* GameplayAbility)
{
	UE_LOG(LogTemp, Log, TEXT("BTTask_ActiveGA Finished"));
	
	FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);

}

void UBTTask_ActiveGA::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
}
EBTNodeResult::Type UBTTask_ActiveGA::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// 부모 호출: 내부에서 OnTaskFinished를 보통 호출해 줍니다.
	EBTNodeResult::Type AbortResult = Super::AbortTask(OwnerComp, NodeMemory);

	// 필요하면 별도의 로직 추가
	// 예: Ability 강제 종료, 로그, 기타 정리 등

	if (ActivatedAbilityInstance && ActivatedAbilityInstance->IsActive())
	{
		UE_LOG(LogPRBTTaskPlayAbility, Log, TEXT("어빌리티를 종료합니다"));

		ActivatedAbilityInstance->K2_CancelAbility();
	}
	ActivatedAbilityInstance = nullptr;
	return AbortResult; 
}