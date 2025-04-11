#include "AbilityHelper.h"

#include "ProjectNL/Character/Enemy/EnemyCharacter.h"
#include "ProjectNL/Character/Player/PlayerCharacter.h"

UAttributeSet* AbilityHelper::GetAttribute(AActor* Owner)
{
	if (const APlayerCharacter* Player = Cast<APlayerCharacter>(Owner))
		return Cast<UAttributeSet>(Player->PlayerAttributeSet);
	if (const AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(Owner))
		return Cast<UAttributeSet>(EnemyCharacter->EnemyAttributeSet);
	
	return nullptr;
}
