// MainMenuWidget.cpp

#include "MainMenuWidget.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/KismetSystemLibrary.h"
void UMainMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (StartButton)
	{
		StartButton->OnClicked.AddDynamic(this, &UMainMenuWidget::HostServer);
	}

	if (joinButton)
	{
		joinButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnJoinButtonClicked);
	}
 }

void UMainMenuWidget::HostServer()
{
	UWorld* World = GetWorld();
	if (World)
	{
		// "YourMapName"을 실제 맵 이름으로 바꿔줘
		UGameplayStatics::OpenLevel(World, FName("TestMap_Targeting"), true, "listen");
	}
}

void UMainMenuWidget::OnJoinButtonClicked()
{
	if (!IPAddressInput) return;
	
	const FString EnteredIP = IPAddressInput->GetText().ToString();

	if (!EnteredIP.IsEmpty())
	{
		JoinServer(EnteredIP);
	}
	else
	{
		// 디버그용: 입력이 비어있을 때
		UKismetSystemLibrary::PrintString(this, TEXT("IP 주소를 입력하세요."));
	}
}
void UMainMenuWidget::JoinServer(const FString& IPAddress)
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		FString URL = IPAddress;
		PlayerController->ClientTravel(URL, ETravelType::TRAVEL_Absolute);
	}
}