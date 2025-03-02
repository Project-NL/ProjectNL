// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipSlotWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "ProjectNL/Helper/ItemHelper.h"

void UEquipSlotWidget::SetupSlot(FItemMetaInfo* ItemMetaInfo)
{
	CurrentMetaInfoData=*ItemMetaInfo;
	CurrentItemData = FItemHelper::GetItemInfoById(GetWorld(), CurrentMetaInfoData.GetId());
	if (CurrentItemData.GetThumbnail().LoadSynchronous())
	{
		// 텍스처를 로드한 후 Image 위젯에 설정 (동기적으로 로드)
		UTexture2D* Texture = CurrentItemData.GetThumbnail().LoadSynchronous();
		if (Background && Texture)
		{
			Background->SetBrushFromTexture(Texture);
		}
	}

	// 아이템 개수를 표시 (개수가 1보다 크면 표시)
	if (ItemCountText)
	{
		Count = CurrentMetaInfoData.GetCurrentCount();
		ItemCountText->SetText(FText::AsNumber(Count));
	}
}
