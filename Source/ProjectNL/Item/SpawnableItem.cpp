#include "SpawnableItem.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "ProjectNL/Character/Player/PlayerCharacter.h"
#include "ProjectNL/Player/BasePlayerState.h"


class APlayerCharacter;

ASpawnableItem::ASpawnableItem()
{
	PrimaryActorTick.bCanEverTick = false;

	// 박스 컴포넌트를 생성하고 RootComponent로 설정
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	RootComponent = CollisionBox;

	// 충돌 설정: 오버랩 이벤트만 발생하도록 설정
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionBox->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Overlap);

	// 충돌 이벤트 바인딩
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ASpawnableItem::OnOverlapBegin);
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
	// OtherActor가 유효하고 자신이 아닌 경우
	if (OtherActor && OtherActor != this)
	{
		// 예시로 APlayerCharacter를 플레이어 캐릭터 클래스로 가정
		APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
		ABasePlayerState* BasePlayerState = Cast<ABasePlayerState>(PlayerCharacter->GetPlayerState());
		if (BasePlayerState)
		{
				// 인벤토리에 아이템 추가 (인벤토리 컴포넌트의 AddItem 함수 구현에 따라 반환값 체크)
				int8 bAdded = BasePlayerState->AddItem(ItemMetaInfo);
				if (bAdded>0)
				{
					// 아이템 추가에 성공하면, 아이템 액터를 파괴합니다.
					Destroy();
				}
			}
		}
	}

void ASpawnableItem::Interact(AActor* Actor)
{
	// 상호작용 인터페이스 구현: 필요에 따라 추가 처리 가능
	// 예를 들어, 플레이어가 상호작용 시에도 인벤토리에 추가할 수 있음.
}

void ASpawnableItem::UseItem()
{
	// 아이템 사용 인터페이스 구현: 필요에 따라 처리
}

FItemMetaInfo* ASpawnableItem::GetItemMetainfo()
{
	return &ItemMetaInfo;
}
