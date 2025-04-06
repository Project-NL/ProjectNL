// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h" 
#include "UIManager.generated.h"
class UUserWidget;
struct FGameplayTag;
/**
 * 
 */

UCLASS(Blueprintable)
class PROJECTNL_API UUIManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	UUIManager();
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "UI Manager")
	void ShowUI(FGameplayTag UITag);

	UFUNCTION(BlueprintCallable, Category = "UI Manager")
	void HideUI(FGameplayTag UITag);

	UFUNCTION(BlueprintCallable, Category = "UI Manager")
	void ToggleUI(FGameplayTag UITag); 

	// UI가 활성화되어 있는지 확인
	UFUNCTION(BlueprintCallable, Category = "UI Manager")
	bool IsUIActive(FGameplayTag UITag) const;


private:
	// 태그와 위젯 클래스를 매핑하는 테이블
	UPROPERTY(EditAnywhere, Category = "UI Manager")
	TMap<FGameplayTag, TSubclassOf<UUserWidget>> UIMapping;

	// 현재 활성화된 위젯들
	UPROPERTY()
	TMap<FGameplayTag, UUserWidget*> ActiveWidgets;

	// UI를 생성하고 뷰포트에 추가하는 헬퍼 함수
	UUserWidget* CreateWidgetForTag(FGameplayTag UITag);

};
