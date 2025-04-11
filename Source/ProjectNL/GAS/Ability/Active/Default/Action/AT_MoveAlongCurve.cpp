// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectNL/GAS/Ability/Active/Default/Action/AT_MoveAlongCurve.h"

UAT_MoveAlongCurve* UAT_MoveAlongCurve::InitialEvent(UGameplayAbility* OwningAbility, FName TaskInstanceName,
	UCurveVector* InCurve, float InDuration)
{
	UAT_MoveAlongCurve* MyTask = NewAbilityTask<UAT_MoveAlongCurve>(OwningAbility, TaskInstanceName);
	MyTask->Curve = InCurve;
	MyTask->Duration = InDuration;
	MyTask->TimeElapsed = 0.0f;
	return MyTask;
}

void UAT_MoveAlongCurve::Activate()
{
	Super::Activate();
	
	AActor* MyActor = GetAvatarActor();
	if (!IsValid(MyActor) || !Curve || Duration <= 0.0f)
	{
		EndTask();
		return;
	}
    bTickingTask = true;
	StartLocation = MyActor->GetActorLocation(); // 시작 위치 저장
}

void UAT_MoveAlongCurve::TickTask(float DeltaTime)
{
	 Super::TickTask(DeltaTime);
    
        if (!Curve)
        {
            EndTask();
            return;
        }
    
        TimeElapsed += DeltaTime;
    
        if (TimeElapsed > Duration)
        {
            OnMovementFinished.Broadcast();
            EndTask();
            return;
        }
    
        // 시간에 따른 커브 값 계산
        float CurveTime = TimeElapsed / Duration; // 0.0 ~ 1.0
        FVector CurveValue = Curve->GetVectorValue(CurveTime);
    
        // 위치 업데이트
        AActor* MyActor = GetAvatarActor();
        if (IsValid(MyActor))
        {
            FVector NewLocation = StartLocation + CurveValue;
            MyActor->SetActorLocation(NewLocation); // 충돌 감지 포함
        }
}
