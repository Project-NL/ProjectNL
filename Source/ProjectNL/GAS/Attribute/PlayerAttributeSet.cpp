#include "PlayerAttributeSet.h"
#include "Net/UnrealNetwork.h"

void UPlayerAttributeSet::InitBaseAttribute()
{
	Super::InitBaseAttribute();
	InitExp(0);
	InitMaxExp(0);
	InitGold(0);
}

void UPlayerAttributeSet::OnRepExp(const FGameplayAttributeData& OldExp) {
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, Exp, OldExp);
}

void UPlayerAttributeSet::OnRepMaxExp(const FGameplayAttributeData& OldMaxExp) {
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, MaxExp, OldMaxExp);
}

void UPlayerAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, Exp, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, MaxExp, COND_None, REPNOTIFY_Always);
}