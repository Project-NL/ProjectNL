// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseAttributeSet.h"
#include "PlayerAttributeSet.generated.h"

UCLASS()
class PROJECTNL_API UPlayerAttributeSet : public UBaseAttributeSet
{
	GENERATED_BODY()

public:
	virtual void InitBaseAttribute() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(BlueprintReadOnly, Category="Attributes", ReplicatedUsing = OnRepExp)
	FGameplayAttributeData Exp;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Exp)

	UPROPERTY(BlueprintReadOnly, Category="Attributes", ReplicatedUsing = OnRepMaxExp)
	FGameplayAttributeData MaxExp;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, MaxExp)
	
	UPROPERTY(BlueprintReadOnly, Category="Attributes")
	FGameplayAttributeData Gold;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Gold)
	
	UFUNCTION()
	virtual void OnRepExp(const FGameplayAttributeData& OldExp);
	
	UFUNCTION()
	virtual void OnRepMaxExp(const FGameplayAttributeData& OldMaxExp);
};
