// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectNL/Ai/BTService/BTService_Targeting.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "ProjectNL/Ai/AiKey.h"

UBTService_Targeting::UBTService_Targeting()
{
	Interval = 0.1f; // 0.1초마다 호출
	RandomDeviation = 0.0f; // 무작위 간격 없음
}


void UBTService_Targeting::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{   Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// AI Controller 가져오기
	AAIController* OwnerController = OwnerComp.GetAIOwner();
	if (!OwnerController)
	{
		return;
	}

	// 블랙보드에서 TargetActorBBKey에 해당하는 오브젝트를 가져옴
	UObject* TargetObject = OwnerController->GetBlackboardComponent()->GetValueAsObject(BBKEY_NEAREST_ENEMY);
	if (!TargetObject)
	{
		return;
	}

	// 가져온 오브젝트를 AActor로 캐스팅
	AActor* TargetActor = Cast<AActor>(TargetObject);
	if (TargetActor)
	{
		// AIController가 TargetActor에 포커스를 맞추게 함
		OwnerController->SetFocus(TargetActor);

		// 블루프린트의 Print String 대신 로그 출력
		UE_LOG(LogTemp, Warning, TEXT("FocusTarget is %s"), *TargetActor->GetName());
	}


}

