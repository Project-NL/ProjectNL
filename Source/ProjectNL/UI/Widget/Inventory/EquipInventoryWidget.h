// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EquipInventoryWidget.generated.h"

class UEquipSlotWidget;
/**
 * 
 */

UCLASS()
class PROJECTNL_API UEquipInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
	virtual void NativeConstruct() override;
	void RefreshInventory();

private:

	UPROPERTY(meta = (BindWidget))
	UEquipSlotWidget* WeaponSlot;

	UPROPERTY(meta = (BindWidget))
	UEquipSlotWidget* ArmorSlot;

	UPROPERTY(meta = (BindWidget))
	UEquipSlotWidget* AccesarySlot;
};
