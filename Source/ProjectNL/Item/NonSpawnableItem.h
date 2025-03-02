// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectNL/DataTable/ItemInfoData.h"
#include "ProjectNL/Interface/InteractableInterface.h"
#include "ProjectNL/Interface/ItemInterface.h"
#include "UObject/NoExportTypes.h"
#include "NonSpawnableItem.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTNL_API UNonSpawnableItem : public UObject,public IItemInterface,public IInteractableInterface
{
	GENERATED_BODY()

public:
	virtual void Interact(AActor* Actor) override;

	virtual void UseItem() override;

private:
	//아이템 데이터
	UPROPERTY(EditAnywhere)
	FItemMetaInfo ItemMetaInfo;
};

