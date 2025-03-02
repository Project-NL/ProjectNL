#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ProjectNL/Helper/UtilHelper.h"
#include "BasePlayerController.generated.h"


class UPlayerStatus;
class UInventoryWidget;
class UInputAction;
class ASpawnableItem;
UCLASS()
class PROJECTNL_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void BeginPlayingState() override;
	virtual void SetupInputComponent() override;

	void SetNearbyItem(ASpawnableItem* NearbyItem);

private:
	void TryInteract();
protected:


	// TODO: 임시코드로 추후 HUD class에 이전할 필요 있음
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UPlayerStatus> PlayerStatusHUDClass;
	
	UPROPERTY()
	UPlayerStatus* PlayerStatus;

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
	UInputAction* AcquireSpawnItem;

	UPROPERTY()
	ASpawnableItem* NearbyItem;
	
	// Tab 키 입력 시 호출될 인벤토리 토글 함수
	UFUNCTION()
	void ToggleInventoryWidget();
};
