// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_RemoveGameplayTag.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTNL_API UAnimNotify_RemoveGameplayTag : public UAnimNotify
{
	GENERATED_BODY()
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	// 제거하려는 Gameplay Tag
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay Tag")
	FGameplayTag TagToRemove;
};
