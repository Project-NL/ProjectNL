#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "ProjectNL/GAS/NLAbilitySystemComponent.h"
#include "BasePlayerState.generated.h"

class UPlayerAttributeSet;

UCLASS()
class PROJECTNL_API ABasePlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	ABasePlayerState();

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ability System")
	TObjectPtr<UNLAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UPlayerAttributeSet> AttributeSet;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
};
