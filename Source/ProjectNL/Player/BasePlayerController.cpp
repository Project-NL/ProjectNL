#include "BasePlayerController.h"

#include "BasePlayerState.h"
#include "EnhancedInputComponent.h"
#include "Blueprint/UserWidget.h"
#include "ProjectNL/Character/Player/PlayerCharacter.h"
#include "ProjectNL/Helper/GameplayTagHelper.h"
#include "ProjectNL/Helper/ItemHelper.h"
#include "ProjectNL/Item/SpawnableItem.h"
#include "ProjectNL/UI/Widget/Inventory/InventoryWidget.h"
#include "ProjectNL/UI/Widget/PlayerStatus/PlayerStatus.h"
#include "ProjectNL/UI/Manager/UIManager.h"


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

		
	PlayerHotslot = CreateWidget<UUserWidget>(this, PlayerHotslotHUDClass);
	PlayerHotslot->AddToViewport();

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UIManager = GameInstance->GetSubsystem<UUIManager>();
	}
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
		if (AcquireSpawnItem)
		{
			EnhancedInputComponent->BindAction(
				AcquireSpawnItem, 
				ETriggerEvent::Triggered, 
				this, 
				&ABasePlayerController::TryInteract
			);
		}
		if (ToggleFirstHotSlotItem)
		{
			EnhancedInputComponent->BindAction(
				ToggleFirstHotSlotItem, 
				ETriggerEvent::Triggered, 
				this, 
				&ABasePlayerController::UseFirstHotSlotItem
			);
		}
		
	}
}

void ABasePlayerController::SetNearbyItem(ASpawnableItem* NearbyActor)
{
	NearbyItem=NearbyActor;
}

void ABasePlayerController::TryInteract()
{
	if (NearbyItem)
	{
		// 'this' 플레이어 캐릭터를 InteractingActor로 전달
		NearbyItem->Interact(GetPawn());
	}
}

void ABasePlayerController::Server_UseFirstHotSlotItem_Implementation()
{
	UseFirstHotSlotItem(); 
}

bool ABasePlayerController::Server_UseFirstHotSlotItem_Validate()
{
	return true;
}

void ABasePlayerController::ToggleInventoryWidget()
{
	if (!UIManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("UIManager not found!"));
		return;
	}

	// 인벤토리 태그 정의
	FGameplayTag InventoryTag = NlGameplayTags::UI_GameMenu;

	// 현재 인벤토리 위젯이 열려 있는지 확인 (UIManager에 상태 확인 로직 필요)
	// 여기서는 Toggle 방식이니까 간단히 Show/Hide로 처리
	if (UIManager->IsUIActive(InventoryTag)) // IsUIActive는 추가해야 할 함수
	{
		//UIManager->HideUI(InventoryTag);
		UIManager->Deinitialize();
		// 게임 모드로 입력 전환: 마우스 커서 숨김
		FInputModeGameOnly GameInputMode;
		SetInputMode(GameInputMode);
		bShowMouseCursor = false;
	}
	else
	{
		UIManager->ShowUI(InventoryTag);
		// UI 모드로 입력 전환
		FInputModeGameAndUI UIInputMode;
	
			SetInputMode(UIInputMode);
			bShowMouseCursor = true;
		
	}
}
void ABasePlayerController::UseFirstHotSlotItem()
{
	if (!HasAuthority()) // 클라이언트라면 서버에 실행 요청
	{
		Server_UseFirstHotSlotItem();
		return;
	}
	ABasePlayerState* BasePlayerState = GetPlayerState<ABasePlayerState>();
	if (BasePlayerState)
	{
		TArray<FItemMetaInfo>* HotSlotList = BasePlayerState->GetPlayerHotSlotList();
		if (HotSlotList && HotSlotList->Num() > 0)
		{

			FItemMetaInfo FirstItem = (*HotSlotList)[0];

			const FItemInfoData& ItemInfoById = FItemHelper::GetItemInfoById(GetWorld(), FirstItem.GetId());
			
			ASpawnableItem* SpawnableItem = GetWorld()->SpawnActor<ASpawnableItem>(ItemInfoById.GetShowItemActor());
			FirstItem.SetCurrentCount(FirstItem.GetCurrentCount()-1);
					// 서버 / 클라이언트 체크 로그
			if (HasAuthority())
			{
				UE_LOG(LogTemp, Warning, TEXT("[Server] 핫슬롯 아이템 사용 - ID: %d"), FirstItem.GetId());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[Client] 핫슬롯 아이템 사용 - ID: %d"), FirstItem.GetId());
			}
			SpawnableItem->UseItem(Cast<APlayerCharacter>(GetPawn()));
		}
	}
}
