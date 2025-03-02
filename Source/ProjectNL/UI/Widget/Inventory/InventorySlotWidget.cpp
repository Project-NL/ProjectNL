#include "InventorySlotWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "ProjectNL/Character/Player/PlayerCharacter.h"
#include "ProjectNL/Component/EquipComponent/EquipComponent.h"
#include "ProjectNL/DataTable/ItemInfoData.h"
#include "ProjectNL/Helper/ItemHelper.h"

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
	// if (CurrentTime - LastPressTime <= DoubleClickThreshold)
	// {
	// 	if (Count > 0)
	// 	{
	// 		if (CurrentItemData.GetItemType()==EItemType::Accessory||
	// 			CurrentItemData.GetItemType()==EItemType::Armor||
	// 			CurrentItemData.GetItemType()==EItemType::Weapon)//장비 일 때 
	// 		{
	// 			EquipItem(); // 더블클릭 확인 시 장착
	// 		}
	// 	}
	// }

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
			UEquipComponent* EquipComponent = PlayerCharacter->GetEquipComponent();
			// 아이템 장착 로직
			EquipComponent->EquipWeapon(CurrentItemData.GetShowItemActor(), true);
			//UE_LOG(LogTemp, Log, TEXT("Item equipped: %s from slot %d via double-click"), CurrentItemData.GetShowItemActor().Get(), CurrentSlotIndex);
			// 델리게이트 브로드캐스트
			
		}
	}
	Count--;
	if (Count==0)
	{
		FItemMetaInfo ItemInfoData{};
		CurrentMetaInfoData=ItemInfoData;//초기화 시킨다.
		//CurrentInventoryList->RemoveAt(CurrentSlotIndex);
		(*CurrentInventoryList)[CurrentSlotIndex]=ItemInfoData;
		OnInventorySlotChanged.Broadcast();
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
