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
		if (ToggleSecondHotSlotItem)
		{
			EnhancedInputComponent->BindAction(
				ToggleSecondHotSlotItem, 
				ETriggerEvent::Triggered, 
				this, 
				&ABasePlayerController::UseSecondHotSlotItem
			);
		}
		if (ToggleThirdHotSlotItem)
		{
			EnhancedInputComponent->BindAction(
				ToggleThirdHotSlotItem, 
				ETriggerEvent::Triggered, 
				this, 
				&ABasePlayerController::UseThirdHotSlotItem
			);
		}
		if (ToggleFourthHotSlotItem)
		{
			EnhancedInputComponent->BindAction(
				ToggleFourthHotSlotItem, 
				ETriggerEvent::Triggered, 
				this, 
				&ABasePlayerController::UseFourthHotSlotItem
			);
		}
		if (ToggleFifthHotSlotItem)
		{
			EnhancedInputComponent->BindAction(
				ToggleFifthHotSlotItem, 
				ETriggerEvent::Triggered, 
				this, 
				&ABasePlayerController::UseFifthHotSlotItem
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

void ABasePlayerController::Server_UseHotSlotItem_Implementation(int32 ItemSlotInit)
{
	UseHotSlotItem(ItemSlotInit); 
}

bool ABasePlayerController::Server_UseHotSlotItem_Validate(int32 ItemSlotInit)
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
	UseHotSlotItem(1);
}

void ABasePlayerController::UseSecondHotSlotItem()
{
	UseHotSlotItem(2);
}

void ABasePlayerController::UseThirdHotSlotItem()
{
	UseHotSlotItem(3);
}

void ABasePlayerController::UseFourthHotSlotItem()
{
	UseHotSlotItem(4);
}

void ABasePlayerController::UseFifthHotSlotItem()
{
	UseHotSlotItem(5);
}

void ABasePlayerController::UseHotSlotItem(int32 ItemSlotInit)
{
	ItemSlotInit-=1;
	if (!HasAuthority()) // 클라이언트라면 서버에 실행 요청
	{
		Server_UseHotSlotItem(ItemSlotInit);
		return;
	}
	ABasePlayerState* BasePlayerState = GetPlayerState<ABasePlayerState>();
	if (BasePlayerState)
	{
		TArray<int32> *HotslotInitailItemList = BasePlayerState->GetHotslotInitialItemList();
		TArray<FItemMetaInfo>* PlayerInventoryList = BasePlayerState->GetPlayerInventoryList();
		if (PlayerInventoryList && PlayerInventoryList->Num() > 0)
		{
			int32 HotslotInitail = (*HotslotInitailItemList)[ItemSlotInit];
			if (HotslotInitail<0)
			{
				return;
			}
			FItemMetaInfo *FirstItem = &(*PlayerInventoryList)[HotslotInitail];

			const FItemInfoData& ItemInfoById = FItemHelper::GetItemInfoById(GetWorld(), FirstItem->GetId());
			
			ASpawnableItem* SpawnableItem = GetWorld()->SpawnActor<ASpawnableItem>(ItemInfoById.GetShowItemActor());
			BasePlayerState->RemoveItem(FirstItem->GetId(),1,ItemSlotInit);
			if (SpawnableItem)
			{
				SpawnableItem->UseItem(Cast<APlayerCharacter>(GetPawn()));
			}// 서버 / 클라이언트 체크 로그
			if (HasAuthority())
			{
				UE_LOG(LogTemp, Warning, TEXT("[Server] 핫슬롯 아이템 사용 - ID: %d"), FirstItem->GetId());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[Client] 핫슬롯 아이템 사용 - ID: %d"), FirstItem->GetId());
			}
		}
	}
}
