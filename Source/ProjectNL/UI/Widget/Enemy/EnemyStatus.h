// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnemyStatus.generated.h"

class AEnemyCharacter;
class ABaseCharacter;
class UValuePercentBar;
class UProgressBar;
struct FOnAttributeChangeData;
/**
 * 
 */
UCLASS()
class PROJECTNL_API UEnemyStatus : public UUserWidget
{
	GENERATED_BODY()
public:
	

	void SetBaseCharacter(AEnemyCharacter* EnemyCharacter);
protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void HandleCurrentHealthChanged(const FOnAttributeChangeData& Data);
	void HandleMaxHealthChanged(const FOnAttributeChangeData& Data);
	
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthBar;
	
	AEnemyCharacter* EnemyCharacter;
};
