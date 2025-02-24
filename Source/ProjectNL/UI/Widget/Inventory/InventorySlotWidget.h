// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProjectNL/DataTable/ItemInfoData.h"
#include "InventorySlotWidget.generated.h"

/**
 * 
 */   DECLARE_MULTICAST_DELEGATE(FOnInventorySlotChanged);
UCLASS()
class PROJECTNL_API UInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 슬롯을 초기화하는 함수
	//UFUNCTION()
	void SetupSlot(int32 SlotIndex, TArray<FItemMetaInfo>* InventoryList,int32 index);

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
	// 슬롯 인덱스와 아이템 데이터를 저장
	int32 CurrentSlotIndex;
	FItemInfoData CurrentItemData;
	FItemMetaInfo CurrentMetaInfoData;
	TArray<FItemMetaInfo>* CurrentInventoryList;
	// 더블클릭 감지를 위한 변수
	float LastPressTime = 0.0f;
	const float DoubleClickThreshold = 0.3f; // 더블클릭 간격 (초 단위)

	// 실제 장착 로직
	void EquipItem();

	int32 Count;
	
	

};
