// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectNL/Component/SkillComponent/TimeRecallComponent.h"

#include "GhostTrailPoolManager.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "ProjectNL/GAS/Ability/Active/Default/Skill/GhostTrail.h"

UTimeRecallComponent::UTimeRecallComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicated(true);
	RecordInterval = 0.05f; // 예: 0.05초마다 기록
	MaxRecordTime = 5.0f; // 예: 최대 5초간 기록
	TimeAccumulator = 0.0f;
	bIsRecalling = false;
	RecallTimeRemaining = 5.0f;
	RecordTime=0.2f;
}

void UTimeRecallComponent::BeginPlay()
{
	Super::BeginPlay();

	// MaxRecordTime 동안 RecordInterval마다 하나씩 Transform을 기록할 수 있는 최대 개수 계산
	MaxTransformsCount = FMath::FloorToInt(MaxRecordTime / RecordInterval);
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	GhostTrailPoolManager = GetWorld()->SpawnActor<AGhostTrailPoolManager>(
		AGhostTrailPoolManager::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	if (GhostTrailPoolManager)
	{
		GhostTrailPoolManager->InitializePool(GetWorld(), GhostTrailActor, 50); // 초기 풀 사이즈 10개
	}
}

void UTimeRecallComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// bIsRecalling 변수를 복제 대기열에 등록
	DOREPLIFETIME(UTimeRecallComponent, bIsRecalling);
}

void UTimeRecallComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                         FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsRecalling)
	{
		PerformRecall(DeltaTime);
	}

	// 평소에는 Transform 기록
	TimeAccumulator += DeltaTime;
	if (TimeAccumulator >= RecordInterval)
	{
		TimeAccumulator = 0.0f;
		if (AActor* Owner = GetOwner())
		{
			FTransform CurrentTransform = Owner->GetActorTransform();

			StoredTransforms.Add(CurrentTransform);

			// 최대 기록 개수를 초과하면 가장 오래된 자료 제거
			if (StoredTransforms.Num() > MaxTransformsCount)
			{
				StoredTransforms.RemoveAt(0);
			}
		}
	}
}

void UTimeRecallComponent::StartRecall(float RecallDuration)
{
	if (StoredTransforms.Num() == 0)
	{
		// 기록된 Transform이 없으면 역행할 수 없음
		return;
	}
	bIsRecalling = true;
	RecallTimeRemaining = RecallDuration;
}

void UTimeRecallComponent::PerformRecall(float DeltaTime)
{
	
	if (RecallTimeRemaining <= 0.0f || StoredTransforms.Num() == 0)
	{
		// 역행 종료 조건
		bIsRecalling = false;
		return;
	}
	
	// 마지막 Transform 가져와 적용
	FTransform LastTransform = StoredTransforms.Last();
	if (AActor* Owner = GetOwner())
	{
		Owner->SetActorTransform(LastTransform);
		APawn* Pawn = Cast<APawn>(Owner);
		if (Pawn && Pawn->GetController())
		{
			FRotator NewRotation = LastTransform.GetRotation().Rotator();
			Pawn->GetController()->SetControlRotation(NewRotation);
		}
		RecordTime+=DeltaTime;//잔상을 몇 초마다 남길지
		if (GhostTrailPoolManager&&RecordTime>0.2)
		{
			RecordTime=0;
			FTransform OwnerTransform = GetOwner()->GetActorTransform();
			AGhostTrail* SpawnedGhostTrail = GhostTrailPoolManager->GetPooledGhostTrail();
			if (SpawnedGhostTrail)
			{
				// 현재 Owner의 Transform을 가져옴
				FTransform GhostTransform = OwnerTransform;

				// 1. 위치(트랜슬레이션) 아래로 -85
				GhostTransform.AddToTranslation(FVector(0.f, 0.f, -85.f));

				// 2. 좌우로 -90도 회전
				//    보통 “좌우로”라는 의미는 Yaw(수평 회전)를 가정합니다.
				FRotator Rotator = GhostTransform.GetRotation().Rotator();
				Rotator.Yaw += -90.f;
				GhostTransform.SetRotation(Rotator.Quaternion());

				// 수정한 Transform 적용
				SpawnedGhostTrail->SetActorTransform(GhostTransform);

				ACharacter* CharacterOwner = Cast<ACharacter>(Owner);
				if (CharacterOwner)
				{
					USkeletalMeshComponent* PawnMesh = CharacterOwner->GetMesh();
					if (PawnMesh)
					{
						SpawnedGhostTrail->Init(PawnMesh);
					}
				}
			}
		}
	}

	StoredTransforms.RemoveAt(StoredTransforms.Num() - 1);

	RecallTimeRemaining -= DeltaTime;
	if (RecallTimeRemaining <= 0.0f)
	{
		bIsRecalling = false;
	}
}

void UTimeRecallComponent::OnRep_BIsRecalling()
{
	if (bIsRecalling)
	{
		RecallTimeRemaining = 5.0f;
	}
	else if (!bIsRecalling)
	{
		RecallTimeRemaining = 0.0f;
	}
}
