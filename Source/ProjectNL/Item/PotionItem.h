// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpawnableItem.h"
#include "ProjectNL/GAS/Ability/Active/Default/DrinkPotion/GA_DrinkPotion.h"
#include "PotionItem.generated.h"

/**
 * 
 */
class UGameplayEffect;
UCLASS()
class PROJECTNL_API APotionItem : public ASpawnableItem
{
	GENERATED_BODY()
public:
	APotionItem();
	
	virtual void UseItem(APlayerCharacter* playerCharacter) override;


private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Assets
		, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* PotionSkeleton;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerUseItem(APlayerCharacter* playerCharacter);
	UPROPERTY(EditAnywhere, Category = "Ability")
	TSubclassOf<UGA_DrinkPotion> DrinkPotionAbility;

	
};
