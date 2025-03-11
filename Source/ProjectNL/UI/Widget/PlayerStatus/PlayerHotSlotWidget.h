// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHotSlotWidget.generated.h"

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
private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> WidgetClass;
	
	TArray<UUserWidget> HotSlots;

	
};
