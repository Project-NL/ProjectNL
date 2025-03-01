// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipInventoryWidget.h"

#include "EquipSlotWidget.h"
#include "ProjectNL/Character/Player/PlayerCharacter.h"
#include "ProjectNL/Component/EquipComponent/EquipComponent.h"
#include "ProjectNL/Player/BasePlayerState.h"
#include "ProjectNL/Weapon/BaseWeapon.h"

void UEquipInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	RefreshInventory();
}

void UEquipInventoryWidget::RefreshInventory()
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (ABasePlayerState* PS = PC->GetPlayerState<ABasePlayerState>())
		{
			APlayerCharacter* PlayerCharacter=Cast<APlayerCharacter>(PS->GetPawn());
			if (PlayerCharacter)
			{
				UEquipComponent* EquipComponent= PlayerCharacter->GetEquipComponent();
				EquipComponent->EquipInventorySlotChangedDelegate.AddUObject(this, &UEquipInventoryWidget::RefreshInventory);
				ABaseWeapon* MainWeapon=EquipComponent->GetMainWeapon();
				WeaponSlot->SetupSlot(MainWeapon->GetItemMetainfo());
			}
		}
	}
}
	
