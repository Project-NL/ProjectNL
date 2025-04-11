// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectNL/Ai/Controller/EnemyAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/OverlapResult.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "ProjectNL/Ai/AiKey.h"
#include "ProjectNL/Character/BaseCharacter.h"
#include "ProjectNL/Character/Player/PlayerCharacter.h"


AEnemyAIController::AEnemyAIController()
{
	// AIPerceptionComponent 초기화
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
	SetPerceptionComponent(*AIPerceptionComponent);

	// SightConfig 초기화
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 300.0f;
	SightConfig->LoseSightRadius = 400.0f;
	SightConfig->PeripheralVisionAngleDegrees = 60.0f;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

	AIPerceptionComponent->ConfigureSense(*SightConfig);
	AIPerceptionComponent->SetDominantSense(*SightConfig->GetSenseImplementation());
	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnPerceptionUpdated);
	
}

void AEnemyAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);


// 	// NearestEnemy가 유효한지 확인
// 	if (NearestEnemy)
// 	{
// 		// 현재 컨트롤 중인 Pawn 가져오기
// D		APawn* ControlledPawn = GetPawn();
// 		if (ControlledPawn)
// 		{
// 			// Pawn과 NearestEnemy의 위치 가져오기
// 			FVector PawnLocation = ControlledPawn->GetActorLocation();
// 			FVector EnemyLocation = NearestEnemy->GetActorLocation();
//
// 			// Pawn에서 Enemy를 바라보는 방향 계산
// 			FVector Direction = EnemyLocation - PawnLocation;
// 			Direction.Z = 0.0f; // 수평 회전만 고려 (필요에 따라 조정)
//             
// 			// LookAt 회전값 계산
// 			FRotator TargetRotation = Direction.Rotation();
//
// 			// 현재 Pawn의 회전값을 부드럽게 TargetRotation으로 보간
// 			// 보간 속도(예: 5.0f)는 필요에 따라 조절하세요.
// 			FRotator NewRotation = FMath::RInterpTo(ControlledPawn->GetActorRotation(), TargetRotation, DeltaSeconds, 5.0f);
//
// 			// AI 컨트롤러의 회전 업데이트 (Pawn이 컨트롤러의 회전을 따르도록 설정되어 있어야 함)
// 			SetControlRotation(TargetRotation);
//             
// 			// 만약 Pawn 자체의 회전을 직접 적용하고 싶다면 아래와 같이 사용 가능:
// 			// ControlledPawn->SetActorRotation(NewRotation);
// 		}
// 	}
	//SearchForNearestEnemy();
}

void AEnemyAIController::RunAi()
{
	UBlackboardComponent* BlackBoardPtr =Blackboard.Get();
	if(UseBlackboard(BBAsset,BlackBoardPtr))
	{
		Blackboard->SetValueAsVector(BBKEY_HOMEPOS,GetPawn()->GetActorLocation()); 
		bool RunResult=RunBehaviorTree(BTAsset);
		ensure(RunResult);
	}
}

void AEnemyAIController::StopAi()
{
	UBehaviorTreeComponent* BTComponent=Cast<UBehaviorTreeComponent>(BrainComponent);
	if(BTComponent)
	{
		BTComponent->StopTree();
	}
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	RunAi();
}
void AEnemyAIController::SearchForNearestEnemy()
{
	APawn* MyPawn = GetPawn();
	if (MyPawn == nullptr)
	{
		return;
	}

	const float SearchRadius = 1000.0f; // 탐색 범위를 설정합니다 (예: 1000 유닛)
	const FVector SearchOrigin = MyPawn->GetActorLocation();
	const FVector BoxExtent = FVector(SearchRadius, SearchRadius, SearchRadius);

	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(MyPawn);

	// 사각형 충돌 박스를 생성하여 충돌을 체크합니다.
	GetWorld()->OverlapMultiByObjectType(
		OverlapResults,
		SearchOrigin,
		FQuat::Identity,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_Pawn),
		FCollisionShape::MakeBox(BoxExtent),
		QueryParams
	);


	float MinDistance = FLT_MAX;
	for (const FOverlapResult& Result : OverlapResults)
	{
		AActor* Actor = Result.GetActor();
		if (Actor && Actor->IsA(APlayerCharacter::StaticClass()))
		{
			float Distance = FVector::Dist(Actor->GetActorLocation(), MyPawn->GetActorLocation());
			if (Distance < MinDistance)
			{
				MinDistance = Distance;
				NearestEnemy = Cast<APlayerCharacter>(Actor);
			}
		}
	}

	if (NearestEnemy)
	{
		Blackboard->SetValueAsObject(BBKEY_NEAREST_ENEMY, NearestEnemy);
	}
	else
	{
		Blackboard->ClearValue(BBKEY_NEAREST_ENEMY); // 범위 내에 적이 없으면 블랙보드 값 초기화
	}

	// 디버그용 박스 그리기 (필요시 주석 처리)
	DrawDebugBox(GetWorld(), SearchOrigin, BoxExtent, FColor::Green, false, 0.1f, 0, 5.0f);
}

void AEnemyAIController::OnPerceptionUpdated(AActor* UpdatedActor, FAIStimulus Stimulus)
{
	if (Stimulus.IsActive())
	{
		UpdateClosestEnemy();
	}
}

void AEnemyAIController::UpdateClosestEnemy()
{
	TArray<AActor*> PerceivedActors;
	AIPerceptionComponent->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), PerceivedActors);

	APawn* MyPawn = GetPawn();
	if (!MyPawn)
	{
		return;
	}

	
	float MinDistance = FLT_MAX;

	for (AActor* Actor : PerceivedActors)
	{
		if (Actor->IsA(APlayerCharacter::StaticClass()))
		{
			float Distance = FVector::Dist(Actor->GetActorLocation(), MyPawn->GetActorLocation());
			if (Distance < MinDistance)
			{
				MinDistance = Distance;
				NearestEnemy = Cast<APlayerCharacter>(Actor);
			}
		}
	}

	if (NearestEnemy)
	{
		GetBlackboardComponent()->SetValueAsObject(BBKEY_NEAREST_ENEMY, NearestEnemy);
	}
	else
	{
		GetBlackboardComponent()->ClearValue(BBKEY_NEAREST_ENEMY);
	}
}
