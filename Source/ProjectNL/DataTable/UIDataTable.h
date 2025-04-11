// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "UIDataTable.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct  PROJECTNL_API FUIDataTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	
	FORCEINLINE const TMap<FGameplayTag, TSubclassOf<UUserWidget>>* GetDisplayName() const { return &UIMapping; }
	// 태그와 위젯 클래스를 매핑하는 테이블
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UI)
	TMap<FGameplayTag, TSubclassOf<UUserWidget>> UIMapping;

};
