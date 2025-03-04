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

void UEquipComponent::InternalEquipWeaponLogic(TSubclassOf<AActor> WeaponClass, bool bIsMainWeapon)
{
	if (ABaseCharacter* Character = Cast<ABaseCharacter>(GetOwner()))
	{
		// 기존 무기 해제 및 제거
		if (bIsMainWeapon && MainWeapon)
		{
			MainWeapon->UnEquipCharacterWeapon(Character, true);
			MainWeapon->Destroy();
			MainWeapon = nullptr;
		}
		else if (!bIsMainWeapon && SubWeapon)
		{
			SubWeapon->UnEquipCharacterWeapon(Character, false);
			SubWeapon->Destroy();
			SubWeapon = nullptr;
		}
		// ItemID를 기반으로 무기 클래스 결정 (예시: 데이터 테이블 활용)
		if (bIsMainWeapon)
		{
			MainWeaponClass=WeaponClass;
			ABaseWeapon* NewWeapon = GetWorld()->SpawnActor<ABaseWeapon>(MainWeaponClass);
			MainWeapon = NewWeapon;
			MainWeapon->EquipCharacterWeapon(Character, bIsMainWeapon);
		}
		else
		{
			SubWeaponClass=WeaponClass;
			ABaseWeapon* NewWeapon = GetWorld()->SpawnActor<ABaseWeapon>(SubWeaponClass);
			SubWeapon = NewWeapon;
			SubWeapon->EquipCharacterWeapon(Character, bIsMainWeapon);
		}
		UpdateEquipWeaponAnimationData(); // 애니메이션 데이터 갱신
	}
}

void UEquipComponent::EquipWeapon(TSubclassOf<AActor> WeaponClass, bool bIsMainWeapon)
{
	// 현재 로컬이 서버 권한이 없는 경우(클라이언트) => 서버로 RPC 호출
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		ServerEquipWeapon(WeaponClass, bIsMainWeapon);
	}
	else
	{
		// 서버이므로 직접 로직 실행
		MulticastEquipWeapon(WeaponClass, bIsMainWeapon);
	}
}

void UEquipComponent::UnequipWeapon(bool bIsMainWeapon)
{
	if (ABaseCharacter* Character = Cast<ABaseCharacter>(GetOwner()))
	{
		if (bIsMainWeapon && MainWeapon)
		{
			MainWeapon->UnEquipCharacterWeapon(Character, true);
			MainWeapon->Destroy();
			MainWeapon = nullptr;
		}
		else if (!bIsMainWeapon && SubWeapon)
		{
			SubWeapon->UnEquipCharacterWeapon(Character, false);
			SubWeapon->Destroy();
			SubWeapon = nullptr;
		}
		UpdateEquipWeaponAnimationData(); // 애니메이션 데이터 갱신
	}
}

void UEquipComponent::MulticastEquipWeapon_Implementation(TSubclassOf<AActor> WeaponClass, bool bIsMainWeapon)
{
	InternalEquipWeaponLogic(WeaponClass, bIsMainWeapon);
}

void UEquipComponent::ServerEquipWeapon_Implementation(TSubclassOf<AActor> WeaponClass, bool bIsMainWeapon)
{
	MulticastEquipWeapon(WeaponClass, bIsMainWeapon);
}

bool UEquipComponent::ServerEquipWeapon_Validate(TSubclassOf<AActor> WeaponClass, bool bIsMainWeapon)
{
	return true;
}
