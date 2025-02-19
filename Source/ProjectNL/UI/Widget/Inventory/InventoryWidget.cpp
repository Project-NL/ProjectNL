#include "InventoryWidget.h"
#include "Components/UniformGridPanel.h"
#include "InventorySlotWidget.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerController.h"
#include "ProjectNL/Player/BasePlayerState.h"


void UInventoryWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (InventoryLabel)
	{
		InventoryLabel->SetText(InventoryTitle);
	}

	// 소유한 PlayerController에서 PlayerState를 가져옵니다.
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (ABasePlayerState* PS = PC->GetPlayerState<ABasePlayerState>())
		{
			PlayerState = PS;
		}
	}

	// PreConstruct 환경에서는 PlayerState를 가져오지 못함.
	// RefreshInventory();
}

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (InventoryLabel)
	{
		InventoryLabel->SetText(InventoryTitle);
	}

	// 소유한 PlayerController에서 PlayerState를 가져옵니다.
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (ABasePlayerState* PS = PC->GetPlayerState<ABasePlayerState>())
		{
			PlayerState = PS;
		}
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
	const TArray<FItemMetaInfo>& InventoryList = PlayerState->GetPlayerInventoryList();
		
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
		UInventorySlotWidget* SlotWidget = CreateWidget<UInventorySlotWidget>(this, InventorySlotWidgetClass);
		if (SlotWidget)
		{
			// 슬롯 위젯에 인덱스와 아이템 데이터를 전달하여 초기화합니다.
			if (Index < InventoryList.Num())
			{
				SlotWidget->SetupSlot(Index, InventoryList[Index]);
			}
			// 행과 열 계산 (Index를 이용)
			const int32 Row = Index / NumColumns;
			const int32 Column = Index % NumColumns;

			// 그리드에 슬롯 위젯 추가
			InventoryGrid->AddChildToUniformGrid(SlotWidget, Row, Column);
		}
	}
}
