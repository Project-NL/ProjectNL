// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemPopUpWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "ProjectNL/DataTable/ItemInfoData.h"

void UItemPopUpWidget::SetItemData(const FItemInfoData& ItemInfoData)
{
	if (ItemInfoData.GetThumbnail().LoadSynchronous())
	{
		UTexture2D* Texture = ItemInfoData.GetThumbnail().LoadSynchronous();
		if (Background && Texture)
		{
			Background->SetBrushFromTexture(Texture);
		}
	}
	//FString DisplayName = ItemInfoData.GetDisplayName();
//	UE_LOG(LogTemp, Log, TEXT("DisplayName: %s"), *DisplayName);
	ItemName->SetText(ItemInfoData.GetDisplayName());
}
	
