// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProjectNL/DataTable/ItemInfoData.h"
#include "PlayerHotSlotWidget.generated.h"

class USlotWidget;
class ABasePlayerState;
/**
 * 
 */
UCLASS()
class PROJECTNL_API UPlayerHotSlotWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	UPROPERTY(meta = (BindWidget))
	class UUniformGridPanel* HotSlotGridPanel;
	UFUNCTION()
	void RefreshHotSlot();
private:
	
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<USlotWidget> HotSlotWidgetClass;
	
	TArray<UUserWidget> HotSlots;

	ABasePlayerState* PlayerState;

	TArray<FItemMetaInfo> *HotSlotList ;
	
};
