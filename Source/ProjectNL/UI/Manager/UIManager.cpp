#include "UIManager.h"

#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "ProjectNL/DataTable/UIDataTable.h"
#include "ProjectNL/Helper/GameplayTagHelper.h"

bool UUIManager::IsUIActive(FGameplayTag UITag) const
{
    return ActiveWidgets.Contains(UITag);
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
    Super::Deinitialize();
}

void UUIManager::ShowUI(FGameplayTag UITag)
{
    if (!UITag.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid UI Tag"));
        return;
    }

    // 이미 활성화된 UI가 있으면 무시하거나 처리
    if (ActiveWidgets.Contains(UITag))
    {
        return;
    }

    UUserWidget* NewWidget = CreateWidgetForTag(UITag);
    if (NewWidget)
    {
        NewWidget->AddToViewport();
        ActiveWidgets.Add(UITag, NewWidget);
    }
}

void UUIManager::HideUI(FGameplayTag UITag)
{
    if (UUserWidget** WidgetPtr = ActiveWidgets.Find(UITag))
    {
        if (*WidgetPtr)
        {
            (*WidgetPtr)->RemoveFromParent();
        }
        ActiveWidgets.Remove(UITag);
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