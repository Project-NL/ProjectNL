#include "EquipComponent.h"

#include "CombatAnimationData.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "ProjectNL/Character/BaseCharacter.h"
#include "ProjectNL/Helper/StateHelper.h"
#include "ProjectNL/Weapon/BaseWeapon.h"

UEquipComponent::UEquipComponent()
{
	//bAutoActivate = true; // 🔹 자동 활성화
	//PrimaryComponentTick.bCanEverTick = true;
	//bReplicates = true; // 🔹 ActorComponent 리플리케이션 활성화
	SetIsReplicatedByDefault(true); // 🔹
	PlayerCombatWeaponState = EPlayerCombatWeaponState::None;
}

void UEquipComponent::BeginPlay()
{
	Super::BeginPlay();
	if (!IsActive())
	{
		UE_LOG(LogTemp, Error, TEXT("UEquipComponent is not active!"));
	}
	if (MainWeaponClass)
	InternalEquipWeaponLogic(MainWeaponClass,true);
	if (SubWeaponClass)
	InternalEquipWeaponLogic(SubWeaponClass,false);
	
}

void UEquipComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UEquipComponent, PlayerCombatWeaponState);

	 DOREPLIFETIME(UEquipComponent, MainWeapon);
	//
	 DOREPLIFETIME(UEquipComponent, SubWeapon);
}

void UEquipComponent::UpdateEquipWeaponAnimationData()
{
	
	PlayerCombatWeaponState = FStateHelper::GetCharacterWeaponState(MainWeapon, SubWeapon);
	 SetAnimationsByWeaponState();
}
void UEquipComponent::OnRep_PlayerCombatWeaponState()
{
	FString WeaponStateString = UEnum::GetValueAsString(PlayerCombatWeaponState);
	UE_LOG(LogTemp, Warning, TEXT("[CLIENT] OnRep_PlayerCombatWeaponState called. New State: %s"), *WeaponStateString);

	SetAnimationsByWeaponState(); // 변경된 상태에 맞게 애니메이션 업데이트
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
		if (!Character->HasAuthority())
		{
			return;
		}
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

	ServerEquipWeapon(WeaponClass, bIsMainWeapon);

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
	InternalEquipWeaponLogic(WeaponClass, bIsMainWeapon);
}

bool UEquipComponent::ServerEquipWeapon_Validate(TSubclassOf<AActor> WeaponClass, bool bIsMainWeapon)
{
	return true;
}
