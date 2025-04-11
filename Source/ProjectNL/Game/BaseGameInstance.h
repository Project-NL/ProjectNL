#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ProjectNL/DataTable/ItemInfoData.h"
#include "BaseGameInstance.generated.h"

UCLASS()
class PROJECTNL_API UBaseGameInstance : public UGameInstance
{
	GENERATED_BODY()

	UBaseGameInstance();

public:
    const TArray<FItemInfoData>& GetItemInfoList();
	virtual void Init() override;

private:
	UPROPERTY()
	TArray<FItemInfoData> ItemInfoList;
	
	void LoadItemData();
};
