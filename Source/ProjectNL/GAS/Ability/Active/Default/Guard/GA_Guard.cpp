#include "GA_Guard.h"

#include "ProjectNL/Character/Player/PlayerCharacter.h"
#include "ProjectNL/Component/EquipComponent/EquipComponent.h"
#include "ProjectNL/GAS/Ability/Active/Default/Knockback/AT_Knockback.h"
#include "ProjectNL/GAS/Ability/Active/Default/Knockback/GA_Knockback.h"
#include "ProjectNL/GAS/Ability/Utility/PlayMontageWithEvent.h"
#include "ProjectNL/GAS/Attribute/PlayerAttributeSet.h"
#include "ProjectNL/Helper/AbilityHelper.h"
#include "ProjectNL/Helper/GameplayTagHelper.h"
#include "ProjectNL/Helper/StateHelper.h"

UGA_Guard::UGA_Guard(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
	
}

bool UGA_Guard::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	if (UBaseAttributeSet* AttributeSet = Cast<UBaseAttributeSet>(AbilityHelper::GetAttribute(GetAvatarActorFromActorInfo())))
	{
		if (AttributeSet->GetStamina() < 30) return false;
	}
	return FStateHelper::IsCombatMode(GetAbilitySystemComponentFromActorInfo());
}

void UGA_Guard::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	NlGameplayTags::SetGameplayTag(GetAbilitySystemComponentFromActorInfo(),NlGameplayTags::Status_Guard, 1, true);
	
	if (ABaseCharacter* CharacterInfo = Cast<ABaseCharacter>(GetAvatarActorFromActorInfo()))
	{
		UEquipComponent* EquipComponent = CharacterInfo->GetEquipComponent();
		check(EquipComponent)
		// EquipComponent가 다 실행되기 이전에 해당 Ability가 실행되는 경우는
		// 이 액션이 중단될 수 있으니 주의하면 좋다.
		// TODO: 애니메이션 Binding에 대해 AvatarSet에서 진행해 EquipComponent에 주입하는 것도 좋아보임
		SetCurrentMontage(EquipComponent->GetBlockAnim());
		
		if (UNLAbilitySystemComponent* ASC = Cast<UNLAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo()))
		{
			ASC->OnDamageStartedNotified.AddDynamic(this, &ThisClass::StartBlock);
		}

		
	}
}

void UGA_Guard::StartBlock(const FDamagedResponse& DamagedResponse)
{
	if (UBaseAttributeSet* AttributeSet = Cast<UBaseAttributeSet>(AbilityHelper::GetAttribute(GetAvatarActorFromActorInfo())))
	{
		// 플레이어의 경우 스테미나가 30 이상 있어야만 Block 스킬이 발동.
		if (AttributeSet->GetStamina() > 30)
		{
			
			if (GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(NlGameplayTags::Status_Guard))
			{
				FStateHelper::ChangePlayerState(
					GetAbilitySystemComponentFromActorInfo(),
					NlGameplayTags::Status_Guard,
					NlGameplayTags::Status_Block, true);
				AttributeSet->SetStamina(AttributeSet->GetStamina() - 30);
				//튕겨져 나가며 애니메이션 발동
				OnKnockBack(DamagedResponse);
			}
			
		} else
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		}
	}
}

void UGA_Guard::OnKnockBack(const FDamagedResponse& DamagedResponse)
{
	UAbilitySystemComponent* AbilitySystemComponent=GetAbilitySystemComponentFromActorInfo();
	FGameplayAbilitySpec AbilitySpec = AbilitySystemComponent->BuildAbilitySpecFromClass(DamagedResponse.KnockbackAbility, 1, INDEX_NONE);
	FGameplayAbilitySpecHandle AbilityHandle = AbilitySystemComponent->GiveAbility(AbilitySpec);

	// 2. AbilityHandle을 통해 AbilitySpec 포인터를 가져옵니다.
	FGameplayAbilitySpec* Spec = AbilitySystemComponent->FindAbilitySpecFromHandle(AbilityHandle);
	if (!Spec)
	{
		UE_LOG(LogTemp, Warning, TEXT("AbilityHandle에 해당하는 AbilitySpec을 찾을 수 없습니다."));
		return;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AbilityHandle에 해당하는 AbilitySpec을 찾을 수 있습니다."));
		//return;
	}
	// 3. Spec에서 Primary Instance를 가져옵니다.
	UGameplayAbility* AbilityInstance = Spec->GetPrimaryInstance();
    
	
	// 4. UGA_Knockback 타입으로 캐스팅합니다.
	ActivatedKnockbackAbility = Cast<UGA_Knockback>(AbilityInstance);
	if (!ActivatedKnockbackAbility)
	{
		UE_LOG(LogTemp, Warning, TEXT("AbilityInstance를 UGA_Knockback으로 캐스팅하는 데 실패했습니다."));
		return;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AbilityInstance를 UGA_Knockback으로 캐스팅하는 데 성공."));
		//return;
	}
	// 5. Knockback Ability에 데미지 관련 정보 및 애니메이션 길이 설정
	ActivatedKnockbackAbility->SetDamageResponse(DamagedResponse);
	ActivatedKnockbackAbility->SetDamageMontageLength(GetCurrentMontage()->GetPlayLength());
	ActivatedKnockbackAbility->SetDamageMontage(GetCurrentMontage());
	ActivatedKnockbackAbility->OnPlayMontageWithEventDelegate.AddDynamic(this, &ThisClass::UGA_Guard::EndBlock);
	// 6. Knockback Ability 활성화 시도
	bool bActivated = AbilitySystemComponent->TryActivateAbilityByClass(DamagedResponse.KnockbackAbility);
}

void UGA_Guard::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);

}

void UGA_Guard::EndBlock(FGameplayTag EventTag, FGameplayEventData EventData)
{
	UE_LOG(LogTemp, Warning, TEXT("UGA_Guard EndBlock."));
	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	
}

void UGA_Guard::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	UE_LOG(LogTemp, Warning, TEXT("UGA_Guard EndAbility."));
	if (UNLAbilitySystemComponent* ASC = Cast<UNLAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo()))
	{
		NlGameplayTags::SetGameplayTag(ASC, NlGameplayTags::Status_Guard, 0, true);
		NlGameplayTags::SetGameplayTag(ASC, NlGameplayTags::Status_Block, 0, true);
		ASC->OnDamageStartedNotified.RemoveDynamic(this, &ThisClass::StartBlock);
		
	}
}

