#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BasePlayerController.generated.h"


class UPlayerStatus;

UCLASS()
class PROJECTNL_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	// TODO: 임시코드로 추후 HUD class에 이전할 필요 있음
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UPlayerStatus> PlayerStatusHUDClass;
protected:
	virtual void BeginPlay() override;

	virtual void BeginPlayingState() override;
private:
	UPROPERTY()
	TObjectPtr<UPlayerStatus> PlayerStatus;
};
