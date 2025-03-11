#include "GE_DamageExecCalculation.h"

#include "AbilitySystemComponent.h"
#include "ProjectNL/Component/EquipComponent/CombatAnimationData.h"
#include "ProjectNL/GAS/NLAbilitySystemComponent.h"
#include "ProjectNL/Helper/GameplayTagHelper.h"


UGE_DamageExecCalculation::UGE_DamageExecCalculation()
{
	//DamageResponse.DamageType = EDamageType::Physical;
	DamageResponse.Damage = 0.f;
}

void UGE_DamageExecCalculation::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput
) const
{
	// 타겟 및 소스의 AbilitySystemComponent 획득
	UAbilitySystemComponent* TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();
	const UAbilitySystemComponent* SourceAbilitySystemComponent = ExecutionParams.GetSourceAbilitySystemComponent();

	// 소스와 타겟의 아바타 액터 획득
	AActor* TargetActor = (TargetAbilitySystemComponent != nullptr) ? TargetAbilitySystemComponent->GetAvatarActor() : nullptr;
	AActor* SourceActor = (SourceAbilitySystemComponent != nullptr) ? SourceAbilitySystemComponent->GetAvatarActor() : nullptr;

	if (!SourceActor || !TargetActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("SourceActor or TargetActor is null"));
		return;
	}

	// GameplayEffect 스펙으로부터 SetByCaller 값(공격 방향)을 가져옴
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	// SetByCaller로 전달된 값은 float형이므로 정수로 변환 후 enum으로 캐스팅
	const float RawDirection = Spec.GetSetByCallerMagnitude(NlGameplayTags::Data_AttackDirection.GetModuleName());
	const EMovementDirection Direction = static_cast<EMovementDirection>(FMath::RoundToInt(RawDirection));

	// 로그 출력
	UE_LOG(LogTemp, Display, TEXT("Damaged Source And Target: %s, %s, %s"),
		*SourceActor->GetName(),
		*TargetActor->GetName(),
		*FEnumHelper::GetClassEnumKeyAsString(Direction)
	);

	// FDamagedResponse 구조체 인스턴스를 생성 및 값 할당
	DamageResponse.SourceActor = SourceActor;
	// TODO: 방어력, 스텟, 무기 공격력 등을 반영하여 DamageValue를 계산
	//DamageResponse.Damage = /* 계산된 데미지 값 */ 10.0f;
	DamageResponse.DamagedDirection = Direction;
	// TODO: 타겟의 높이 정보(예: Low, Middle, High)를 계산하여 할당. 여기서는 기본값으로 Middle 사용.
	//DamageResponse.DamagedHeight = ETargetHeight::Middle;
	// TODO: 필요 시 DamageEffect와 KnockbackAbility를 설정. 예시는 멤버 변수나 스펙에서 가져온다고 가정
	//DamageResponse.DamageEffect = DamageEffect;       
	// TODO: HitStop 여부도 상황에 맞게 계산. 여기서는 기본값 false 사용
	//DamageResponse.IsHitStop = false;

	// UNLAbilitySystemComponent로 캐스팅 후 DamageResponse 전달
	if (UNLAbilitySystemComponent* TargetASC = Cast<UNLAbilitySystemComponent>(TargetAbilitySystemComponent))
	{
		TargetASC->ReceiveDamage(DamageResponse);

		FGameplayTagContainer CancelTags;
		CancelTags.AddTag(NlGameplayTags::State_Attack_Combo);

		// 현재 적용된 GameplayEffect 확인 후 제거
		TArray<FActiveGameplayEffectHandle> ActiveEffectHandles;
		TargetASC->GetActiveEffectsWithAllTags(CancelTags);

		for (const FActiveGameplayEffectHandle& Handle : ActiveEffectHandles)
		{
			TargetASC->RemoveActiveGameplayEffect(Handle, 1);
		}

		// 활성화된 어빌리티 중 해당 태그를 가진 것 취소
		TArray<FGameplayAbilitySpec> ActiveAbilities = TargetASC->GetActivatableAbilities();
		for (FGameplayAbilitySpec AbilitySpec : ActiveAbilities)
		{
			if (AbilitySpec.Ability && AbilitySpec.Ability->AbilityTags.HasTagExact(NlGameplayTags::State_Attack_Combo))
			{
				TargetASC->CancelAbilityHandle(AbilitySpec.Handle);
			}
		}
	}
}
// TargetAbilitySystemComponent->
// 	RemoveLooseGameplayTag(NlGameplayTags::State_Attack_Combo);