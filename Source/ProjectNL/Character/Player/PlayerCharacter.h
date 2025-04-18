﻿#pragma once

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
class PROJECTNL_API APlayerCharacter : public ABaseCharacter,public IInteractionInterface
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
protected:
	virtual void BeginPlay() override;

	virtual void OnRep_PlayerState() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void
	SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void MoveTo(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

private:
	UFUNCTION()
	virtual void OnDamaged(const FDamagedResponse& DamagedResponse) override;
	virtual void OnDamagedMontageEnded(UAnimMontage* Montage, bool bInterrupted) override;
	virtual void OnKnockback(const FDamagedResponse& DamagedResponse,float DamageMontageLength) override;
	UFUNCTION()
	void Death();

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Targeting", meta = (AllowPrivateAccess = "true"))
	AEnemyCharacter* TargetingCharacter;
};

