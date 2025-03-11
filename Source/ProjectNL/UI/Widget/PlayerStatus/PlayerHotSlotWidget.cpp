// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHotSlotWidget.h"

#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"


void UPlayerHotSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!WidgetClass || !HotSlotGridPanel) return;

	for (int32 i = 0; i < 5; i++)
	{
		UUserWidget* NewSlot = CreateWidget<UUserWidget>(GetWorld(), WidgetClass);
		if (NewSlot)
		{
			HotSlotGridPanel->AddChildToUniformGrid(NewSlot, 0, i);
		}
	}
}
