#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProjectNL/DataTable/ItemInfoData.h"
#include "InventoryWidget.generated.h"

class UItemPopUpWidget;
class USizeBox;
class UOverlay;
class USlotWidget;
class UCanvasPanel;
class UEquipInventoryComponent;
// 전방 선언
class UUniformGridPanel;
class UInventorySlotWidget;
class UTextBlock;
class ABasePlayerState;


UCLASS()
class PROJECTNL_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

	// 인벤토리가 업데이트될 때 호출할 함수
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RefreshInventory();
	void UpdateItemDescription(int32 SlotIndex, const FItemInfoData& ItemData, const FVector2D& MousePosition);

	// Blueprint에서 인벤토리 제목을 수정할 수 있게 만듭니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FText InventoryTitle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	EItemType ItemType;

protected:
	// UMG 에디터에서 Grid Panel과 바인딩할 변수
	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* InventoryGrid;

	// UMG 에디터에서 인벤토리 제목을 표시할 TextBlock
	UPROPERTY(meta = (BindWidget))
	UTextBlock* InventoryLabel;
	
	//UPROPERTY(meta = (BindWidget))
	//UCanvasPanel* ItemPopUpCanvas;

	// 각 인벤토리 슬롯에 사용될 위젯 클래스. 에디터에서 지정할 수 있음.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TSubclassOf<USlotWidget> InventorySlotWidgetClass;

	// 각 인벤토리 슬롯에 사용될 위젯 클래스. 에디터에서 지정할 수 있음.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TSubclassOf<UUserWidget> ExplainSlotWidgetClass;


	UItemPopUpWidget* ItemDescriptionPanel;
private:
	// 인벤토리 데이터를 가지고 있는 PlayerState를 캐싱
	ABasePlayerState* PlayerState;
	UEquipInventoryComponent* EquipInventoryComponent;

	TArray<FItemMetaInfo> *InventoryList ;
};
