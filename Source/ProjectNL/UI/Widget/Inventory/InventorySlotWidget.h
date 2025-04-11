// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SlotWidget.h"
#include "Blueprint/UserWidget.h"
#include "ProjectNL/DataTable/ItemInfoData.h"
#include "InventorySlotWidget.generated.h"

/**
 * 
 */  
UCLASS()
class PROJECTNL_API UInventorySlotWidget : public USlotWidget
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

	
protected:
	// UMG 에디터에서 바인딩할 요소 (예: 아이템 이미지, 개수를 표시할 TextBlock)

	void EquipItem();

};
