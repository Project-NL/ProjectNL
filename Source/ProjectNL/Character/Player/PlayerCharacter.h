#pragma once

#include "CoreMinimal.h"
#include "ProjectNL/Character/BaseCharacter.h"
#include "ProjectNL/Interface/InteractionInterface.h"
#include "PlayerCharacter.generated.h"

class AEnemyCharacter;
class UEquipInventoryComponent;
class UPlayerSpringArmComponent;
class UPlayerCameraComponent;
class UPlayerAttributeSet;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;
struct FGameplayAbilitySpec;
UCLASS()
class PROJECTNL_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();
	
	UPROPERTY()
	TObjectPtr<UPlayerAttributeSet> PlayerAttributeSet;

	UEquipInventoryComponent* GetEquipInventoryComponent();

	void SetTargetingCharacter(AEnemyCharacter* TargetingCharacter);

	UFUNCTION(BlueprintCallable, Category = "Targeting")
	AEnemyCharacter* GetTargetingCharacter();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	void UnlockInput();

protected:
	virtual void BeginPlay() override;

	virtual void OnRep_PlayerState() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void
	SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void MoveTo(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Die();

private:
	UFUNCTION()
	virtual void OnDamaged(const FDamagedResponse& DamagedResponse) override;
	virtual void OnDamagedMontageEnded(UAnimMontage* Montage, bool bInterrupted) override;
	
	UFUNCTION()
	void Death();

	/** 입력 잠금 해제용 */
	FTimerHandle InputUnlockHandle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input
		, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, Category = "Input"
		, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveInputAction;

	UPROPERTY(EditAnywhere, Category = "Input"
		, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookInputAction;
	/** 플레이어 카메라 스프링 암 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UPlayerSpringArmComponent* PlayerCameraSpringArm;
	
	/** 플레이어 카메라 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UPlayerCameraComponent* PlayerCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UEquipInventoryComponent* EquipInventoryComponent;
	
	UPROPERTY(EditDefaultsOnly, Category="Ability|Effect"
		, meta=(AllowPrivateAccess = true))
	TSubclassOf<UGameplayEffect> RegenEffect;
	
	FOnMontageEnded MontageEndedDelegate;

	FDamagedResponse DamageResponse;

	UPROPERTY(EditAnywhere, Category = "Ability")
	TSubclassOf<UGameplayAbility> KnockbackAbility;

	UPROPERTY(Replicated,VisibleAnywhere, BlueprintReadOnly, Category = "Targeting", meta = (AllowPrivateAccess = "true"))
	AEnemyCharacter* TargetingCharacter;

	// 타겟 위젯 클래스
	UPROPERTY(EditDefaultsOnly, Category = "Target System")
	TSubclassOf<UUserWidget> LockedOnWidgetClass;

	// (신규) 무적으로 만들 때 적용할 Gameplay Effect 클래스
	UPROPERTY(EditDefaultsOnly, Category="Ability|Effect"
		, meta=(AllowPrivateAccess = true))
	TSubclassOf<UGameplayEffect> InvincibilityEffect;
	// 생성된 위젯 컴포넌트
	UPROPERTY()
	class UWidgetComponent* LockOnWidgetComponent;

};

