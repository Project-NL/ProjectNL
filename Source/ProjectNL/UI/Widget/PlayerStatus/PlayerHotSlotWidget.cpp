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
	// PlayerState에서 인벤토리 리스트를 가져옵니다.

	HotSlotList = PlayerState->GetPlayerHotSlotList();


	// 예시로 4열 그리드를 만든다고 가정합니다.
	if (!HotSlotList)
	{
		return;
	}

	if (!HotSlotWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventorySlotWidgetClass가 설정되지 않음"));
	}

	// 슬롯 위젯을 생성합니다.

	for (int32 index = 0; index < 5; index++)
	{
		UHotSlotWidget* NewSlot = CreateWidget<UHotSlotWidget>(GetWorld(), HotSlotWidgetClass);
		if (NewSlot)
		{
		
			if (index < HotSlotList->Num())
			{
				NewSlot->SetupSlot(&(*HotSlotList)[index], index);
				NewSlot->OnInventorySlotChanged.AddUObject(this, &UPlayerHotSlotWidget::RefreshHotSlot);
			}
			HotSlotGridPanel->AddChildToUniformGrid(NewSlot, 0, index);
		}
	}
}
