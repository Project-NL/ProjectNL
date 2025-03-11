#include "InventoryWidget.h"
#include "Components/UniformGridPanel.h"
#include "InventorySlotWidget.h"
#include "ItemPopUpWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Overlay.h"
#include "Components/Overlay.h"
#include "Components/SizeBox.h"
#include "Components/Widget.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerController.h"
#include "ProjectNL/Character/Player/PlayerCharacter.h"
#include "ProjectNL/Component/InventoryComponent/EquipInventoryComponent.h"
#include "ProjectNL/Player/BasePlayerState.h"


void UInventoryWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	
}

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (InventoryLabel)
	{
		InventoryLabel->SetText(InventoryTitle);
	}
	//GetWorld()
	// 소유한 PlayerController에서 PlayerState를 가져옵니다.
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (ABasePlayerState* PS = PC->GetPlayerState<ABasePlayerState>())
		{
			PlayerState = PS;
		}
		APlayerCharacter* PlayerCharacter=Cast<APlayerCharacter>(PlayerState->GetPawn());
		if (PlayerCharacter)
		{
			EquipInventoryComponent=PlayerCharacter->GetEquipInventoryComponent();
		}
		ItemDescriptionPanel = CreateWidget<UItemPopUpWidget>(this, ExplainSlotWidgetClass);
	}
	

	RefreshInventory();
	
}

void UInventoryWidget::RefreshInventory()
{
	if ( !InventoryGrid)
	{
		return;
	}

	// 기존의 슬롯 위젯들을 지웁니다.
	InventoryGrid->ClearChildren();

	// PlayerState에서 인벤토리 리스트를 가져옵니다.
	
		switch (ItemType)
		{
		case EItemType::Consume:
			InventoryList = PlayerState->GetPlayerInventoryList();
			break;
		case EItemType::Weapon:
			InventoryList = EquipInventoryComponent->GetWeaponInventoryList();
			break;
		case EItemType::Armor:
			InventoryList = EquipInventoryComponent->GetArmorInventoryList();
			break;
		case EItemType::Accessory:
			InventoryList = EquipInventoryComponent->GetAccessoryInventoryList();
			break;
		}

	
		
	// 예시로 4열 그리드를 만든다고 가정합니다.
	const int32 NumColumns = 4;

	for (int32 Index = 0; Index < 20; ++Index)
	{
		if (!InventorySlotWidgetClass)
		{
			UE_LOG(LogTemp, Warning, TEXT("InventorySlotWidgetClass가 설정되지 않음"));
			continue;
		}

		// 슬롯 위젯을 생성합니다.
		USlotWidget* SlotWidget = CreateWidget<USlotWidget>(this, InventorySlotWidgetClass);
		if (SlotWidget)
		{
			// 슬롯 위젯에 인덱스와 아이템 데이터를 전달하여 초기화합니다.
			if (Index < InventoryList->Num())
			{
				SlotWidget->SetupSlot(Index, InventoryList,Index);
				SlotWidget->OnInventorySlotChanged.AddUObject(this, &UInventoryWidget::RefreshInventory);
				SlotWidget->OnItemHovered.AddUObject(this, &UInventoryWidget::UpdateItemDescription);
			}
			// 행과 열 계산 (Index를 이용)
			int32 Row = Index / NumColumns;
			int32 Column = Index % NumColumns;

			// 그리드에 슬롯 위젯 추가
			InventoryGrid->AddChildToUniformGrid(SlotWidget, Row, Column);
		}
	}
}
void UInventoryWidget::UpdateItemDescription(int32 SlotIndex, const FItemInfoData& ItemData,const FVector2D& MousePosition)
{
	
	if (!ItemDescriptionPanel)
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemDescriptionPanel is not set"));
		return;
	}

	if (SlotIndex >= 0 && ItemData.GetThumbnail()) // 유효한 슬롯과 아이템 데이터
	{
		// 설명창 생성 및 초기화 (이미 생성된 경우 재사용)
		if (!ItemDescriptionPanel->IsInViewport())
		{
			ItemDescriptionPanel->SetItemData(ItemData);
			ItemDescriptionPanel->AddToViewport();
		}

		// 크기 설정 (300x300 픽셀)
		ItemDescriptionPanel->SetDesiredSizeInViewport(FVector2D(500.0f, 600.0f));

		// 마우스 위치를 기준으로 설명창 위치 조정 (마우스 오른쪽에 표시, 화면 경계 체크)
		FVector2D ViewportSize;
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		FVector2D PopUpPosition = MousePosition + FVector2D(10.0f, 0.0f); // 마우스 오른쪽 10픽셀

		// 화면 경계 체크
		if (PopUpPosition.X + 300.0f > ViewportSize.X)
		{
			PopUpPosition.X = MousePosition.X +90.0f; // 마우스 왼쪽에 표시
		}
		if (PopUpPosition.Y + 200.0f > ViewportSize.Y)
		{
			PopUpPosition.Y = ViewportSize.Y - 200.0f; // 화면 아래로 고정
		}

		ItemDescriptionPanel->SetPositionInViewport(PopUpPosition);
		ItemDescriptionPanel->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		ItemDescriptionPanel->SetVisibility(ESlateVisibility::Hidden);
		if (ItemDescriptionPanel->IsInViewport())
		{
			ItemDescriptionPanel->RemoveFromParent(); // 필요 시 제거
		}
	}

}