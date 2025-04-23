// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

/**
 * 
 */
class UButton;
class UEditableTextBox;
UCLASS()
class PROJECTNL_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
		
public:
	void NativeOnInitialized();
	// 서버 생성
	UFUNCTION()
	void HostServer();
	UFUNCTION()
	void OnJoinButtonClicked();

	// 서버 접속
	UFUNCTION()
	void JoinServer(const FString& IPAddress);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> StartButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> joinButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> IPAddressInput;
};
