// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectNL/UI/Widget/Inventory/SlotWidget.h"
#include "HotSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTNL_API UHotSlotWidget : public USlotWidget
{
	GENERATED_BODY()
public:
	void SetupSlot(FItemMetaInfo* i1temMetaInfo, int32 index);
};
