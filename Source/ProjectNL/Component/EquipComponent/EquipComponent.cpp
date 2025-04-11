#include "EquipComponent.h"

#include "CombatAnimationData.h"
#include "GameFramework/Character.h"
#include "ProjectNL/Character/BaseCharacter.h"
#include "ProjectNL/Helper/StateHelper.h"
#include "ProjectNL/Weapon/BaseWeapon.h"

UEquipComponent::UEquipComponent()
{
	// 혹시 몰라 초기값 할당
	PlayerCombatWeaponState = EPlayerCombatWeaponState::None;
}

void UEquipComponent::BeginPlay()
{
	Super::BeginPlay();
	if (ABaseCharacter* Character = Cast<ABaseCharacter>(GetOwner()))
	{
		MainWeapon = GetWorld()->SpawnActor<ABaseWeapon>(MainWeaponClass);
		SubWeapon = GetWorld()->SpawnActor<ABaseWeapon>(SubWeaponClass);
		
		if (IsFirstEquipWeapon)
		{
			if (MainWeapon)
            {
             	MainWeapon->EquipCharacterWeapon(Character, true);
            }
            if (SubWeapon)
            {
             	SubWeapon->EquipCharacterWeapon(Character, false);
            }
		} else
		{
			if (MainWeapon)
            {
            	MainWeapon->UnEquipCharacterWeapon(Character, true);
            }
            if (SubWeapon)
            {
            	SubWeapon->UnEquipCharacterWeapon(Character, false);
            }	
		}
	}
	
	UpdateEquipWeaponAnimationData();
}

void UEquipComponent::UpdateEquipWeaponAnimationData()
{
	PlayerCombatWeaponState = FStateHelper::GetCharacterWeaponState(MainWeapon, SubWeapon);
	SetAnimationsByWeaponState();
}

// TODO: 추후 코드 분리 필요 (EquipComponent와 맞는 취지는 아님)
void UEquipComponent::MoveNextComboCount()
{
	AttackComboIndex = AttackComboIndex == MaxAttackCombo - 1 ? 0 : AttackComboIndex + 1;
}

void UEquipComponent::ClearCurrentComboCount()
{
	AttackComboIndex = 0;
}



// TODO: 추후 별도의 Manager로 옮겨야 할 지 고려할 필요 있음
void UEquipComponent::SetAnimationsByWeaponState()
{
	const FString RowName = FEnumHelper::GetClassEnumKeyAsString(
		PlayerCombatWeaponState);

	if (const FCombatAnimationData* AnimData = CombatAnimData
		->FindRow<FCombatAnimationData>(*RowName, ""))
	{
		ComboAttackAnim = AnimData->ComboAttackAnim;
		MaxAttackCombo = AnimData->ComboAttackAnim.Num();
		
		HeavyAttackAnim = AnimData->HeavyAttackAnim;
		JumpAttackAnim = AnimData->JumpAttackAnim;
		EquipAnim = AnimData->EquipAnim;
		UnEquipAnim = AnimData->UnEquipAnim;
		BlockAnim = AnimData->BlockAnim;
		EvadeAnim = AnimData->EvadeAnim;
		StepAnim = AnimData->StepAnim;
		DamagedAnim = AnimData->DamagedAnim;
	}
}