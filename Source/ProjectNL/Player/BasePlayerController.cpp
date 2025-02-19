#include "BasePlayerController.h"

#include "EnhancedInputComponent.h"
#include "Blueprint/UserWidget.h"
#include "ProjectNL/UI/Widget/Inventory/InventoryWidget.h"
#include "ProjectNL/UI/Widget/PlayerStatus/PlayerStatus.h"

void ABasePlayerController::BeginPlay()
{
	Super::BeginPlay();
	ConsoleCommand(TEXT("showdebug abilitysystem"));
}

void ABasePlayerController::BeginPlayingState()
{
	Super::BeginPlayingState();

	if (PlayerStatus)
	{
		return;
	}

	if (!PlayerStatusHUDClass)
	{
		return;
	}
	
	if (!IsLocalPlayerController())
	{
		return;
	}
	
	PlayerStatus = CreateWidget<UPlayerStatus>(this, PlayerStatusHUDClass);
	PlayerStatus->AddToViewport();
}

void ABasePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// ToggleInventoryAction이 유효하다면 바인딩합니다.
		if (ToggleInventoryAction)
		{
			EnhancedInputComponent->BindAction(
				ToggleInventoryAction, 
				ETriggerEvent::Triggered, 
				this, 
				&ABasePlayerController::ToggleInventoryWidget
			);
		}
	}

	// Enhanced Input 시스템을 사용하는 경우, 보통 로컬 플레이어 서브시스템에 InputMappingContext를 추가합니다.
	// 예시 코드:
	// if (APlayerController* PC = GetLocalPlayerController())
	// {
	// 	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
	// 	{
	// 		// 여기서 인벤토리 토글 액션이 포함된 매핑 컨텍스트를 추가할 수 있습니다.
	// 		// Subsystem->AddMappingContext(MyMappingContext, 0);
	// 	}
	// }
}

void ABasePlayerController::ToggleInventoryWidget()
{
	if (InventoryWidget)
	{
		// 인벤토리 위젯이 이미 열려 있다면, 제거하여 숨깁니다.
		InventoryWidget->RemoveFromParent();
		InventoryWidget = nullptr;
		// 게임 모드로 입력 전환: 마우스 커서 숨김
		FInputModeGameOnly GameInputMode;
		SetInputMode(GameInputMode);
		bShowMouseCursor = false;
	}
	else
	{
		// 인벤토리 위젯이 없다면, 생성 후 뷰포트에 추가합니다.
		if (InventoryWidgetClass)
		{
			InventoryWidget = CreateWidget<UUserWidget>(this, InventoryWidgetClass);
			if (InventoryWidget)
			{
				InventoryWidget->AddToViewport();
				// UI 모드로 입력 전환: 마우스 커서를 보이게 하고, 입력은 UI로만 전달
				FInputModeGameAndUI InputMode;
				// 포커스를 인벤토리 위젯에 설정합니다.
				//UIInputMode.SetWidgetToFocus(nullptr);
				//UIInputMode.SetWidgetToFocus(InventoryWidget->TakeWidget());
				// 마우스 커서를 뷰포트에 고정하지 않음
				InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
				SetInputMode(InputMode);
				bShowMouseCursor = true;
			}
		}
	}
}
