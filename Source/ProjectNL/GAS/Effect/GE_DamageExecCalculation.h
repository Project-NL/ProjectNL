#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "ProjectNL/GAS/NLAbilitySystemComponent.h"
#include "GE_DamageExecCalculation.generated.h"

// TODO: 현재는 진짜 이곳에서만 쓰여서 enum 선언함
// 상황에 따라 EnumHelper로 이전 필요함
UENUM()
enum class EDamageType
{
	Physical UMETA(DisplayName = "Physical")
	, Magical UMETA(DisplayName = "Magical")
	, TrueDamage UMETA(DisplayName = "TrueDamage")
};

UCLASS()
class PROJECTNL_API UGE_DamageExecCalculation : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UGE_DamageExecCalculation();

protected:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

private:
	
	// 블루프린트에서 수정 가능하도록 BlueprintReadWrite 추가
	UPROPERTY(EditAnywhere, Category = "Value", meta = (AllowPrivateAccess = "true"))
	mutable FDamagedResponse DamageResponse;
};
