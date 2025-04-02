// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Death.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "Components/CapsuleComponent.h"
#include "ProjectNL/Character/Player/PlayerCharacter.h"
#include "ProjectNL/Component/EquipComponent/EquipComponent.h"
#include "ProjectNL/GAS/Ability/Utility/PlayMontageWithEvent.h"
#include "ProjectNL/Weapon/BaseWeapon.h"

class AAIController;

UGA_Death::UGA_Death(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	
}

void UGA_Death::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ABaseCharacter* CurrentCharacter = Cast<ABaseCharacter>(GetAvatarActorFromActorInfo());
	if (!CurrentCharacter)
	{
		return;
	}
	UAnimMontage* DeathAnimMontage=CurrentCharacter->GetDeathMontage();
	if (!DeathAnimMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_Death DeathAnimMontage is nullptr in UGA_Death::ActivateAbility"));
		return;
	}

	// 몽타주 이름 로그 출력
	UE_LOG(LogTemp, Log, TEXT("UGA_Death Playing Death Montage: %s"), *DeathAnimMontage->GetName());
	// PlayMontageWithEventTask = UPlayMontageWithEvent::InitialEvent(this, NAME_None,
	// 															DeathAnimMontage
	// 															,FGameplayTagContainer());
	// PlayMontageWithEventTask->OnBlendOut.AddDynamic(this, &ThisClass::OnCompleted);
	// PlayMontageWithEventTask->OnCancelled.AddDynamic(this, &ThisClass::OnCancelled);
	// PlayMontageWithEventTask->OnInterrupted.AddDynamic(this, &ThisClass::OnCompleted);
	// PlayMontageWithEventTask->OnCompleted.AddDynamic(this, &ThisClass::OnCompleted);
	// PlayMontageWithEventTask->ReadyForActivation();

	// UPlayMontageWithEvent* Task = UPlayMontageWithEvent::InitialEvent(this,
	// 							NAME_None, DeathAnimMontage, FGameplayTagContainer());
	// Task->OnCompleted.AddDynamic(this, &ThisClass::OnCancelled);
	// Task->ReadyForActivation();
	FGameplayTag EventTag;
	FGameplayEventData EventData;
	OnCompleted(EventTag,EventData);
}

void UGA_Death::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	UE_LOG(LogTemp, Warning, TEXT("UGA_Death 멈춤!"));
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Death::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	UE_LOG(LogTemp, Warning, TEXT("UGA_Death OnCompleted 멈춤!"));

	ABaseCharacter* CurrentCharacter = Cast<ABaseCharacter>(GetAvatarActorFromActorInfo());
	if (CurrentCharacter)
	{
		if (CurrentCharacter->HasAuthority()) // 서버에서만 실행
		{
			
			ServerHandleDeath(CurrentCharacter);
		}
	}
	

	// 어빌리티 종료
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_Death::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

bool UGA_Death::ServerHandleDeath_Validate(ABaseCharacter* Character)
{
	return true;
}

void UGA_Death::ServerHandleDeath_Implementation(ABaseCharacter* Character)
{
	if (!Character) return;

	// 모든 클라이언트에 동기화
	MulticastHandleDeath(Character);
	APlayerCharacter* PlayerCharacter =Cast<APlayerCharacter>(Character);
	if (PlayerCharacter)
	{
		PlayerCharacter->SetTargetingCharacter(nullptr);
	}
	// 컨트롤러 제거 (서버에서만)
	AController* Controller = Character->GetController();
	
	// 일정 시간 후 제거
	ABaseWeapon* MainWeapon = Character->GetEquipComponent()->GetMainWeapon();
	MainWeapon->SetLifeSpan(5.0f);
	Character->SetLifeSpan(5.0f);

	
}

void UGA_Death::MulticastHandleDeath_Implementation(ABaseCharacter* Character)
{
	if (!Character) return;

	// Ragdoll 활성화
	USkeletalMeshComponent* Mesh = Character->GetMesh();
	if (Mesh)
	{
		Mesh->SetSimulatePhysics(true);
		Mesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	
	}
	APlayerCharacter* PlayerCharacter =Cast<APlayerCharacter>(Character);
	if (PlayerCharacter)
	{
		PlayerCharacter->SetTargetingCharacter(nullptr);
	}
	// 충돌 제거
	Character->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// AIController 로직 정지 (서버에서만)
	AAIController* AIController = Cast<AAIController>(Character->GetController());
	if (AIController && AIController->BrainComponent && Character->HasAuthority())
	{
		AIController->BrainComponent->StopLogic("Death");
	}
}
