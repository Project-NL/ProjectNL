// Fill out your copyright notice in the Description page of Project Settings.


#include "SlotWidget.h"


void USlotWidget::SetupSlot(int32 SlotIndex, TArray<FItemMetaInfo>* InventoryList, int32 index)
{
}

void USlotWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	if (CurrentItemData.GetThumbnail()) // 데이터 유효성 체크
	{
		FVector2D MousePosition = InMouseEvent.GetScreenSpacePosition();
		OnItemHovered.Broadcast(CurrentSlotIndex, CurrentItemData, MousePosition);

		UE_LOG(LogTemp, Warning, TEXT("NativeOnMouseEnter is Change"));
	}
}

void USlotWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	OnItemDescriptionHide.Broadcast();
	UE_LOG(LogTemp, Warning, TEXT("NativeOnMouseEnter is not Change"));
}
