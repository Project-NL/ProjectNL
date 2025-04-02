#include "BaseAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "ProjectNL/Character/BaseCharacter.h"

void UBaseAttributeSet::OnRepHealth(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, Health, OldHealth);

	UNLAbilitySystemComponent* ASC = Cast<UNLAbilitySystemComponent>(GetOwningAbilitySystemComponent());
	if (Health.GetCurrentValue()<=0)
	{
		
		//	ASC->OnDeathReactNotified.Broadcast();
			
	}
}

void UBaseAttributeSet::OnRepMaxHealth(
	const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, MaxHealth, OldMaxHealth);
}

void UBaseAttributeSet::OnRepMana(const FGameplayAttributeData& OldMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, Mana, OldMana);
}

void UBaseAttributeSet::OnRepMaxMana(const FGameplayAttributeData& OldMaxMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, MaxMana, OldMaxMana);
}

void UBaseAttributeSet::OnRepStamina(const FGameplayAttributeData& OldStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, Stamina, OldStamina);
}

void UBaseAttributeSet::OnRepMaxStamina(
	const FGameplayAttributeData& OldMaxStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, MaxStamina, OldMaxStamina);
}

void UBaseAttributeSet::OnRepLevel(const FGameplayAttributeData& OldLevel)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, Level, OldLevel);
}

void UBaseAttributeSet::OnRepMovementSpeed(
	const FGameplayAttributeData& OldMovementSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, MovementSpeed
															, OldMovementSpeed);
}

void UBaseAttributeSet::PostGameplayEffectExecute(const  FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	// if ((GetHealth() <= 0.0f) )
	// {
	// 	//Data.Target.AddLooseGameplayTag(ABTAG_CHARACTER_ISDEAD);
	// 	OnOutOfHealth.Broadcast();
	// }

}

void UBaseAttributeSet::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributeSet, Health, COND_None
																, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributeSet, MaxHealth, COND_None
																, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributeSet, Mana, COND_None
																, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributeSet, MaxMana, COND_None
																, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributeSet, Stamina, COND_None
																, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributeSet, MaxStamina, COND_None
																, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributeSet, Level, COND_None
																, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributeSet, MovementSpeed, COND_None
																, REPNOTIFY_Always);
}

void UBaseAttributeSet::InitBaseAttribute()
{
	InitLevel(1);

	InitEndurance(10);
	InitMuscularPower(10);
	InitWisdom(10);
	InitConcentration(10);
	InitSharpness(10);
	InitGuts(10);

	InitMaxHealth(200 + GetEndurance() * 20);
	InitHealth(GetMaxHealth());
	InitMaxMana(100);
	InitMana(GetMaxMana());
	InitMaxStamina(100);
	InitStamina(GetMaxStamina());

	InitPhysicalDamage(10 + GetMuscularPower() * 5);
	InitMagicalDamage(10 + GetWisdom() * 5);
	InitMovementSpeed(280);
	InitDefense(10);
	InitCriticalPercent(10 + GetConcentration() * 0.01);
	InitCriticalPower(10 + GetSharpness() * 0.005);
}

void UBaseAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	if (NewValue < 0) NewValue = 0;
	
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Min(NewValue, GetMaxHealth());

		// if ((GetHealth() <= 0.0f) )
		// {
		// 	//Data.Target.AddLooseGameplayTag(ABTAG_CHARACTER_ISDEAD);
		// 	OnOutOfHealth.Broadcast();
		// }
	}

	
}

void UBaseAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	AActor* OwningActor = GetOwningActor();
	if (!OwningActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Owning Actor not found!"));
		return;
	}
	
	UNLAbilitySystemComponent* ASC = Cast<UNLAbilitySystemComponent>(GetOwningAbilitySystemComponent());
	if (Attribute == GetHealthAttribute())
	{
		if ((GetHealth() <= 0.0f) )
		{
			//ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(OwningActor);
			//BaseCharacter->ActiveDeathAbility();
			//ASC->OnDeathReactNotified.Broadcast();
			//Data.Target.AddLooseGameplayTag(ABTAG_CHARACTER_ISDEAD);
			OnOutOfHealth.Broadcast();
		}
	}
	// if (Health.GetCurrentValue() <= 0.0f)
	// {
	// 	
	// 	OnOutOfHealth.Broadcast();
	// 	
	// }
}


