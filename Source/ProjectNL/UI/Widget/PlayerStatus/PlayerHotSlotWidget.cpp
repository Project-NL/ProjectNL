// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHotSlotWidget.h"

#include "HotSlotWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "ProjectNL/Player/BasePlayerState.h"
#include "ProjectNL/UI/Widget/Inventory/SlotWidget.h"


void UPlayerHotSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!HotSlotWidgetClass || !HotSlotGridPanel) return;
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (ABasePlayerState* PS = PC->GetPlayerState<ABasePlayerState>())
		{
			PlayerState = PS;
			PlayerState->OnHotSlotUpdatedDelegate.AddDynamic(this, &UPlayerHotSlotWidget::RefreshHotSlot);
		}
	}
	RefreshHotSlot();
}

void UPlayerHotSlotWidget::RefreshHotSlot()
{
	// 기존 슬롯 제거
	if (HotSlotGridPanel)
	{
		HotSlotGridPanel->ClearChildren();
	}

	// PlayerState에서 인벤토리 리스트를 가져옵니다.
	HotSlotList = PlayerState->GetPlayerHotSlotList();
	TArray<int32>* HotslotInitialItemList = PlayerState->GetHotslotInitialItemList();
	TArray<FItemMetaInfo>* InventoryItemList = PlayerState->GetPlayerInventoryList();

	if (!HotSlotList || !HotslotInitialItemList || !InventoryItemList)
	{
		return;
	}

	if (!HotSlotWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("HotSlotWidgetClass가 설정되지 않음"));
		return;
	}

	// 슬롯 위젯을 생성합니다.
	for (int32 index = 0; index < HotslotInitialItemList->Num(); index++)
	{
		UHotSlotWidget* NewSlot = CreateWidget<UHotSlotWidget>(GetWorld(), HotSlotWidgetClass);
		int32 HotslotInitial = (*HotslotInitialItemList)[index];
		if (NewSlot)
		{
			if (HotslotInitial >= 0 && InventoryItemList->IsValidIndex(HotslotInitial))
			{
				NewSlot->SetupSlot(&(*InventoryItemList)[HotslotInitial], index);
			}
			NewSlot->OnInventorySlotChanged.AddUObject(this, &UPlayerHotSlotWidget::RefreshHotSlot);
			HotSlotGridPanel->AddChildToUniformGrid(NewSlot, 0, index);
		}
	}
}

