#include "SpawnableItem.h"
#include "Components/BoxComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "ProjectNL/Character/Player/PlayerCharacter.h"
#include "ProjectNL/Player/BasePlayerController.h"
#include "ProjectNL/Player/BasePlayerState.h"
#include "ProjectNL/UI/Widget/Item/ItemAcquireTextWidget.h"

ASpawnableItem::ASpawnableItem()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;

    SetReplicates(true);
    // SkeletalMeshComponent (물리 적용 대상)
    SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
    RootComponent = SkeletalMeshComponent;

    SkeletalMeshComponent->SetSimulatePhysics(false);
    SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    SkeletalMeshComponent->SetCollisionObjectType(ECC_PhysicsBody);
    SkeletalMeshComponent->SetCollisionResponseToAllChannels(ECR_Block);
    SkeletalMeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore); 

    // CollisionBox (획득용 오버랩 판정)
    CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
    CollisionBox->SetupAttachment(RootComponent);
    CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    CollisionBox->SetCollisionObjectType(ECC_WorldDynamic);
    CollisionBox->SetCollisionResponseToAllChannels(ECR_Overlap);
    CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ASpawnableItem::OnOverlapBegin);
    CollisionBox->OnComponentEndOverlap.AddDynamic(this, &ASpawnableItem::OnOverlapEnd);

    // 위젯 컴포넌트 (획득 UI)
    AcquireWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("AcquireWidgetComponent"));
    AcquireWidgetComponent->SetupAttachment(RootComponent);
    AcquireWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
    AcquireWidgetComponent->SetDrawSize(FVector2D(200.f, 50.f));
    AcquireWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
    AcquireWidgetComponent->SetWidgetClass(UItemAcquireTextWidget::StaticClass());
    AcquireWidgetComponent->SetVisibility(false);

    bcheckitem=false;
}

void ASpawnableItem::BeginPlay()
{
    Super::BeginPlay();
}

void ASpawnableItem::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    //AcquireWidgetComponentLookAtPlayer();
}

void ASpawnableItem::AcquireWidgetComponentLookAtPlayer()
{
    if (!AcquireWidgetComponent || AcquireWidgetComponent->GetWidgetSpace() != EWidgetSpace::World)
        return;

    APlayerCameraManager* CamManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
    if (!CamManager) return;

    FVector CameraLocation = CamManager->GetCameraLocation();
    FVector WidgetLocation = AcquireWidgetComponent->GetComponentLocation();
    FRotator LookRot = UKismetMathLibrary::FindLookAtRotation(WidgetLocation, CameraLocation);
    LookRot.Pitch = 0.f;
    LookRot.Roll = 0.f;
    AcquireWidgetComponent->SetWorldRotation(LookRot);
}

void ASpawnableItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherActor || OtherActor == this) return;

    APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
    if (!PlayerCharacter) return;

    ABasePlayerController* BasePlayerController = Cast<ABasePlayerController>(PlayerCharacter->GetController());
    if (BasePlayerController)
    {
        OverlappingPlayer = PlayerCharacter;
        OverlappingPlayerController = BasePlayerController;
        OverlappingPlayerController->SetNearbyItem(this);
        AcquireWidgetComponent->SetVisibility(true);
        SetOwner(PlayerCharacter);
    }
}

void ASpawnableItem::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (!OtherActor || OtherActor == this) return;

    APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
    if (PlayerCharacter && PlayerCharacter == OverlappingPlayer)
    {
        OverlappingPlayer = nullptr;
        OverlappingPlayerController = nullptr;
        AcquireWidgetComponent->SetVisibility(false);
    }
}

void ASpawnableItem::Interact(AActor* InteractingActor)
{
    APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(InteractingActor);
    if (PlayerCharacter && OverlappingPlayer == PlayerCharacter)
    {
        ABasePlayerState* BasePlayerState = Cast<ABasePlayerState>(OverlappingPlayer->GetPlayerState());
        if (BasePlayerState)
        {
            ABasePlayerController* BasePlayerController = Cast<ABasePlayerController>(PlayerCharacter->GetController());
            if (BasePlayerController)
            {
                BasePlayerController->SetNearbyItem(nullptr);
            }
            int8 bAdded = BasePlayerState->AddItem(ItemMetaInfo);
            ServerInteract(InteractingActor);
        }
    }
}

void ASpawnableItem::ServerInteract_Implementation(AActor* InteractingActor)
{
    APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(InteractingActor);
    if (PlayerCharacter)
    {
        
        ABasePlayerController* BasePlayerController = Cast<ABasePlayerController>(PlayerCharacter->GetController());
        if (BasePlayerController)
        {
            BasePlayerController->SetNearbyItem(nullptr);
        }
    }
    DestroyItem();
}

bool ASpawnableItem::ServerInteract_Validate(AActor* InteractingActor)
{
    return true;
}

void ASpawnableItem::DestroyItem()
{
    if (HasAuthority())
    {
        MulticastDestroyItem();
        Super::Destroy();
    }
}

void ASpawnableItem::MulticastDestroyItem_Implementation()
{
    
    Destroy();
}

void ASpawnableItem::Multicast_SetCollision_Implementation()
{
    if (CollisionBox)
    {
        CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        UE_LOG(LogTemp, Warning, TEXT("Multicast_SetCollision() 실행됨! (클라이언트)"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("CollisionBox가 nullptr입니다!"));
    }
}

void ASpawnableItem::OnRep_CollisionBox()
{
    CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    UE_LOG(LogTemp, Error, TEXT("OnRep_CollisionBox 발동."));
}

void ASpawnableItem::UseItem(APlayerCharacter* playerCharacter)
{
    // 아이템 사용 로직이 있다면 여기에 작성
}

FItemMetaInfo* ASpawnableItem::GetItemMetainfo()
{
    return &ItemMetaInfo;
}

void ASpawnableItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ASpawnableItem, bcheckitem);
}
