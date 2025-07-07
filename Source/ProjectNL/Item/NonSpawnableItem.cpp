// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectNL/Item/NonSpawnableItem.h"

void UNonSpawnableItem::Interact(AActor* Actor)
{
}

bool UNonSpawnableItem::UseItem(APlayerCharacter* playerCharacter)
{
	return true;
}
