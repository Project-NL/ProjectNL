#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
// ATTRIBUTE_ACCESSORS 선언시 GAMEPLAYATTRIBUTE_VALUE_SETTER에서 ASC를 사용함
#include "AbilitySystemComponent.h"
#include "BaseAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class PROJECTNL_API UBaseAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	virtual void InitBaseAttribute();

	UPROPERTY(BlueprintReadOnly, Category="Attributes"
		, ReplicatedUsing = OnRepHealth)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Health)

	UPROPERTY(BlueprintReadOnly, Category="Attributes"
		, ReplicatedUsing = OnRepMaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, MaxHealth)

	UPROPERTY(BlueprintReadOnly, Category="Attributes"
		, ReplicatedUsing = OnRepMana)
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Mana)

	UPROPERTY(BlueprintReadOnly, Category="Attributes"
		, ReplicatedUsing = OnRepMaxMana)
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, MaxMana)

	UPROPERTY(BlueprintReadOnly, Category="Attributes"
		, ReplicatedUsing = OnRepStamina)
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Stamina)

	UPROPERTY(BlueprintReadOnly, Category="Attributes"
		, ReplicatedUsing = OnRepMaxStamina)
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, MaxStamina)

	// 실제 반영 스텟 Meta Attribute
	UPROPERTY(BlueprintReadOnly, Category="Attributes")
	FGameplayAttributeData PhysicalDamage;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, PhysicalDamage)

	UPROPERTY(BlueprintReadOnly, Category="Attributes")
	FGameplayAttributeData MagicalDamage;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, MagicalDamage)

	UPROPERTY(BlueprintReadOnly, Category="Attributes")
	FGameplayAttributeData Defense;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Defense)

	UPROPERTY(BlueprintReadOnly, Category="Attributes"
		, ReplicatedUsing = OnRepMovementSpeed)
	FGameplayAttributeData MovementSpeed;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, MovementSpeed)

	UPROPERTY(BlueprintReadOnly, Category="Attributes")
	FGameplayAttributeData CriticalPercent;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, CriticalPercent)

	UPROPERTY(BlueprintReadOnly, Category="Attributes")
	FGameplayAttributeData CriticalPower;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, CriticalPower)

	// 수치 조절 스텟 Meta Attribute
	// 지구력
	UPROPERTY(BlueprintReadOnly, Category="Attributes")
	FGameplayAttributeData Endurance;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Endurance)

	// 근력
	UPROPERTY(BlueprintReadOnly, Category="Attributes")
	FGameplayAttributeData MuscularPower;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, MuscularPower)

	// 지혜
	UPROPERTY(BlueprintReadOnly, Category="Attributes")
	FGameplayAttributeData Wisdom;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Wisdom)

	// 집중
	UPROPERTY(BlueprintReadOnly, Category="Attributes")
	FGameplayAttributeData Concentration;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Concentration)

	// 예리
	UPROPERTY(BlueprintReadOnly, Category="Attributes")
	FGameplayAttributeData Sharpness;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Sharpness)

	// 근성
	UPROPERTY(BlueprintReadOnly, Category="Attributes")
	FGameplayAttributeData Guts;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Guts)

	UPROPERTY(BlueprintReadOnly, Category="Attributes"
		, ReplicatedUsing = OnRepLevel)
	FGameplayAttributeData Level;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Level)

protected:
	// TODO: 복제에 도움이 되는 함수지만 정확한 기능은 추후
	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	
	UFUNCTION()
	virtual void OnRepHealth(const FGameplayAttributeData& OldHealth);
	
	UFUNCTION()
	virtual void OnRepMaxHealth(const FGameplayAttributeData& OldMaxHealth);
	
	UFUNCTION()
	virtual void OnRepMana(const FGameplayAttributeData& OldMana);
	
	UFUNCTION()
	virtual void OnRepMaxMana(const FGameplayAttributeData& OldMaxMana);
	
	UFUNCTION()
	virtual void OnRepStamina(const FGameplayAttributeData& OldStamina);
	
	UFUNCTION()
	virtual void OnRepMaxStamina(const FGameplayAttributeData& OldMaxStamina);
	
	UFUNCTION()
	virtual void OnRepLevel(const FGameplayAttributeData& OldLevel);
	
	UFUNCTION()
	virtual void OnRepMovementSpeed(
		const FGameplayAttributeData& OldMovementSpeed);
};
