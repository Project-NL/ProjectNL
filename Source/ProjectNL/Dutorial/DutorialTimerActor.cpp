// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectNL/Dutorial/DutorialTimerActor.h"
#include "Components/BoxComponent.h"
#include "Blueprint/UserWidget.h"
#include "TimerManager.h"
#include "GameFramework/PlayerController.h"

ADutorialTimerActor::ADutorialTimerActor()
{
    PrimaryActorTick.bCanEverTick = false;

    // Trigger Box 구성
    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    RootComponent = TriggerBox;                  // 루트로 설정
    TriggerBox->SetBoxExtent(FVector(100.f));    // 기본 크기 (BP에서 변경 가능)
    TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    TriggerBox->SetGenerateOverlapEvents(true);
}

void ADutorialTimerActor::BeginPlay()
{
    Super::BeginPlay();

    // 델리게이트 바인딩
    if (TriggerBox)
    {
        TriggerBox->OnComponentBeginOverlap.AddDynamic(
            this, &ADutorialTimerActor::OnTriggerBeginOverlap);
    }
}

void ADutorialTimerActor::OnTriggerBeginOverlap(
    UPrimitiveComponent* /*OverlappedComp*/,
    AActor*              OtherActor,
    UPrimitiveComponent* /*OtherComp*/,
    int32                /*OtherBodyIndex*/,
    bool                 /*bFromSweep*/,
    const FHitResult&    /*SweepResult*/)
{
    if (!OtherActor || SpawnedWidget) return;          // 이미 띄웠으면 무시

    // 플레이어 Pawn만 트리거하도록 간단한 체크
    if (APawn* Pawn = Cast<APawn>(OtherActor))
    {
        if (Pawn->IsLocallyControlled())               // 로컬 클라이언트만
        {
            ShowWidget();

            // 더 이상 중복 트리거 안 나도록 끔
            TriggerBox->SetGenerateOverlapEvents(false);
        }
    }
}

void ADutorialTimerActor::ShowWidget()
{
    if (TutorialWidgetClass)
    {
        SpawnedWidget = CreateWidget<UUserWidget>(
            GetWorld(), TutorialWidgetClass);

        if (SpawnedWidget)
        {
            SpawnedWidget->AddToViewport();

            // 10초 뒤에 UI 제거
            GetWorldTimerManager().SetTimer(
                HideTimerHandle,
                this,
                &ADutorialTimerActor::HideWidget,
                10.f,
                false);
        }
    }
}

void ADutorialTimerActor::HideWidget()
{
    if (SpawnedWidget)
    {
        SpawnedWidget->RemoveFromParent();
        SpawnedWidget = nullptr;
    }

    Destroy();   // 필요 없다면 Actor 삭제
}
