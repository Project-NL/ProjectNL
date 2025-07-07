// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectNL/UI/Widget/MainMenu/RestartMenuWidget.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProjectNL/Helper/GameplayTagHelper.h"
#include "ProjectNL/UI/Manager/UIManager.h"

void URestartMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (QuitButton)
	{
		QuitButton->OnClicked.AddDynamic(this, &URestartMenuWidget::OnQuitButtonClicked);
	}
	if (RespawnButton)
	{
		RespawnButton->OnClicked.AddDynamic(this, &URestartMenuWidget::OnRespawnButtonClicked);
	}
}

void URestartMenuWidget::OnQuitButtonClicked()
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		// 퀴트 다이얼로그 없이 즉시 종료
		UKismetSystemLibrary::QuitGame(this, PC, EQuitPreference::Quit, false);
	}
}

void URestartMenuWidget::OnRespawnButtonClicked()
{
	if (APlayerController* PC = GetOwningPlayer())          // 위젯을 띄웠던 컨트롤러
	{
		// 1) UI 전용 → 게임 전용으로 복구
		PC->SetInputMode(FInputModeGameOnly());
		PC->bShowMouseCursor = false;

		// 혹시 메뉴 열 때 Look/Move 입력을 막았다면 다시 풀기
		PC->SetIgnoreLookInput(false);
		PC->SetIgnoreMoveInput(false);
	}

	RemoveFromParent();   // 위젯 닫기

	// 2) 레벨 다시 로드
	UWorld* World = GetWorld();
	if (!World) return;

	FString CurrentLevel = World->GetMapName();
	CurrentLevel.RemoveFromStart(World->StreamingLevelsPrefix);
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UUIManager* UIManager = GameInstance->GetSubsystem<UUIManager>();
		UIManager->Deinitialize();
	}
	UGameplayStatics::OpenLevel(this, FName(*CurrentLevel));
}