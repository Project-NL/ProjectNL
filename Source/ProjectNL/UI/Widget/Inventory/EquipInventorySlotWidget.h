// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SlotWidget.h"
#include "Blueprint/UserWidget.h"
#include "ProjectNL/DataTable/ItemInfoData.h"
#include "EquipInventorySlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTNL_API UEquipInventorySlotWidget : public USlotWidget
{
	GENERATED_BODY()

public:
	// 슬롯을 초기화하는 함수
	//UFUNCTION()
	virtual void SetupSlot(int32 SlotIndex, TArray<FItemMetaInfo>* InventoryList,int32 index) override;

	// 버튼 눌림 시 호출될 함수
	UFUNCTION()
	void OnItemButtonPressed();

	// 초기화 후 호출되는 네이티브 함수
	virtual void NativeConstruct() override;

	FOnInventorySlotChanged OnInventorySlotChanged;
protected:
	// UMG 에디터에서 바인딩할 요소 (예: 아이템 이미지, 개수를 표시할 TextBlock)
	UPROPERTY(meta = (BindWidget))
	class UImage* Background;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemCountText;

	// 새로 추가된 버튼
	UPROPERTY(meta = (BindWidget))
	class UButton* ItemButton;

private:
	
	void EquipItem();
	
	
	
};
