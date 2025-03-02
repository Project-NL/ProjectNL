#include "SpawnableItem.h"
#include "Components/BoxComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "ProjectNL/Character/Player/PlayerCharacter.h"
#include "ProjectNL/Player/BasePlayerController.h"
#include "ProjectNL/Player/BasePlayerState.h"
#include "ProjectNL/UI/Widget/Item/ItemAcquireTextWidget.h"

ASpawnableItem::ASpawnableItem()
{
    PrimaryActorTick.bCanEverTick = true;

    CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
    RootComponent = CollisionBox;

    CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    CollisionBox->SetCollisionObjectType(ECC_WorldDynamic);
    CollisionBox->SetCollisionResponseToAllChannels(ECR_Overlap);

    // Overlap 이벤트들
    CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ASpawnableItem::OnOverlapBegin);
    CollisionBox->OnComponentEndOverlap.AddDynamic(this, &ASpawnableItem::OnOverlapEnd);

    // 3D WidgetComponent 생성
    AcquireWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("AcquireWidgetComponent"));
    AcquireWidgetComponent->SetupAttachment(RootComponent);
    
    // (옵션) 위젯 공간을 'World' 또는 'Screen'으로 설정
    AcquireWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
    
    // (옵션) 위젯 크기 지정
    AcquireWidgetComponent->SetDrawSize(FVector2D(200.f, 50.f));

    // (옵션) 살짝 위로 띄워서 아이템 상단에 표시하기
    AcquireWidgetComponent->SetRelativeLocation(FVector(0.f, 200.f, 100.f));
    AcquireWidgetComponent->SetWidgetClass(UItemAcquireTextWidget::StaticClass());
    
    // 시작 시에는 보이지 않도록 설정
    AcquireWidgetComponent->SetVisibility(false);

    
}

void ASpawnableItem::BeginPlay()
{
    Super::BeginPlay();
}

void ASpawnableItem::OnOverlapBegin(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor != this)
    {
        APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
        if (!PlayerCharacter)
        {
            return;
        }
        ABasePlayerController *BasePlayerController = Cast<ABasePlayerController>(PlayerCharacter->GetController());
        if (BasePlayerController)
        {
            // 플레이어가 충돌 범위 안에 들어옴
            OverlappingPlayer = PlayerCharacter;
            OverlappingPlayerController=BasePlayerController;
            OverlappingPlayerController->SetNearbyItem(this);
            AcquireWidgetComponent->SetVisibility(true);
        }
    }
}

void ASpawnableItem::OnOverlapEnd(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex)
{
    if (OtherActor && OtherActor != this)
    {
        APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
        if (PlayerCharacter && PlayerCharacter == OverlappingPlayer)
        {
            // 플레이어가 범위를 벗어남 → 참조 제거
            OverlappingPlayer = nullptr;
            OverlappingPlayerController=nullptr;
            AcquireWidgetComponent->SetVisibility(false);
        }
    }
}

void ASpawnableItem::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    AcquireWidgetComponentLookAtPlayer();
}

void ASpawnableItem::AcquireWidgetComponentLookAtPlayer()
{
    // 혹은 InfoWidget이 유효하고 World 공간 모드일 때만 작업
    if (!AcquireWidgetComponent || AcquireWidgetComponent->GetWidgetSpace() != EWidgetSpace::World)
    {
        return;
    }

    // 1) 플레이어 카메라 매니저 가져오기
    APlayerCameraManager* CamManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
    if (!CamManager) return;

    FVector CameraLocation = CamManager->GetCameraLocation();
    FVector WidgetLocation = AcquireWidgetComponent->GetComponentLocation();

    // 2) FindLookAtRotation(시작 위치, 목표 위치) 사용
    // → (Widget가 카메라를 '보게' 하려면, 시작=위젯, 목표=카메라)
    FRotator LookRot = UKismetMathLibrary::FindLookAtRotation(WidgetLocation, CameraLocation);

    // 3) 필요하다면 Pitch이나 Yaw만 사용 / Roll은 0 처리 등
    // **Pitch(상하)과 Roll(회전축 기울임)는 고정하고, Yaw만 유지**
    LookRot.Pitch = 0.f;
    LookRot.Roll = 0.f;

    // 4) 적용
    AcquireWidgetComponent->SetWorldRotation(LookRot);
}

void ASpawnableItem::Interact(AActor* InteractingActor)
{
    // 예: InteractingActor가 플레이어인지 확인
    ABasePlayerController* BasePlayerController = Cast<ABasePlayerController>(InteractingActor);
    if (BasePlayerController && OverlappingPlayerController ==BasePlayerController)
    {
        // 실제 아이템 획득 로직
        ABasePlayerState* BasePlayerState = Cast<ABasePlayerState>(OverlappingPlayer->GetPlayerState());
        if (BasePlayerState)
        {
            int8 bAdded = BasePlayerState->AddItem(ItemMetaInfo);
        //    if (bAdded > 0)
        //    {
                // 획득 성공 시 아이템 파괴
                Destroy();
          //  }
        }
        
    }
}

void ASpawnableItem::UseItem()
{
}

FItemMetaInfo* ASpawnableItem::GetItemMetainfo()
{
    return &ItemMetaInfo;
}
