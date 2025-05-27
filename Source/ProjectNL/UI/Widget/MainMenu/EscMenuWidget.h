// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EscMenuWidget.generated.h"

class UButton;

UCLASS()
class PROJECTNL_API UEscMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	// UMG 에디터에서 BindWidget 으로 지정된 버튼 포인터
	UPROPERTY(meta = (BindWidget))
	UButton* ResumeButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ExitButton;

	// 위젯이 생성된 직후 호출
	virtual void NativeConstruct() override;

	// 버튼 클릭 핸들러
	UFUNCTION()
	void OnResumeClicked();

	UFUNCTION()
	void OnExitClicked();
};
