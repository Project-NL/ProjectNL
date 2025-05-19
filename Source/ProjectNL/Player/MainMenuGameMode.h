#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MainMenuGameMode.generated.h"


class UUserWidget;
UCLASS()
class PROJECTNL_API AMainMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;


protected:
	// 위젯 클래스는 에디터에서 지정할 수 있도록 BlueprintReadWrite
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> MainMenuWidgetClass;

private:
	// 생성된 위젯 인스턴스 저장
	UPROPERTY()
	UUserWidget* MainMenuWidget;
};
