#include "GE_DamageExecCalculation.h"

#include "AbilitySystemComponent.h"
#include "ProjectNL/Component/EquipComponent/CombatAnimationData.h"
#include "ProjectNL/GAS/NLAbilitySystemComponent.h"
#include "ProjectNL/Helper/GameplayTagHelper.h"

UGE_DamageExecCalculation::UGE_DamageExecCalculation()
{
	DamageType = EDamageType::Physical;
	DamageValue = 0.f;
}

void UGE_DamageExecCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	UAbilitySystemComponent* TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();
	const UAbilitySystemComponent* SourceAbilitySystemComponent = ExecutionParams.GetSourceAbilitySystemComponent();

	AActor* TargetActor = TargetAbilitySystemComponent->GetAvatarActor();
	AActor* SourceActor = SourceAbilitySystemComponent->GetAvatarActor();

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	
	const EMovementDirection Direction = static_cast<EMovementDirection>(Spec.GetSetByCallerMagnitude(NlGameplayTags::Data_AttackDirection.GetModuleName()));

	UE_LOG(LogTemp, Display, TEXT("Damaged Source And Target: %s, %s, %s"), *SourceActor->GetName(), *TargetActor->GetName(), *FEnumHelper::GetClassEnumKeyAsString(Direction));
	// TODO: SetCaller로 계산된 effect 정보를 기반으로 방어력과 스텟, 무기 공격력 등을 계산 후 최종 데미지를 산정
	if (UNLAbilitySystemComponent* TargetASC = Cast<UNLAbilitySystemComponent>(TargetAbilitySystemComponent))
	{
		TargetASC->ReceiveDamage({ SourceActor,DamageValue, Direction, ETargetHeight::Middle,DamageEffect });
	}
}
