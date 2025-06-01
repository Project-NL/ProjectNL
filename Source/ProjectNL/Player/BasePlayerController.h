#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "ProjectNL/Helper/UtilHelper.h"
#include "BasePlayerController.generated.h"


class UPlayerStatus;
class UInventoryWidget;
class UInputAction;
class ASpawnableItem;
class UUIManager;
class ADutorialActor;

UCLASS()
class PROJECTNL_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void BeginPlayingState() override;
	virtual void SetupInputComponent() override;

	void SetNearbyItem(ASpawnableItem* NearbyItem);
	void InteractWidget(FGameplayTag UITag);
	GETTER_SETTER(ADutorialActor*,CurrentTutorialActor);

public:
	UPROPERTY()
	ADutorialActor* CurrentTutorialActor;

	
private:
	void TryInteract();
protected:
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_UseHotSlotItem(int32 ItemSlotInit, const TArray<int32>& HotList, 
	const TArray<FItemMetaInfo>& InvList );

	// TODO: 임시코드로 추후 HUD class에 이전할 필요 있음
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UPlayerStatus> PlayerStatusHUDClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> PlayerHotslotHUDClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> KeyTutorialHUDClass;
	
	UPROPERTY()
	UPlayerStatus* PlayerStatus;

	UPROPERTY()
	UUserWidget* PlayerHotslot;

	UPROPERTY()
	UUserWidget* KeyTutorial;
	/** ESC 메뉴 위젯 블루프린트 클래스를 에디터에서 지정 */

	// 인벤토리 위젯 클래스 (Blueprint에서 지정)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> InventoryWidgetClass;
	// 생성된 인벤토리 위젯 인스턴스
	UPROPERTY()
	UUserWidget* InventoryWidget;
	// Enhanced Input용 인벤토리 토글 액션
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* ToggleInventoryAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* EscButtonInputAction;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* AcquireSpawnItem;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* HandleTutorial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* ToggleFirstHotSlotItem;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* ToggleSecondHotSlotItem;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* ToggleThirdHotSlotItem;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* ToggleFourthHotSlotItem;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* ToggleFifthHotSlotItem;
	UPROPERTY()
	ASpawnableItem* NearbyItem;
	
	UPROPERTY()
	UUIManager* UIManager;

	// Esc입력시 호출될 Esc메뉴 
	UFUNCTION()
	void EscMenuWidget();
	
	// Tab 키 입력 시 호출될 인벤토리 토글 함수
	UFUNCTION()
	void ToggleInventoryWidget();

	UFUNCTION()
	void UseFirstHotSlotItem();

	UFUNCTION()
	void UseSecondHotSlotItem();

	UFUNCTION()
	void UseThirdHotSlotItem();

	UFUNCTION()
	void UseFourthHotSlotItem();

	UFUNCTION()
	void UseFifthHotSlotItem();
	
	UFUNCTION()
	void UseHotSlotItem(int32 ItemSlotInit);

	UFUNCTION()
	void ClientUseHotSlotItem(int32 ItemSlotInit, const TArray<int32>& HotList, 
	const TArray<FItemMetaInfo>& InvList);

	void HandleNextTutorial();
	
};
