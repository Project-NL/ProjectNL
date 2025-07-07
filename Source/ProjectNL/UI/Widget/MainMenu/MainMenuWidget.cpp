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
		UGameplayStatics::OpenLevel(World, FName("DutorialMap1"), true, "listen");
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
	if (IPAddress.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("JoinServer: IP address is empty. Join canceled."));
		return;
	}

	// 간단한 IP 주소 형식 검사 (예: 127.0.0.1:7777)
	FString Pattern = TEXT("^(\\d{1,3}\\.){3}\\d{1,3}(:\\d+)?$");
	const FRegexPattern RegexPattern(Pattern);
	FRegexMatcher Matcher(RegexPattern, IPAddress);
	if (!Matcher.FindNext())
	{
		UE_LOG(LogTemp, Warning, TEXT("JoinServer: Invalid IP address format."));
		return;
	}

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		FString URL = IPAddress;
		PlayerController->ClientTravel(URL, ETravelType::TRAVEL_Absolute);
	}
}