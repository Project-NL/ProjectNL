#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "ProjectNL/DataTable/ItemInfoData.h"
#include "BasePlayerState.generated.h"

class UPlayerAttributeSet;
class UNLAbilitySystemComponent;

UCLASS()
class PROJECTNL_API ABasePlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	ABasePlayerState();

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ability System")
	TObjectPtr<UNLAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UPlayerAttributeSet> AttributeSet;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	

	void InitializeData();
	
	FORCEINLINE uint8 GetInventorySlotCount() const  { return InventorySlotCount; }
	FORCEINLINE uint8 GetHotSlotCount() const  { return HotSlotCount; }
	FORCEINLINE uint8 GetTotalSlotCount() const  { return HotSlotCount + InventorySlotCount; }
	FORCEINLINE TArray<FItemMetaInfo> GetPlayerInventoryList() const { return PlayerInventoryList; }
	
	uint32 AddItem(const FItemMetaInfo& ItemInfo);

	bool DropItem(const uint16 Index, const uint32 Count);
	
	bool RemoveItem(const uint16 Id, const uint32 Count);

	uint32 AddItemToInventory(const uint16 Index, const FItemMetaInfo& ItemInfo);

	void SwapItemInInventory(const uint16 Prev, const uint16 Next);

	void SetPlayerHandItemByPS(const uint16 NewIndex);

	FORCEINLINE TMap<uint32, uint32> GetCurrentRemainItemValue() const { return CurrentRemainItemValue; }

	bool HasItemInInventory(const uint32 Id, const uint32 Count);
	
private:
	TMap<uint32, uint32> CurrentRemainItemValue;
	void UpdateCurrentRemainItemValue();
	
	UPROPERTY(EditDefaultsOnly, Category = "Options", meta = (AllowPrivateAccess = true))
	TArray<uint32> InitialItemList;

	UPROPERTY()
	TArray<FItemMetaInfo> PlayerInventoryList;

	// TODO: 아래와 같은 설정은 추후 Data Asset으로 이전해보기
	UPROPERTY(EditDefaultsOnly, Category = "Options", meta = (AllowPrivateAccess = true))
	uint8 InventorySlotCount = 0;
	
	UPROPERTY(EditDefaultsOnly, Category = "Options", meta = (AllowPrivateAccess = true))
	uint8 HotSlotCount = 0;

	void OnUpdateInventory();
};
