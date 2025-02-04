// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectNL/GAS/Ability/Active/Default/Knockback/AT_Knockback.h"
#include "GameFramework/Actor.h"
#include "Abilities/GameplayAbility.h"

UAT_Knockback* UAT_Knockback::InitialEvent(UGameplayAbility* OwningAbility, FDamagedResponse& DamageResponse,float DamageMontageLength)
{
	// AbilityTask를 생성할 때 호출하는 정적 함수
	UAT_Knockback* MyTask = NewAbilityTask<UAT_Knockback>(OwningAbility);

	MyTask->DamagedResponse = DamageResponse;
	MyTask->KnockbackDuration=DamageMontageLength*0.4;

	return MyTask;
}
void UAT_Knockback::Activate()
{
	Super::Activate();

	// AvatarActor(일반적으로 Character나 Pawn)를 가져온다.
	AActor* AvatarActor = GetAvatarActor();
	if (!AvatarActor)
	{
		OnCanceled.Broadcast();
		EndTask();  // 에러 처리
		return;
	}

	// 공격자(혹은 소스) 액터
	AActor* SourceActor = DamagedResponse.SourceActor;
	if (!SourceActor)
	{
		OnCanceled.Broadcast();
		EndTask();  
		return;
	}

	// 넉백 시작 위치(피격자의 현재 위치)
	StartLocation = AvatarActor->GetActorLocation();

	// 소스 -> 대상 방향 벡터 구하기
	// (만약 반대로, 대상->소스 방향으로 밀어내고 싶다면 부호를 반대로 해주세요)
	FVector SourceLocation = SourceActor->GetActorLocation();
	FVector AvatarLocation = AvatarActor->GetActorLocation();

	// 방향 벡터: 소스에서 대상 쪽
	FVector KnockbackDir = (AvatarLocation - SourceLocation).GetSafeNormal();

	// 넉백 목표 지점 = 시작 위치 + (방향 벡터 × 거리)
	// 여기서는 예시로 DamagedResponse.Damage를 곱해 넉백 거리로 사용
	// (필요에 따라 별도의 KnockDistance 변수를 두어도 됩니다)
	TargetLocation = StartLocation + (KnockbackDir * DamagedResponse.Damage);

	// 넉백 타이머/시간 계산 등에 사용하기 위한 초기화
	ElapsedTime = 0.f;
	bKnockbackActive = true;

	// TickTask를 계속 호출할 수 있게끔 설정 (AbilityTask를 매 프레임 갱신)
	bTickingTask = true;
}

void UAT_Knockback::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	if (!bKnockbackActive)
	{
		return;
	}

	ElapsedTime += DeltaTime;
	float Alpha = FMath::Clamp(ElapsedTime / KnockbackDuration, 0.f, 1.f);

	if (AActor* AvatarActor = GetAvatarActor())
	{
		// 선형 보간 (Lerp)으로 현재 위치를 계산
		FVector NewLocation = FMath::Lerp(StartLocation, TargetLocation, Alpha);
		AvatarActor->SetActorLocation(NewLocation);
	}

	// 넉백이 끝났다면 Task 종료
	if (Alpha >= 1.f)
	{
		bKnockbackActive = false;
		EndTask();
		OnCanceled.Broadcast();
	}
}
