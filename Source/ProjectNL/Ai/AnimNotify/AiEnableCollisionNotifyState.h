#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ProjectNL/Weapon/BaseWeapon.h"
#include "ProjectNL/GAS/Ability/Active/Default/Action/ActionAbilityData.h"
#include "AiEnableCollisionNotifyState.generated.h"

class ABaseWeapon;
class UCharacterAnimInstance;
class UGameplayEffect;

UCLASS()
class PROJECTNL_API UAiEnableCollisionNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;

	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	void StartTrace(AActor* Owner);

private:
	void MakeSweepTrace(AActor* Owner, ABaseWeapon* Weapon);
	
	void PerformShapeSweepTrace(
		AActor* Owner,
		const FVector& Start,
		const FVector& End,
		TArray<FHitResult>& OutHitResults);

	void ReactToHitActor(AActor* Owner, ABaseWeapon* Weapon, TArray<FHitResult>& HitResults);

	FVector CurrentStartLocation;
	FVector CurrentEndLocation;

	UPROPERTY(EditAnywhere, Category="Options", meta=(AllowPrivateAccess = true))
	uint8 IsHitStop:1 = false;
};