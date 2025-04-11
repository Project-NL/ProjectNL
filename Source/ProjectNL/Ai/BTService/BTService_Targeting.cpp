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
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return;
	}

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return;
	}

	// AI가 제어하는 Pawn 가져오기
	APawn* ControlledPawn = AIController->GetPawn();
	if (!ControlledPawn)
	{
		return;
	}

	// Blackboard에서 NearestEnemy 값 가져오기
	AActor* NearestEnemy = Cast<AActor>(BlackboardComp->GetValueAsObject(BBKEY_NEAREST_ENEMY));
	if (!NearestEnemy)
	{
		return; // 타겟이 없으면 아무 것도 하지 않음
	}

	// NearestEnemy의 위치 가져오기
	FVector EnemyLocation = NearestEnemy->GetActorLocation();
	FVector PawnLocation = ControlledPawn->GetActorLocation();

	// 타겟 방향 계산
	FVector Direction = (EnemyLocation - PawnLocation).GetSafeNormal();
	FRotator TargetRotation = FRotationMatrix::MakeFromX(Direction).Rotator();

	// 현재 회전 가져오기
	FRotator CurrentRotation = ControlledPawn->GetActorRotation();

	// Pitch 고정: TargetRotation의 Pitch를 현재 회전의 Pitch로 대체
	TargetRotation.Pitch = CurrentRotation.Pitch;

	// Roll 고정: TargetRotation의 Roll을 현재 회전의 Roll로 대체
	TargetRotation.Roll = CurrentRotation.Roll;
	
	// 부드러운 회전 (보간)
	FRotator SmoothRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaSeconds, 5.0f);

	// AI Pawn 회전 적용
	ControlledPawn->SetActorRotation(SmoothRotation);




}
