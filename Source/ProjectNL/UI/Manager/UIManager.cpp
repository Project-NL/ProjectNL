#include "UIManager.h"

#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "ProjectNL/DataTable/UIDataTable.h"
#include "ProjectNL/Helper/GameplayTagHelper.h"

bool UUIManager::IsUIActive(FGameplayTag UITag) const
{
    return ActiveWidgets.Contains(UITag);
}
// UUIManager.cpp
bool UUIManager::IsAnyUIActive() const
{
    // 하나라도 등록돼 있으면 true
    return ActiveWidgets.Num() > 0;
}

UUIManager::UUIManager()
{
    const static ConstructorHelpers::FObjectFinder<UDataTable>UIDataTable(TEXT("/Script/Engine.DataTable'/Game/Blueprints/UI/Manager/DT_UIManager.DT_UIManager'"));
 
    if (UIDataTable.Succeeded())
    {
        TArray<FUIDataTable*> TempItemInfoList;
        UIDataTable.Object->GetAllRows<FUIDataTable>(TEXT(""), TempItemInfoList);
        UIMapping=TempItemInfoList[0]->UIMapping;
      
    }
}

void UUIManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

  
    // 초기화 시 기본 UI 매핑 설정 (필요하면 에디터에서 수정 가능)
    // UIMapping.Add(NlGameplayTags::UI, nullptr); // 실제 위젯 클래스로 교체
    // UIMapping.Add(NlGameplayTags::UI_Inventory, nullptr);
    // UIMapping.Add(FGameplayTag::RequestGameplayTag("UI.PauseMenu"), nullptr);
    // UIMapping.Add(FGameplayTag::RequestGameplayTag("UI.Inventory"), nullptr);
}

void UUIManager::Deinitialize()
{
    // 모든 활성화된 위젯 제거
    for (auto& WidgetPair : ActiveWidgets)
    {
        if (WidgetPair.Value)
        {
            WidgetPair.Value->RemoveFromParent();
        }
    }
    ActiveWidgets.Empty();
    UnlockLookInput();
    Super::Deinitialize();
}

void UUIManager::ShowUI(FGameplayTag UITag)
{
    if (!UITag.IsValid()) { UE_LOG(LogTemp, Warning, TEXT("Invalid UI Tag")); return; }
    if (ActiveWidgets.Contains(UITag)) { return; }

    if (UUserWidget* NewWidget = CreateWidgetForTag(UITag))
    {
        NewWidget->AddToViewport();
        ActiveWidgets.Add(UITag, NewWidget);

        /** ← 여기서 카메라 입력 잠금 */
        LockLookInput();
    }
}

void UUIManager::HideUI(FGameplayTag UITag)
{
    if (UUserWidget** WidgetPtr = ActiveWidgets.Find(UITag))
    {
        if (*WidgetPtr) { (*WidgetPtr)->RemoveFromParent(); }
        ActiveWidgets.Remove(UITag);

        /** ← UI 하나 내려갔으니 잠금 해제 검사 */
        UnlockLookInput();
    }
}

/* ───────────────────────────────────────────── */

void UUIManager::LockLookInput()
{
    ++LookInputLockCount;
    if (LookInputLockCount > 1) return; // 이미 잠긴 상태

    if (APlayerController* PC = GEngine->GetFirstLocalPlayerController(GetWorld()))
    {
        PC->SetInputMode(FInputModeGameOnly());
        PC->bShowMouseCursor = false;

        // 혹시 메뉴 열 때 Look/Move 입력을 막았다면 다시 풀기
        PC->SetIgnoreLookInput(false);
        PC->SetIgnoreMoveInput(false);        // 마우스 커서 표시

        FInputModeUIOnly Mode;
        Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        Mode.SetWidgetToFocus(nullptr);        // 필요하면 최근 위젯의 TakeWidget() 전달
        PC->SetInputMode(Mode);
    }
}

void UUIManager::UnlockLookInput()
{
    if (--LookInputLockCount > 0) return;      // 아직 다른 UI가 남아 있음

    if (APlayerController* PC = GEngine->GetFirstLocalPlayerController(GetWorld()))
    {
        PC->SetIgnoreLookInput(false);         // 룩 입력 복구
        PC->bShowMouseCursor = false;

        PC->SetInputMode(FInputModeGameOnly());
    }
}

void UUIManager::ToggleUI(FGameplayTag UITag)
{
    if (ActiveWidgets.Contains(UITag))
    {
        HideUI(UITag);
    }
    else
    {
        ShowUI(UITag);
    }
}

UUserWidget* UUIManager::CreateWidgetForTag(FGameplayTag UITag)
{
    TSubclassOf<UUserWidget>* WidgetClassPtr = UIMapping.Find(UITag);
    if (!WidgetClassPtr || !*WidgetClassPtr)
    {
        UE_LOG(LogTemp, Error, TEXT("No widget class mapped for tag: %s"), *UITag.ToString());
        return nullptr;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("No valid world context"));
        return nullptr;
    }

    return CreateWidget<UUserWidget>(World, *WidgetClassPtr);
}
UUserWidget* UUIManager::GetActiveUI(FGameplayTag UITag)
{
    if (UUserWidget** WidgetPtr = ActiveWidgets.Find(UITag))
    {
        return *WidgetPtr;
    }
    return nullptr;
}