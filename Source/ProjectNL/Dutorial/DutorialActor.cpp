#include "DutorialActor.h"
#include "Components/BoxComponent.h"
#include "Blueprint/UserWidget.h"
#include "ProjectNL/Character/Player/PlayerCharacter.h"
#include "TimerManager.h"

ADutorialActor::ADutorialActor()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	RootComponent = CollisionBox;
	CollisionBox->SetCollisionProfileName(TEXT("Trigger"));
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ADutorialActor::OnOverlapBegin);
	CollisionBox->OnComponentEndOverlap.AddDynamic(this, &ADutorialActor::OnOverlapEnd);
}

void ADutorialActor::BeginPlay()
{
	Super::BeginPlay();
}

void ADutorialActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ( OtherActor && OtherActor->IsA<APlayerCharacter>())
	{
		if (TutorialWidgetClasses.Num() > 0)
		{
			ShowNextTutorial();
		}
	}
}

void ADutorialActor::ShowNextTutorial()
{
	if (ActiveWidget)
	{
		ActiveWidget->RemoveFromParent();
		ActiveWidget = nullptr;
	}

	if (TutorialWidgetClasses.IsValidIndex(CurrentTutorialIndex))
	{
		ActiveWidget = CreateWidget<UUserWidget>(GetWorld(), TutorialWidgetClasses[CurrentTutorialIndex]);
		if (ActiveWidget)
		{
			ActiveWidget->AddToViewport();

			APlayerController* PC = GetWorld()->GetFirstPlayerController();
			if (PC)
			{
				if (CurrentTutorialIndex == 0)
				{
					// 첫 번째 튜토리얼: 입력 막기
					PC->SetIgnoreMoveInput(true);
					PC->SetIgnoreLookInput(true);
					PC->bShowMouseCursor = true;
					PC->SetInputMode(FInputModeUIOnly());
				}
				else if (CurrentTutorialIndex == 1)
				{
					// 두 번째 튜토리얼부터: 입력 해제
					PC->SetIgnoreMoveInput(false);
					PC->SetIgnoreLookInput(false);
					PC->bShowMouseCursor = false;
					PC->SetInputMode(FInputModeGameOnly());
				}
			}
		}

		++CurrentTutorialIndex;
		if (TutorialWidgetClasses.IsValidIndex(CurrentTutorialIndex))
		{
			GetWorld()->GetTimerManager().SetTimer(TutorialTimerHandle, this, &ADutorialActor::ShowNextTutorial, 10.f, false);
		}
	}
}
void ADutorialActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
								  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor->IsA<APlayerCharacter>())
	{
		if (ActiveWidget)
		{
			ActiveWidget->RemoveFromParent();
			ActiveWidget = nullptr;

			APlayerController* PC = GetWorld()->GetFirstPlayerController();
			if (PC)
			{
				// 입력 다시 허용
				PC->SetIgnoreMoveInput(false);
				PC->SetIgnoreLookInput(false);
				PC->bShowMouseCursor = false;
				PC->SetInputMode(FInputModeGameOnly());
			}
		}
		// 타이머도 취소해줘야 다음 튜토리얼 자동 진행을 막을 수 있음
		GetWorld()->GetTimerManager().ClearTimer(TutorialTimerHandle);
	}
}
