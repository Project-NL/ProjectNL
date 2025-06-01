// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectNL/UI/Widget/MainMenu/RestartMenuWidget.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

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
	// 현재 레벨 이름을 가져와서 다시 로드
	UWorld* World = GetWorld();
	if (!World) return;

	FString CurrentLevel = World->GetMapName();
	// MapName 에 접두사(UEDPIE_X_)가 붙는 경우 제거
	CurrentLevel.RemoveFromStart(World->StreamingLevelsPrefix);

	UGameplayStatics::OpenLevel(this, FName(*CurrentLevel));
}
