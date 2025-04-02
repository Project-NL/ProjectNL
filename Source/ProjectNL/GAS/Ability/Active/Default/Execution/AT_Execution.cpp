// Fill out your copyright notice in the Description page of Project Settings.


#include "AT_Execution.h"

#include "BrainComponent.h"
#include "ExecutionCameraActor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "ProjectNL/Ai/Controller/EnemyAIController.h"
#include "ProjectNL/Character/Enemy/EnemyCharacter.h"
#include "ProjectNL/Character/Player/PlayerCharacter.h"

class AExecutionCameraActor;

UAT_Execution* UAT_Execution::InitialEvent(UGameplayAbility* owningAbility, APlayerCharacter* playerCharacter,
                                           AEnemyCharacter* targetEnemyCharacter, float fdistance)
{
	UAT_Execution* Task = NewAbilityTask<UAT_Execution>(owningAbility);
	Task->TargetEnemyCharacter = targetEnemyCharacter;
	Task->PlayerCharacter = playerCharacter;
	Task->Distance = fdistance;
	return Task;
}

void UAT_Execution::Activate()
{
	Super::Activate();

	if (!PlayerCharacter || !TargetEnemyCharacter)
	{
		FinishTask();
		return;
	}

	// 틱에서 위치와 회전을 보정
	DisableEnemyAI();
	bTickingTask = true;

}

void UAT_Execution::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);
	AdjustPositionAndRotation(DeltaTime);
}

void UAT_Execution::FinishTask()
{
	if (!bIsFinished)
	{
		bIsFinished = true;
		bTickingTask = false;
		OnCompleted.Broadcast();
		EndTask();
	}
}


void UAT_Execution::AdjustPositionAndRotation(float deltaTime)
{
	// 적 캐릭터 위치
	FVector EnemyLocation = TargetEnemyCharacter->GetActorLocation();
	FVector PlayerLocation = PlayerCharacter->GetActorLocation();

	// 적과 플레이어 사이의 방향 계산
	FVector DirectionToPlayer = (PlayerLocation - EnemyLocation).GetSafeNormal();

	// 중간 점 계산: 적에서 300cm 떨어진 위치
	float OffsetFromEnemy = 200.0f; // 3 유닛 = 300cm
	FVector DesiredLocation = EnemyLocation + (DirectionToPlayer * OffsetFromEnemy);

	// 플레이어 이동: CharacterMovementComponent 사용
	UCharacterMovementComponent* MovementComponent = PlayerCharacter->GetCharacterMovement();
	if (MovementComponent)
	{
		FVector DirectionToDesired = (DesiredLocation - PlayerLocation).GetSafeNormal();
		FVector MovementInput = DirectionToDesired * 600 * deltaTime;

		// 목표 지점에 도달했는지 확인 후 속도 조정
		float DistanceToDesired = FVector::Dist(PlayerLocation, DesiredLocation);
		if (DistanceToDesired > 5.0f)
		{
			MovementComponent->AddInputVector(DirectionToDesired); // 이동 입력 추가
		}
	}

	// 적 회전 보정 (플레이어를 바라보게)
	FVector DirectionToPlayerFromEnemy = (PlayerLocation - EnemyLocation).GetSafeNormal();
	FRotator TargetRotation = FRotationMatrix::MakeFromX(DirectionToPlayerFromEnemy).Rotator();
	TargetEnemyCharacter->SetActorRotation(FQuat::Slerp(TargetEnemyCharacter->GetActorRotation().Quaternion(),
	                                                    TargetRotation.Quaternion(), deltaTime * LerpSpeed));

	// 완료 조건
	if (FVector::Dist(PlayerLocation, DesiredLocation) < 5.0f &&
		TargetEnemyCharacter->GetActorRotation().Equals(TargetRotation, 5.0f))
	{
		UE_LOG(LogTemp, Log,
		       TEXT(
			       "UAT_Execution: Position and Rotation adjustment completed. Distance to Desired: %f, Rotation Delta: %f"
		       ),
		       FVector::Dist(PlayerLocation, DesiredLocation),
		       FMath::Abs(TargetEnemyCharacter->GetActorRotation().Yaw - TargetRotation.Yaw));
		FinishTask();
	}
}

void UAT_Execution::DisableEnemyAI()
{
	if (TargetEnemyCharacter)
	{
		AAIController* TargetAIController = Cast<AAIController>(TargetEnemyCharacter->GetController());
		if (TargetAIController && TargetAIController->BrainComponent)
		{
			TargetAIController->BrainComponent->StopLogic("Execution Started");
		}
	}
}
