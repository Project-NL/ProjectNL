// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectNL/UI/Widget/Enemy/EnemyStatus.h"

#include "ProjectNL/Character/Enemy/EnemyCharacter.h"
#include "ProjectNL/GAS/NLAbilitySystemComponent.h"
#include "ProjectNL/GAS/Attribute/BaseAttributeSet.h"
#include "ProjectNL/GAS/Attribute/PlayerAttributeSet.h"
#include "ProjectNL/Player/BasePlayerState.h"
#include "ProjectNL/UI/Widget/ValuePercentbar/ValuePercentBar.h"

void UEnemyStatus::NativeConstruct()
{
	Super::NativeConstruct();
	
}

void UEnemyStatus::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UEnemyStatus::SetBaseCharacter(AEnemyCharacter* enemyCharacter)
{
	EnemyCharacter = enemyCharacter;
	{
		if (UAbilitySystemComponent* ASC = EnemyCharacter->GetAbilitySystemComponent())
		{
			if (const UBaseAttributeSet* PlayerAttributeSet = EnemyCharacter->
				EnemyAttributeSet)
			{
				HealthBar->InitializePercent(PlayerAttributeSet->GetHealth(), PlayerAttributeSet->GetMaxHealth());
				
				ASC->GetGameplayAttributeValueChangeDelegate(
					PlayerAttributeSet->GetHealthAttribute()).AddUObject(
					this, &ThisClass::HandleCurrentHealthChanged);
				ASC->GetGameplayAttributeValueChangeDelegate(
					PlayerAttributeSet->GetMaxHealthAttribute()).AddUObject(
					this, &ThisClass::HandleMaxHealthChanged);
			}
		}
	}
}

void UEnemyStatus::HandleCurrentHealthChanged(const FOnAttributeChangeData& Data)
{
	if (EnemyCharacter)
	{
		UE_LOG(LogTemp, Log, TEXT("Character: %s, Current Health Changed: %f"), 
			*EnemyCharacter->GetName(), Data.NewValue);
	}
	HealthBar->SetCurrentValue(Data.NewValue/4);
}

void UEnemyStatus::HandleMaxHealthChanged(const FOnAttributeChangeData& Data)
{
	HealthBar->SetMaxValue(Data.NewValue/4);
}
