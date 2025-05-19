// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectNL/GAS/Ability/Active/Default/Knockback/GA_Knockback.h"

#include "GameFramework/CharacterMovementComponent.h"
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
void UGA_Knockback::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    APawn* Pawn = Cast<APawn>(ActorInfo->AvatarActor.Get());
    APlayerController* PC = Pawn ? Cast<APlayerController>(Pawn->GetController()) : nullptr;

    if (PC && PC->IsLocalController())
    {
        // 1) Lock out ALL input
        PC->SetIgnoreMoveInput(true);
        PC->SetIgnoreLookInput(true);
        Pawn->DisableInput(PC);

        // 2) Disable the MovementComponent entirely
        if (ACharacter* C = Cast<ACharacter>(Pawn))
        {
            if (auto* MoveComp = C->GetCharacterMovement())
            {
                MoveComp->DisableMovement();
            }
        }
    }

    // … your existing gameplay‐effect & montage code …

    KnockbackTask = UAT_Knockback::InitialEvent(this, DamagedResponse, DamageMontageLength);
    if (KnockbackTask)
    {
        KnockbackTask->OnCanceled.AddDynamic(this, &UGA_Knockback::OnCancelled);
        KnockbackTask->ReadyForActivation();
    }
}

void UGA_Knockback::EndAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility,
    bool bWasCancelled)
{
    // … your existing cleanup …

    APawn* Pawn = Cast<APawn>(ActorInfo->AvatarActor.Get());
    APlayerController* PC = Pawn ? Cast<APlayerController>(Pawn->GetController()) : nullptr;

    if (PC && PC->IsLocalController())
    {
        // 1) Re-enable input
        PC->SetIgnoreMoveInput(false);
        PC->SetIgnoreLookInput(false);
        Pawn->EnableInput(PC);

        // 2) Restore movement
        if (ACharacter* Character = Cast<ACharacter>(Pawn))
        {
            if (auto* MoveComp = Character->GetCharacterMovement())
            {
                MoveComp->SetMovementMode(MOVE_Walking);
            }
        }
    }

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
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