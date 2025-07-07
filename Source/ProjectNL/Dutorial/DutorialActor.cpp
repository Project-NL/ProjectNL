#include "DutorialActor.h"
#include "Components/BoxComponent.h"
#include "Blueprint/UserWidget.h"
#include "ProjectNL/Character/Player/PlayerCharacter.h"
//
#include "Kismet/GameplayStatics.h"
#include "ProjectNL/Player/BasePlayerController.h"

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
	// 1) OtherActor가 플레이어 캐릭터이고, 그 플레이어가 '로컬 컨트롤' 중인 자신의 캐릭터일 때만 이후 로직 수행
	if (APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor))
	{
		if (!Player->IsLocallyControlled())
		{
			// 다른 클라이언트나 서버에서는 아무것도 안 함
			return;
		}

		// 이제 로컬 플레이어일 때만 실행
		if (TutorialWidgetClasses.Num() > 0 && CurrentTutorialIndex < 1)
		{
			ShowNextTutorial();

			if (APlayerController* PC = Cast<APlayerController>(Player->GetController()))
			{
				if (ABasePlayerController* BasePC = Cast<ABasePlayerController>(PC))
				{
					BasePC->CurrentTutorialActor = this;
				}
			}
		}
	
			UAbilitySystemComponent* AbilitySystemComponent =Player->GetAbilitySystemComponent();
			if (AbilitySystemComponent && OverlapGameplayEffect)
			{
				// EffectContext 생성 (소스 오브젝트를 this로 지정)
				FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
				EffectContext.AddSourceObject(this);

				// Level 은 1.0f로 가정
				const float EffectLevel = 1.0f;
				FGameplayEffectSpecHandle EffectSpec = AbilitySystemComponent->MakeOutgoingSpec(
					OverlapGameplayEffect,  // 헤더에서 지정한 클래스
					EffectLevel,
					EffectContext
				);

				if (EffectSpec.IsValid() && EffectSpec.Data.IsValid())
				{
					// 자기 자신에게 즉시 적용
					AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(
						*EffectSpec.Data.Get()
					);
				}
		}
		
	}
}

void ADutorialActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
								  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor))
	{
		if (ActiveWidget)
		{
			ActiveWidget->RemoveFromParent();
			ActiveWidget = nullptr;
		}

		if (APlayerController* PC = Cast<APlayerController>(Player->GetController()))
		{
			if (ABasePlayerController* BasePlayerController = Cast<ABasePlayerController>(PC))
			{
				BasePlayerController->CurrentTutorialActor = nullptr;
			}

			PC->SetIgnoreMoveInput(false);
			PC->SetIgnoreLookInput(false);
			PC->bShowMouseCursor = false;
			PC->SetInputMode(FInputModeGameOnly());
		}
	}
}

void ADutorialActor::ShowNextTutorial()
{
	// 이전 위젯 제거
	if (ActiveWidget)
	{
		ActiveWidget->RemoveFromParent();
		ActiveWidget = nullptr;
	}

	// 아직 남은 튜토리얼이 있으면
	if (TutorialWidgetClasses.IsValidIndex(CurrentTutorialIndex))
	{
		if (NextTutorialSound)
		{
			UGameplayStatics::PlaySound2D(this, NextTutorialSound);
		}
		// 새 위젯 생성
		ActiveWidget = CreateWidget<UUserWidget>(GetWorld(), TutorialWidgetClasses[CurrentTutorialIndex]);
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
				PC->SetInputMode(FInputModeGameAndUI());
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

		// 다음 인덱스로
		++CurrentTutorialIndex;
	}
	else
	{
		// 모든 튜토리얼 종료 시 원래대로 복구
		APlayerController* PC = GetWorld()->GetFirstPlayerController();
		if (PC)
		{
			PC->SetIgnoreMoveInput(false);
			PC->SetIgnoreLookInput(false);
			PC->bShowMouseCursor = false;
			PC->SetInputMode(FInputModeGameOnly());
		}
	}
}

void ADutorialActor::NextTutorialStep()
{
	ShowNextTutorial();
}
