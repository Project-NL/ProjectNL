// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTNL_API UInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 슬롯을 초기화하는 함수
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SetupSlot(int32 SlotIndex, const FItemMetaInfo& ItemData);

protected:
	// UMG 에디터에서 바인딩할 요소 (예: 아이템 이미지, 개수를 표시할 TextBlock)
	UPROPERTY(meta = (BindWidget))
	class UImage* Background;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemCountText;
};
