// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemAcquireTextWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTNL_API UItemAcquireTextWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
};
