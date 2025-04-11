#include "InventorySlotWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "ProjectNL/Character/Player/PlayerCharacter.h"
#include "ProjectNL/Component/EquipComponent/EquipComponent.h"
#include "ProjectNL/DataTable/ItemInfoData.h"
#include "ProjectNL/Helper/ItemHelper.h"
#include "ProjectNL/Player/BasePlayerState.h"

void UInventorySlotWidget::SetupSlot(int32 SlotIndex,TArray<FItemMetaInfo>* InventoryList,int32 index)
{
	// FItemHelper를 이용해 아이템의 정보를 가져옵니다.
	CurrentInventoryList=InventoryList;
	CurrentSlotIndex=index;
	CurrentMetaInfoData=(*CurrentInventoryList)[index];
	CurrentItemData = FItemHelper::GetItemInfoById(GetWorld(), CurrentMetaInfoData.GetId());
	// 썸네일 이미지 설정 (아이템 정보에 따라)
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

void UInventorySlotWidget::OnItemButtonPressed()
{
	// 현재 시간 가져오기
	float CurrentTime = GetWorld()->GetTimeSeconds();
	// 더블클릭 감지
	if (CurrentTime - LastPressTime <= DoubleClickThreshold)
	{
		EquipItem(); // 더블클릭 확인 시 장착
	}
	// 마지막 눌림 시간 업데이트
	LastPressTime = CurrentTime;
}

void UInventorySlotWidget::EquipItem()
{
	// 플레이어 캐릭터를 가져와서 아이템 장착 로직 호출
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(PlayerController->GetPawn()))
		{
			ABasePlayerState* BasePlayerState = Cast<ABasePlayerState>(PlayerCharacter->GetPlayerState());

			BasePlayerState->SetPlayerHotSlot(CurrentSlotIndex);
			BasePlayerState->OnHotSlotUpdatedDelegate.Broadcast();
		}
	}
}


void UInventorySlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 버튼 눌림 이벤트 바인딩
	if (ItemButton)
	{
		ItemButton->OnPressed.AddDynamic(this, &UInventorySlotWidget::OnItemButtonPressed);
	}
}
