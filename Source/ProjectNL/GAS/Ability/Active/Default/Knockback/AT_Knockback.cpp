// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectNL/GAS/Ability/Active/Default/Knockback/AT_Knockback.h"
#include "GameFramework/Actor.h"
#include "Abilities/GameplayAbility.h"
#include "ProjectNL/Helper/GameplayTagHelper.h"
#include "Kismet/KismetMathLibrary.h"

UAT_Knockback* UAT_Knockback::InitialEvent(UGameplayAbility* OwningAbility, FDamagedResponse& DamageResponse, float DamageMontageLength)
{
	UAT_Knockback* MyTask = NewAbilityTask<UAT_Knockback>(OwningAbility);
	MyTask->DamagedResponse = DamageResponse;
	MyTask->KnockbackDuration = DamageMontageLength * 0.4f;
	return MyTask;
}

void UAT_Knockback::Activate()
{
	Super::Activate();

	AActor* AvatarActor = GetAvatarActor();
	if (!AvatarActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("UAT_Knockback: AvatarActor 실패."));
		OnCanceled.Broadcast();
		EndTask();
		return;
	}

	AActor* SourceActor = DamagedResponse.SourceActor;
	if (!SourceActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("UAT_Knockback: SourceActor 실패."));
		OnCanceled.Broadcast();
		EndTask();
		return;
	}

	StartLocation = AvatarActor->GetActorLocation();
	FVector SourceLocation = SourceActor->GetActorLocation();
	FVector AvatarLocation = AvatarActor->GetActorLocation();
	FVector KnockbackDir = (AvatarLocation - SourceLocation).GetSafeNormal();

	// 기본 넉백 거리
	float KnockbackDistance = DamagedResponse.Damage;

	// 가드 상태 여부 확인
	bool bIsGuarding = AbilitySystemComponent->HasMatchingGameplayTag(NlGameplayTags::Status_GuardReady);
	bool bInGuardAngle = false;

	if (bIsGuarding)
	{
		FVector Forward = AvatarActor->GetActorForwardVector();
		FVector ToSource = (SourceActor->GetActorLocation() - AvatarActor->GetActorLocation()).GetSafeNormal();

		float Dot = FVector::DotProduct(Forward, ToSource);
		float AngleDeg = FMath::Acos(Dot) * (180.f / PI);

		if (AngleDeg <= 60.f)
		{
			bInGuardAngle = true;
			UE_LOG(LogTemp, Warning, TEXT("유효 가드: 각도 %f도"), AngleDeg);
		}
		else
		{
			
			TArray<FGameplayAbilitySpec> ActiveAbilities = AbilitySystemComponent->GetActivatableAbilities();
			for (FGameplayAbilitySpec AbilitySpec : ActiveAbilities)
			{
				if (AbilitySpec.Ability && AbilitySpec.Ability->AbilityTags.HasTagExact(NlGameplayTags::Status_GuardReady))
				{
					AbilitySystemComponent->CancelAbilityHandle(AbilitySpec.Handle);
				}
			}
			UE_LOG(LogTemp, Warning, TEXT("가드 각도 벗어남: %f도 → 일반 넉백"), AngleDeg);
		}
	}

	if (bIsGuarding && bInGuardAngle)
	{
		// 유효 가드: 넉백 거리 감소
		KnockbackDistance *= 0.3f;
	}

	TargetLocation = StartLocation + (KnockbackDir * KnockbackDistance);

	ElapsedTime = 0.f;
	bKnockbackActive = true;
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
		FVector NewLocation = FMath::Lerp(StartLocation, TargetLocation, Alpha);
		AvatarActor->SetActorLocation(NewLocation, true);
	}

	if (Alpha >= 1.f)
	{
		bKnockbackActive = false;
		OnCanceled.Broadcast();
		EndTask();
	}
}
