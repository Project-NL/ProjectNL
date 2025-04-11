// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Curves/CurveVector.h" // CurveVector 사용
#include "AT_MoveAlongCurve.generated.h"

/**
 * 
 */
// 이동 이벤트를 위한 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCurveMovementFinished);
UCLASS()
class PROJECTNL_API UAT_MoveAlongCurve : public UAbilityTask
{
	GENERATED_BODY()

public:
	// 태스크 생성 함수
	static UAT_MoveAlongCurve* InitialEvent(
		UGameplayAbility* OwningAbility,
		FName TaskInstanceName,
		UCurveVector* InCurve,
		float InDuration
	);

	// 커스텀 이벤트
	UPROPERTY(BlueprintAssignable)
	FOnCurveMovementFinished OnMovementFinished;

protected:
	// 태스크 주요 메서드
	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;

private:
	UPROPERTY()
	UCurveVector* Curve; // 커브 벡터

	float Duration;      // 총 이동 시간
	float TimeElapsed;   // 현재 경과 시간

	FVector StartLocation; // 이동 시작 위치
	
};
