#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "ProjectNL/GAS/NLAbilitySystemComponent.h"
#include "ProjectNL/GAS/NLAbilitySystemInitializationData.h"
#include "ProjectNL/Helper/EnumHelper.h"
#include "ProjectNL/Helper/UtilHelper.h"
#include "BaseCharacter.generated.h"

enum class EEntityCategory : uint8;

UCLASS()
class PROJECTNL_API ABaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override
	{
		return AbilitySystemComponent;
	}

	UFUNCTION(Server, Reliable)
	void Server_ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> Effect, const uint32 Level = 1);

	UFUNCTION(Server, Reliable)
	void Server_RemoveActiveGameplayEffectBySourceEffect(
		TSubclassOf<UGameplayEffect> Effect);
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY()
	UNLAbilitySystemComponent* AbilitySystemComponent;
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Entity|Category"
		, meta = (AllowPrivateAccess = "true"))
	EEntityCategory EntityType = EEntityCategory::Undefined;
	GETTER_SETTER(EEntityCategory, EntityType);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Entity|Status"
		, meta = (AllowPrivateAccess = "true"))
	bool IsFirstEquip = false;
	GETTER(bool, IsFirstEquip);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AbilitySystem"
		, meta = (AllowPrivateAccess = "true"))
	FNLAbilitySystemInitializationData InitializeData;
};
