// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RestartMenuWidget.generated.h"

class UButton;
/**
 * 
 */
UCLASS()
class PROJECTNL_API URestartMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

protected:
	// 퀴트 버튼 (게임 종료)
	UPROPERTY(meta = (BindWidget))
	UButton* QuitButton;

	// 리스폰 버튼 (레벨 재시작)
	UPROPERTY(meta = (BindWidget))
	UButton* RespawnButton;

	UFUNCTION()
	void OnQuitButtonClicked();

	UFUNCTION()
	void OnRespawnButtonClicked();
};
