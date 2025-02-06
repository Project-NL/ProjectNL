#include "GA_Sprint.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "ProjectNL/Component/EquipComponent/EquipComponent.h"
#include "ProjectNL/GAS/Ability/Utility/PlayMontageWithEvent.h"
#include "ProjectNL/Helper/EnumHelper.h"
#include "ProjectNL/Helper/LocateHelper.h"
#include "ProjectNL/Helper/StateHelper.h"

UGA_Sprint::UGA_Sprint(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	HoldExistTime = 1;
}

bool UGA_Sprint::CanActivateAbility(const FGameplayAbilitySpecHandle Handle
																, const FGameplayAbilityActorInfo* ActorInfo
																, const FGameplayTagContainer* SourceTags
																, const FGameplayTagContainer* TargetTags
																, FGameplayTagContainer* OptionalRelevantTags)
const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags
																, OptionalRelevantTags))
	{
		return false;
	}
	
	return true;
}

void UGA_Sprint::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ActiveTime = FDateTime::Now();

	UPlayMontageWithEvent* Task = UPlayMontageWithEvent::InitialEvent(this,
								NAME_None, GetCurrentMontage(), FGameplayTagContainer());
	Task->OnCompleted.AddDynamic(this, &ThisClass::EndEvade);
	Task->ReadyForActivation();
	WaitDelayTask=UAbilityTask_WaitDelay ::WaitDelay(this,HoldExistTime);
	WaitDelayTask->OnFinish.AddDynamic(this, &UGA_Sprint::OnWaitTimeFinished);
	WaitDelayTask->ReadyForActivation();

}
void UGA_Sprint::OnWaitTimeFinished()
{
	//아직 끝나지 않은 상태라면, HoldExistTime만큼 누르고 있었다고 판단 -> 버프 효과 적용
	if (BuffEffect)
	{
		FGameplayEffectContextHandle EffectContext = GetAbilitySystemComponentFromActorInfo()->
			MakeEffectContext();
		EffectContext.AddSourceObject(GetAvatarActorFromActorInfo());
	
		FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(
			BuffEffect, 1.0f, EffectContext);
		
		if (SpecHandle.IsValid())
		{
			ActiveHandle = GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(
				*SpecHandle.Data.Get());
		}
	}
}
void UGA_Sprint::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	
	if (BuffEffect)
	{
		GetAbilitySystemComponentFromActorInfo()->RemoveActiveGameplayEffect(ActiveHandle);
	}

	const FTimespan HoldDuration = FDateTime::Now() - ActiveTime;

	if (HoldDuration.GetTotalSeconds() <= HoldExistTime)
	{
		if (ABaseCharacter* OwnerCharacter = Cast<ABaseCharacter>(ActorInfo->AvatarActor.Get()))
		{
			const EMovementDirection CurrentDirection =
				FStateHelper::GetIsCharacterTargetMode(GetAbilitySystemComponentFromActorInfo())
					? FLocateHelper::GetDirectionByMovementData(OwnerCharacter->GetMovementVector()) : EMovementDirection::F;
			// [추가] CurrentDirection 로그 남기기
			UE_LOG(LogTemp, Log, TEXT("CurrentDirection : %s"),
				*UEnum::GetValueAsString(CurrentDirection));
			if (UAnimMontage* EvadeAnim = OwnerCharacter->GetEquipComponent()
				->GetEvadeAnim().GetAnimationByDirection(CurrentDirection))
			{
				if (RollEffect)
				{
					FGameplayEffectContextHandle EffectContext = GetAbilitySystemComponentFromActorInfo()->
			MakeEffectContext();
					EffectContext.AddSourceObject(GetAvatarActorFromActorInfo());

					FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(
						RollEffect, 1.0f, EffectContext);

					if (SpecHandle.IsValid())
					{
						ActiveHandle = GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(
							*SpecHandle.Data.Get());
					}
				}
				if (WaitDelayTask)
				{
					WaitDelayTask->OnFinish.RemoveDynamic(this, &UGA_Sprint::OnWaitTimeFinished);//달리는 테스크를 지우는 코드 이 코드가 없다면 달리는 버프가 활성화 되어서 문제가 됨 
				}
				SetCurrentMontage(EvadeAnim);
				
					UPlayMontageWithEvent* Task = UPlayMontageWithEvent::InitialEvent(this,
									NAME_None, GetCurrentMontage(), FGameplayTagContainer());
					Task->OnCompleted.AddDynamic(this, &ThisClass::EndEvade);
					Task->ReadyForActivation();
				
			} else
			{
				EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo,
					true, false);
			}
		}
	} else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo,
					true, false);
	}
}

void UGA_Sprint::EndEvade(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo,
		true, false);
}

void UGA_Sprint::CancelAbility(const FGameplayAbilitySpecHandle Handle
														, const FGameplayAbilityActorInfo* ActorInfo
														, const FGameplayAbilityActivationInfo
														ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
	// GetAbilitySystemComponentFromActorInfo()->
	// 	RemoveActiveGameplayEffectBySourceEffect(BuffEffect, GetAbilitySystemComponentFromActorInfo());
}

void UGA_Sprint::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	GetAbilitySystemComponentFromActorInfo()->
		RemoveActiveGameplayEffectBySourceEffect(BuffEffect, GetAbilitySystemComponentFromActorInfo());
	GetAbilitySystemComponentFromActorInfo()->
		RemoveActiveGameplayEffectBySourceEffect(RollEffect, GetAbilitySystemComponentFromActorInfo());
}

