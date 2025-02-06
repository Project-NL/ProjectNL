#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "ProjectNL/Helper/UtilHelper.h"
#include "NLAbilitySystemComponent.generated.h"

struct FInitGameplayAbilitySystem;
struct FNLAbilitySystemInitializationData;

enum class EMovementDirection: uint8;
enum class ETargetHeight: uint8;

USTRUCT(BlueprintType)
struct FDamagedResponse
{
	GENERATED_BODY()
	UPROPERTY()
	AActor* SourceActor;
	UPROPERTY(EditAnywhere)
	float Damage;
	UPROPERTY()
	EMovementDirection DamagedDirection;
	UPROPERTY(EditAnywhere)
	ETargetHeight DamagedHeight;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> DamageEffect;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayAbility> KnockbackAbility;
	UPROPERTY(EditAnywhere)
	bool IsHitStop;

};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDamageStartedNotifiedSignature, const FDamagedResponse&, DamageResponse);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDamageReactNotifiedSignature, const FDamagedResponse&, DamageResponse);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTNL_API UNLAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UNLAbilitySystemComponent();

	void InitializeAbilitySystem(
		const FNLAbilitySystemInitializationData& InitData);
	
	void ReceiveDamage(const FDamagedResponse& DamagedResponse) const;



	GETTER_SETTER(bool, IsInitialized)

	FOnDamageStartedNotifiedSignature OnDamageStartedNotified;
	FOnDamageReactNotifiedSignature OnDamageReactNotified;
private:
	UPROPERTY(EditDefaultsOnly, Category="Options", meta = (AllowPrivateAccess = true))
	uint16 LevelByDamaged = 10;
	
	bool IsInitialized = false;
	
};
