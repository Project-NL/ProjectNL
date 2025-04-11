#pragma once

#include "CoreMinimal.h"
#include "CombatAnimationData.h"
#include "Components/ActorComponent.h"
#include "ProjectNL/Helper/UtilHelper.h"
#include "EquipComponent.generated.h"

class ABaseWeapon;
enum class EPlayerCombatWeaponState : uint8;

// TODO: 해당 컴포넌트의 이름에 대한 고민이 더 필요하다.
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTNL_API UEquipComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEquipComponent();

	void UpdateEquipWeaponAnimationData();

	void MoveNextComboCount();

	void ClearCurrentComboCount();
	
	GETTER(uint8, AttackComboIndex)
	
	GETTER(bool, IsFirstEquipWeapon)
	
	// ABP에서 주로 사용함.
	GETTER(TArray<TObjectPtr<UAnimMontage>>, ComboAttackAnim)
	GETTER(TObjectPtr<UAnimMontage>, HeavyAttackAnim);
	GETTER(TObjectPtr<UAnimMontage>, JumpAttackAnim);
	
	GETTER(TObjectPtr<UAnimMontage>, EquipAnim)
	GETTER(TObjectPtr<UAnimMontage>, UnEquipAnim)
	GETTER(TObjectPtr<UAnimMontage>, BlockAnim)
	GETTER(FAnimationByRotation, EvadeAnim)
	GETTER(FAnimationByRotation, StepAnim)
	GETTER(FAnimationBy4RotationWithHeight, DamagedAnim)

	GETTER(TObjectPtr<ABaseWeapon>, MainWeapon)
	GETTER(TObjectPtr<ABaseWeapon>, SubWeapon)

protected:
	virtual void BeginPlay() override;

	void SetAnimationsByWeaponState();

private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EPlayerCombatWeaponState PlayerCombatWeaponState;
	
	UPROPERTY(Category="Property", EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	bool IsFirstEquipWeapon = true;

	// 무기 관련 정보
	// TODO: class 관련은 테스트용이기에 추후 없애야한다.
	UPROPERTY(Category="Property|Weapon", EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TSubclassOf<ABaseWeapon> MainWeaponClass;

	UPROPERTY(Category="Property|Weapon", EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TSubclassOf<ABaseWeapon> SubWeaponClass;

	TObjectPtr<ABaseWeapon> MainWeapon;
	
	TObjectPtr<ABaseWeapon> SubWeapon;

	// 애니메이션 관련 정보 Table
	// 아무 DB나 다 들어가게 되어있지만 실제 사용 DB는 CombatAnimationData.h 기준으로 들어감
	// TODO: 타입 제한이 가능한지 다시 check할 필요가 있음
	UPROPERTY(Category="Property|Weapon", EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataTable> CombatAnimData;

	// 애니메이션 관련 정보
	UPROPERTY()
	TObjectPtr<UAnimMontage> EquipAnim;

	UPROPERTY()
	TObjectPtr<UAnimMontage> UnEquipAnim;
	
	UPROPERTY()
	TObjectPtr<UAnimMontage> BlockAnim;

	UPROPERTY()
	TArray<TObjectPtr<UAnimMontage>> ComboAttackAnim;
	//강공격
	UPROPERTY()
	TObjectPtr<UAnimMontage> HeavyAttackAnim;
	
	//점프공격
	UPROPERTY()
	TObjectPtr<UAnimMontage> JumpAttackAnim;

	FAnimationByRotation EvadeAnim;
	
	FAnimationByRotation StepAnim;

	FAnimationBy4RotationWithHeight DamagedAnim;

	// TODO: 추후 코드 분리 필요 (EquipComponent와 맞는 취지는 아님)
	uint8 AttackComboIndex = 0;
	uint8 MaxAttackCombo = 0;
};
