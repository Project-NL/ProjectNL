// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProjectNL/DataTable/ItemInfoData.h"
#include "SlotWidget.generated.h"

/**
 * 
 */
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnItemHovered, int32 /*SlotIndex*/, const FItemInfoData& /*ItemData*/, const FVector2D& /*MousePosition*/);
DECLARE_MULTICAST_DELEGATE(FOnItemDescriptionHide);
DECLARE_MULTICAST_DELEGATE(FOnInventorySlotChanged);
UCLASS()
class PROJECTNL_API USlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	FOnInventorySlotChanged OnInventorySlotChanged;

	// 델리게이트: 아이템 설명창 표시/숨김 요청

	FOnItemHovered OnItemHovered;
	FOnItemDescriptionHide OnItemDescriptionHide;
	virtual void SetupSlot(int32 SlotIndex, TArray<FItemMetaInfo>* InventoryList,int32 index);

	// 마우스 이벤트 오버라이드
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	
protected:
	// 슬롯 인덱스와 아이템 데이터를 저장
	int32 CurrentSlotIndex;
	FItemInfoData CurrentItemData;
	FItemMetaInfo CurrentMetaInfoData;
	TArray<FItemMetaInfo>* CurrentInventoryList;
	// 더블클릭 감지를 위한 변수
	float LastPressTime = 0.0f;
	const float DoubleClickThreshold = 0.3f; // 더블클릭 간격 (초 단위)
	
	int32 Count;
	int32 HotSlotCount;

	// UMG 에디터에서 바인딩할 요소 (예: 아이템 이미지, 개수를 표시할 TextBlock)
	UPROPERTY(meta = (BindWidget))
	class UImage* Background;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemCountText;

	// 새로 추가된 버튼
	UPROPERTY(meta = (BindWidget))
	class UButton* ItemButton;

};
