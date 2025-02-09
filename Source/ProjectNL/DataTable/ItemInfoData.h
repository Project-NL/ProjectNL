#pragma once

#include "CoreMinimal.h"
#include "ItemInfoData.generated.h"

UENUM()
enum class EItemType : uint32
{
	Undefined,
	Interactive,
	Equipment,
	Consume,
	Ingredient,
	Build,
	Max,
};


UENUM()
enum class EMetaDataKey : uint32
{
	None,
	Durability,
};

UENUM()
enum class EOptionDataKey : uint32
{
	None,
	Damage,
	AttachSocket,
	CookedTo,
};

// 아이템 정보를 담아 추후 아이템을 구성할 때 사용할 요소
USTRUCT(BlueprintType)
struct PROJECT_02_API FItemInfoData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
	
	FORCEINLINE FString GetDisplayName() const { return DisplayName; }
	FORCEINLINE EItemType GetItemType() const { return ItemType; }
	FORCEINLINE TSoftObjectPtr<UTexture2D> GetThumbnail() const { return Thumbnail; }
	FORCEINLINE TSubclassOf<AActor> GetShowItemActor() const { return ShowItemActor; }
	FORCEINLINE int GetMaxItemCount() const { return MaxItemCount; }
	FORCEINLINE TMap<EMetaDataKey, FString> GetMetaData() const { return MetaData; }
	FORCEINLINE TMap<EOptionDataKey, FString> GetOptionData() const { return OptionData; }
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Data", meta=(AllowPrivateAccess = true));
	FString DisplayName;
	
	UPROPERTY(EditDefaultsOnly, Category = "Data", meta=(AllowPrivateAccess = true));
	EItemType ItemType = EItemType::Undefined;
	
	UPROPERTY(EditDefaultsOnly, Category = "Data", meta=(AllowPrivateAccess = true));
	TSoftObjectPtr<UTexture2D> Thumbnail;

	// 드롭될 때 나올 아이템 액터 정보
	// TODO: 액터는 추후 아이템 액터 공통화로 처리한다.
	UPROPERTY(EditDefaultsOnly, Category = "Data", meta=(AllowPrivateAccess = true));
	TSubclassOf<AActor> ShowItemActor;
	
	UPROPERTY(EditDefaultsOnly, Category = "Data", meta=(AllowPrivateAccess = true));
	TSubclassOf<AActor> DroppedItemActor;
	
	UPROPERTY(EditDefaultsOnly, Category = "Data", meta = (AllowPrivateAccess = true,
		ClampMin = 0, ClampMax = 1000, UIMin = 0, UIMax = 1000));
	int MaxItemCount = 0;
	
	UPROPERTY(EditDefaultsOnly, Category = "Data", meta=(AllowPrivateAccess = true));
	TMap<EMetaDataKey, FString> MetaData;
	
	UPROPERTY(EditDefaultsOnly, Category = "Data", meta=(AllowPrivateAccess = true));
	TMap<EOptionDataKey, FString> OptionData;
};

// 실제 플레이어가 저장할 정보 값
USTRUCT(BlueprintType)
struct PROJECT_02_API FItemMetaInfo
{
	GENERATED_USTRUCT_BODY()
	
	FORCEINLINE int GetId() const { return Id; }
	FORCEINLINE int GetCurrentCount() const { return CurrentCount; }
	FORCEINLINE TMap<EMetaDataKey, FString> GetMetaData() const { return MetaData; }
	
	FORCEINLINE void SetId(const int NewValue) { Id = NewValue; }
	FORCEINLINE void SetCurrentCount(const int NewValue) { CurrentCount = NewValue; }
	
	FORCEINLINE void SetMetaData(const TMap<EMetaDataKey, FString>& NewMeta) { MetaData = NewMeta; }

	bool operator == (const FItemMetaInfo& CompareItem) const
	{
		return CompareItem.GetId() == GetId();
	}
	
private:
	// 0인 경우 아이템이 안들어가게 처리해야함.
	UPROPERTY(EditDefaultsOnly, Category = "Data", meta =
		(ClampMin = 0, ClampMax = 1000, UIMin = 0, UIMax = 1000));
	int Id = 0;
	
	UPROPERTY(EditDefaultsOnly, Category = "Data", meta =
		(ClampMin = 0, ClampMax = 1000, UIMin = 0, UIMax = 1000));
	int CurrentCount = 0;
	
	UPROPERTY(EditDefaultsOnly, Category = "Data");
	TMap<EMetaDataKey, FString> MetaData;
};