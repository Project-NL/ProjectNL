// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectNL/GAS/Ability/Active/Default/Knockback/GA_Knockback.h"
#include "Net/UnrealNetwork.h"
#include "ProjectNL/GAS/Ability/Active/Default/Knockback/AT_Knockback.h"
#include "ProjectNL/GAS/Ability/Utility/PlayMontageWithEvent.h"


UGA_Knockback::UGA_Knockback(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
	// 거리는 300, 시간은 0.5초 정도로 예시
	
}

void UGA_Knockback::SetDamageResponse(const FDamagedResponse& DamageResponse)
{
	DamagedResponse=DamageResponse;
}

void UGA_Knockback::SetDamageMontage(UAnimMontage* DamageMontage)
{
	DamagedMontage=DamageMontage;
}

void UGA_Knockback::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// 여기서 다시 입력 활성화
	APawn* Pawn = Cast<APawn>(ActorInfo->AvatarActor.Get());
	APlayerController* PC = Cast<APlayerController>(Pawn->GetController());
	if (PC)
	{
		PC->SetIgnoreMoveInput(false);  // 키보드 이동 무시
	}
	if (DamagedResponse.DamageEffect)
	{
		// 이펙트 컨텍스트를 생성
		FGameplayEffectContextHandle EffectContext = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
        
		// 이 이펙트의 소스 오브젝트 설정 (보통 '소유자(Owner)'를 설정)
		EffectContext.AddSourceObject(ActorInfo->AvatarActor.Get());
				
		GetAbilitySystemComponentFromActorInfo()->BP_ApplyGameplayEffectToSelf(DamagedResponse.DamageEffect,
		1.0f,    
		EffectContext);
				
	}
	
	if (DamagedMontage)//데미지 몽타주가 있을 때 
	{
		UPlayMontageWithEvent* Task = UPlayMontageWithEvent::InitialEvent(this,
										NAME_None, DamagedMontage, FGameplayTagContainer());
		Task->OnCompleted.AddDynamic(this, &ThisClass::OnCancelled);
		Task->ReadyForActivation();
	}
	KnockbackTask = UAT_Knockback::InitialEvent(this,DamagedResponse,DamageMontageLength);
	if (KnockbackTask)
	{
		// Task 실행
		KnockbackTask->OnCanceled.AddDynamic(this, &UGA_Knockback::OnCancelled);
		KnockbackTask->ReadyForActivation();
	 }


	
}

void UGA_Knockback::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	UE_LOG(LogTemp, Warning, TEXT("UGA_Knockback EndAbility 성공."));
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	OnPlayMontageWithEventDelegate.Broadcast(FGameplayTag(), FGameplayEventData());
	
	GetAbilitySystemComponentFromActorInfo()->
			RemoveActiveGameplayEffectBySourceEffect(DamagedResponse.DamageEffect, GetAbilitySystemComponentFromActorInfo());
	
	APawn* Pawn = Cast<APawn>(ActorInfo->AvatarActor.Get());
	if (Pawn)
	{
		// 컨트롤러를 얻어서 입력 무시(false) 처리하면, 플레이어의 이동 입력이 다시 활성화됩니다.
		APlayerController* PC = Cast<APlayerController>(Pawn->GetController());
		if (PC)
		{
			PC->SetIgnoreMoveInput(false);  // 키보드 이동 입력 재활성화
		}
	}
}


void UGA_Knockback::OnCancelled()
{
	UE_LOG(LogTemp, Warning, TEXT("UGA_Knockback  OnCancelled ActivateAbility EndAbility 성공."));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);

}
void UGA_Knockback::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo,
		true, false);
}

void UGA_Knockback::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UGA_Knockback, DamagedResponse);
	DOREPLIFETIME(UGA_Knockback, DamageMontageLength);
	DOREPLIFETIME(UGA_Knockback, DamagedMontage);
}