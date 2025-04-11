// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProjectNL/DataTable/ItemInfoData.h"
#include "ItemPopUpWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTNL_API UItemPopUpWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetItemData(const FItemInfoData& ItemInfoData);
protected:
	// UMG 에디터에서 바인딩할 요소 (예: 아이템 이미지, 개수를 표시할 TextBlock)
	UPROPERTY(meta = (BindWidget))
	class UImage* Background;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Description;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemName;
	
	
};
