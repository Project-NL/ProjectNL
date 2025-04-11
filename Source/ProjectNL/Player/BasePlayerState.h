#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "ProjectNL/DataTable/ItemInfoData.h"
#include "BasePlayerState.generated.h"

class UPlayerAttributeSet;
class UNLAbilitySystemComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHotSlotUpdated);
UCLASS()
class PROJECTNL_API ABasePlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	ABasePlayerState();

	virtual void BeginPlay() override;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ability System")
	TObjectPtr<UNLAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UPlayerAttributeSet> AttributeSet;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	
	void InitializeData();
	
	FORCEINLINE uint8 GetInventorySlotCount() const  { return InventorySlotCount; }
	FORCEINLINE uint8 GetHotSlotCount() const  { return HotSlotCount; }
	FORCEINLINE uint8 GetTotalSlotCount() const  { return InventorySlotCount; }
	FORCEINLINE TArray<FItemMetaInfo> *GetPlayerInventoryList()  { return &PlayerInventoryList; }
	FORCEINLINE TArray<FItemMetaInfo> *GetPlayerHotSlotList()  { return &PlayerHotSlotList; }
	FORCEINLINE TArray<int32> *GetHotslotInitialItemList()  { return &HotslotInitialItemList; }
	uint32 AddItem(const FItemMetaInfo& ItemInfo);

	//uint32 AddItem(const FItemMetaInfo& ItemInfo);

	bool DropItem(const uint16 Index, const uint32 Count);
	
	bool RemoveItem(const uint16 Id, const uint32 Count,int32 HotslotInit);

	uint32 AddItemToInventory(const uint16 Index, const FItemMetaInfo& ItemInfo);

	void SwapItemInInventory(const uint16 Prev, const uint16 Next);

	void SetPlayerHandItemByPS(const uint16 NewIndex);

	void SetPlayerHotSlot(const uint16 NewIndex);

	FORCEINLINE TMap<uint32, uint32> GetCurrentRemainItemValue() const { return CurrentRemainItemValue; }

	bool HasItemInInventory(const uint32 Id, const uint32 Count);

	
	// 인벤토리 변경 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnHotSlotUpdated OnHotSlotUpdatedDelegate;
private:
	TMap<uint32, uint32> CurrentRemainItemValue;
	void UpdateCurrentRemainItemValue();
	
	UPROPERTY(EditDefaultsOnly, Category = "Options", meta = (AllowPrivateAccess = true))
	TArray<uint32> InitialItemList;

	UPROPERTY(EditDefaultsOnly, Category = "Options", meta = (AllowPrivateAccess = true))
	TArray<int32> HotslotInitialItemList;
	
	UPROPERTY()
	TArray<FItemMetaInfo> PlayerInventoryList;//인벤토리 

	UPROPERTY()
	TArray<FItemMetaInfo> PlayerHotSlotList;//핫슬롯
	
	// TODO: 아래와 같은 설정은 추후 Data Asset으로 이전해보기
	UPROPERTY(EditDefaultsOnly, Category = "Options", meta = (AllowPrivateAccess = true))
	uint8 InventorySlotCount = 20;
	
	UPROPERTY(EditDefaultsOnly, Category = "Options", meta = (AllowPrivateAccess = true))
	uint8 HotSlotCount = 5;

	
	void OnUpdateInventory();
};

