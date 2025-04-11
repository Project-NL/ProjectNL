// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SlotWidget.h"
#include "Blueprint/UserWidget.h"
#include "EquipSlotWidget.generated.h"

class USlotWidget;
/**
 * 
 */
UCLASS()
class PROJECTNL_API UEquipSlotWidget : public USlotWidget
{
	GENERATED_BODY()
public:
	void SetupSlot(FItemMetaInfo* InventoryList) ;

	
};
