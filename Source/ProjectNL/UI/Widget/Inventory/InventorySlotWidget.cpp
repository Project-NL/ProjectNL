#include "InventorySlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "ProjectNL/DataTable/ItemInfoData.h"
#include "ProjectNL/Helper/ItemHelper.h"

void UInventorySlotWidget::SetupSlot(int32 SlotIndex, const FItemMetaInfo& ItemData)
{
	// FItemHelper를 이용해 아이템의 정보를 가져옵니다.
	
	const FItemInfoData& ItemInfo = FItemHelper::GetItemInfoById(GetWorld(), ItemData.GetId());
	// 썸네일 이미지 설정 (아이템 정보에 따라)
	if (ItemInfo.GetThumbnail().IsValid())
	{
		// 텍스처를 로드한 후 Image 위젯에 설정 (동기적으로 로드)
		UTexture2D* Texture = ItemInfo.GetThumbnail().LoadSynchronous();
		if (Background && Texture)
		{
			Background->SetBrushFromTexture(Texture);
		}
	}

	// 아이템 개수를 표시 (개수가 1보다 크면 표시)
	if (ItemCountText)
	{
		const int32 Count = ItemData.GetCurrentCount();
		ItemCountText->SetText(FText::AsNumber(Count));
	}
}
