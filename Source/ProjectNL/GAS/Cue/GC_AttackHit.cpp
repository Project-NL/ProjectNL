#include "ProjectNL/GAS/Cue/GC_AttackHit.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectNL/Character/BaseCharacter.h"
#include "ProjectNL/Component/EquipComponent/EquipComponent.h"
#include "ProjectNL/Weapon/BaseWeapon.h"

UGC_AttackHit::UGC_AttackHit()
{
}

bool UGC_AttackHit::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	const FHitResult* HitResult = Parameters.EffectContext.GetHitResult();
	ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(MyTarget);
	if (!BaseCharacter)
	{
		return false;
	}
	UEquipComponent* EquipComponent= BaseCharacter->GetEquipComponent();
	if (!EquipComponent)
	{
		return false;
	}
	ABaseWeapon* BaseWeapon= EquipComponent->GetMainWeapon();
	if (!BaseWeapon)
	{
		return false;
	}
	const FGameplayTag Tag = Parameters.MatchedTagName;
	// UNiagaraSystem* NiagaraSystem = BaseWeapon->GetAttackHitNiagaraSystemByTag(Tag);
	// if (UNiagaraSystem* const* FoundSystem = AttackHitNiagaraMap.Find(Tag))
	// {
	// 	return *FoundSystem;
	// }
	// UNiagaraSystem* NiagaraSystem
	if (HitResult && NiagaraSystem )
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			MyTarget->GetWorld(),
			NiagaraSystem,
			HitResult->ImpactPoint,
			HitResult->ImpactNormal.Rotation(), // 혹은 FRotator::ZeroRotator
			FVector(1.0f),
			true,
			true,
			ENCPoolMethod::AutoRelease,
			true
		);
	}

	return true; // false → true 변경: 실행 성공을 나타냄
}
