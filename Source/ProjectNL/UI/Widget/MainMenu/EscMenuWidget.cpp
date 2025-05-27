#include "EscMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "GameFramework/PlayerController.h"

void UEscMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Resume 버튼 바인딩
	if (ResumeButton)
	{
		ResumeButton->OnClicked.AddDynamic(this, &UEscMenuWidget::OnResumeClicked);
	}

	// Exit 버튼 바인딩
	if (ExitButton)
	{
		ExitButton->OnClicked.AddDynamic(this, &UEscMenuWidget::OnExitClicked);
	}
}

void UEscMenuWidget::OnResumeClicked()
{
	// 메뉴 위젯 제거
	RemoveFromParent();

	// 입력 모드를 게임 전용으로 돌리고 마우스 커서 숨기기
	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->SetShowMouseCursor(false);

		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
	}
}

void UEscMenuWidget::OnExitClicked()
{
	// 게임 종료
	if (APlayerController* PC = GetOwningPlayer())
	{
		// 퀴트 다이얼로그 없이 즉시 종료
		UKismetSystemLibrary::QuitGame(this, PC, EQuitPreference::Quit, false);
	}
}
