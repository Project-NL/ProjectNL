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
#include"ProjectNL/Dutorial/DutorialActor.h"


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
		if (HandleTutorial)
		{
			EnhancedInputComponent->BindAction(
				HandleTutorial, 
				ETriggerEvent::Triggered, 
				this, 
				&ABasePlayerController::HandleNextTutorial
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

void ABasePlayerController::Server_UseHotSlotItem_Implementation(int32 ItemSlotInit, const TArray<int32>& HotList, 
	const TArray<FItemMetaInfo>& InvList)
{
	ClientUseHotSlotItem(ItemSlotInit,HotList,InvList); 
}

bool ABasePlayerController::Server_UseHotSlotItem_Validate(int32 ItemSlotInit,  const TArray<int32>& HotList, 
	const TArray<FItemMetaInfo>& InvList)
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
		UIManager->ShowUI(InventoryTag);// UI 모드로 입력 전환
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
	// 1-based → 0-based 변환
	ItemSlotInit -= 1;
	
	ABasePlayerState* PS = GetPlayerState<ABasePlayerState>();
	if (!PS) return;

	TArray<int32>* HotList = PS->GetHotslotInitialItemList();
	TArray<FItemMetaInfo>* InvList = PS->GetPlayerInventoryList();
	
	Server_UseHotSlotItem(ItemSlotInit,*HotList,*InvList);
	//return;
	

	
}
void ABasePlayerController::ClientUseHotSlotItem(
	int32 ItemSlotInit,
	const TArray<int32>& HotList,
	const TArray<FItemMetaInfo>& InvList
)
{
	// 1) 빈 배열 체크
	if (HotList.Num() == 0 || InvList.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("UseHotSlotItem: HotList 또는 InvList가 비어있음"));
		return;
	}

	// 2) 슬롯 범위 검사
	if (!HotList.IsValidIndex(ItemSlotInit))
	{
		UE_LOG(LogTemp, Warning, TEXT("UseHotSlotItem: 잘못된 슬롯 인덱스(%d)"), ItemSlotInit);
		return;
	}

	int32 HotIndex = HotList[ItemSlotInit];
	// 3) 인벤토리 범위 검사
	if (HotIndex < 0 || !InvList.IsValidIndex(HotIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("UseHotSlotItem: 인벤토리 인덱스(%d)가 유효하지 않음"), HotIndex);
		return;
	}

	// 4) 아이템 사용 로직
	const FItemMetaInfo& FirstItem = InvList[HotIndex];
	const FItemInfoData& ItemInfo = FItemHelper::GetItemInfoById(GetWorld(), FirstItem.GetId());

	if (ABasePlayerState* PS = GetPlayerState<ABasePlayerState>())
	{
		PS->RemoveItem(FirstItem.GetId(), 1, ItemSlotInit);
	}

	if (UWorld* World = GetWorld())
	{
		if (ASpawnableItem* Spawned = World->SpawnActor<ASpawnableItem>(ItemInfo.GetShowItemActor()))
		{
			if (APlayerCharacter* PC = Cast<APlayerCharacter>(GetPawn()))
			{
				Spawned->UseItem(PC);
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("핫슬롯 아이템 사용 완료 – ID: %d"), FirstItem.GetId());
}


void ABasePlayerController::HandleNextTutorial()
{
	if (CurrentTutorialActor)
	{
		CurrentTutorialActor->NextTutorialStep();
	}
}
