#pragma once

#include "CoreMinimal.h"
#include "ProjectNL/Character/BaseCharacter.h"
#include "PlayerCharacter.generated.h"

class UPlayerSpringArmComponent;
class UPlayerCameraComponent;
class UPlayerAttributeSet;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

UCLASS()
class PROJECTNL_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();
	
	UPROPERTY()
	TObjectPtr<UPlayerAttributeSet> PlayerAttributeSet;

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
	void OnDamaged(const FDamagedResponse& DamagedResponse);
	
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
};
