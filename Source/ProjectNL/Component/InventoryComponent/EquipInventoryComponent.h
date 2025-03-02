// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProjectNL/DataTable/ItemInfoData.h"
#include "EquipInventoryComponent.generated.h"


struct FItemMetaInfo;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTNL_API UEquipInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEquipInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void InitializeData();
	
	FORCEINLINE uint8 GetInventorySlotCount() const  { return InventorySlotCount; }
	FORCEINLINE uint8 GetTotalSlotCount() const  { return InventorySlotCount; }
	FORCEINLINE TArray<FItemMetaInfo> *GetWeaponInventoryList()  { return &WeaponInventoryList; }
	FORCEINLINE TArray<FItemMetaInfo> *GetArmorInventoryList()  { return &ArmorInventoryList; }
	FORCEINLINE TArray<FItemMetaInfo> *GetAccessoryInventoryList()  { return &AccessoryInventoryList; }
	
	uint32 AddItemMeta(const FItemMetaInfo& ItemInfo);

	uint32 AddItem(TArray<FItemMetaInfo>& PlayerInventoryList,const FItemMetaInfo& ItemInfo);
	uint32 RemoveItemMeta(const FItemMetaInfo& ItemInfo);

	bool DropItem(TArray<FItemMetaInfo>& PlayerInventoryList, uint16 Index, uint32 Count);
	
	bool RemoveItem(TArray<FItemMetaInfo>& PlayerInventoryList,const uint16 Id, const uint32 Count);

	uint32 AddItemToInventory(const uint16 Index, const FItemMetaInfo& ItemInfo);
	
	uint32 AddEquipmentToInventory(TArray<FItemMetaInfo>& PlayerInventoryList, uint16 Index, const FItemMetaInfo& ItemInfo, const FItemInfoData&
	                               ItemInfoById);

	void SwapItemInInventory(const uint16 Prev, const uint16 Next);

	void SwapItemToEquipItem(const uint16 Prev, const uint16 Next);

	void SetPlayerHandItemByPS(const uint16 NewIndex);

	FORCEINLINE TMap<uint32, uint32> GetCurrentRemainItemValue() const { return CurrentRemainItemValue; }

	bool HasItemInInventory(const uint32 Id, const uint32 Count);
	
private:
	TMap<uint32, uint32> CurrentRemainItemValue;
	void UpdateCurrentRemainItemValue(TArray<FItemMetaInfo>& PlayerInventoryList);
	
	UPROPERTY(EditDefaultsOnly, Category = "Options", meta = (AllowPrivateAccess = true))
	TArray<uint32> InitialItemList;

	UPROPERTY()
	TArray<FItemMetaInfo> WeaponInventoryList;//무기 인벤토리

	UPROPERTY()
	TArray<FItemMetaInfo> ArmorInventoryList;//방어구 인벤토리

	UPROPERTY()
	TArray<FItemMetaInfo> AccessoryInventoryList;//악세서리 인벤토리
	
	

	// TODO: 아래와 같은 설정은 추후 Data Asset으로 이전해보기
	UPROPERTY(EditDefaultsOnly, Category = "Options", meta = (AllowPrivateAccess = true))
	uint8 InventorySlotCount = 10;
	

	void OnUpdateInventory(TArray<FItemMetaInfo>& PlayerInventoryList);
};
