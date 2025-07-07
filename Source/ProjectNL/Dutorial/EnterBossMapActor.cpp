// Fill out your copyright notice in the Description page of Project Settings.

#include "EnterBossMapActor.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "ProjectNL/Character/BaseCharacter.h"
#include "ProjectNL/Helper/GameplayTagHelper.h"

// Sets default values
AEnterBossMapActor::AEnterBossMapActor()
{
	PrimaryActorTick.bCanEverTick = true;

	// 1) CollisionBox 생성 및 루트 컴포넌트로 설정
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	RootComponent = CollisionBox;

	// 2) Box 크기 조정 (예: 가로 200, 세로 200, 높이 200으로 설정)
	CollisionBox->SetBoxExtent(FVector(100.f, 100.f, 100.f));

	// 3) 충돌 프로필 설정: Overlap 이벤트만 트리거되도록
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);

	// 4) Overlap 이벤트 바인딩
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AEnterBossMapActor::OnOverlapBegin);
}

// Called when the game starts or when spawned
void AEnterBossMapActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AEnterBossMapActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AEnterBossMapActor::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	// 1) OtherActor가 유효하고 자기자신(this)이 아닐 때만 처리
	if (!OtherActor || OtherActor == this)
	{
		return;
	}

	// 2) 캐릭터 타입인지 검사
	ACharacter* OverlappingCharacter = Cast<ACharacter>(OtherActor);
	if (!OverlappingCharacter)
	{
		return;
	}

	ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(OverlappingCharacter);
	if (!BaseCharacter)
	{
		return;
	}

	// 3) OverlapGameplayEffect 프로퍼티에 클래스가 지정되어 있는지 확인
	if (OverlapGameplayEffect)
	{
		// 4) 플레이어의 AbilitySystemComponent 가져오기
		UAbilitySystemComponent* ASC = BaseCharacter->GetAbilitySystemComponent();
		if (ASC)
		{
			ASC->RemoveLooseGameplayTag(NlGameplayTags::Status_Invincibile);
			// 5) 지정된 클래스에 해당하는 활성화된 모든 이펙트를 제거
			//    RemoveActiveEffectsWithGameplayEffectClass 함수는 제거된 개수를 반환합니다.
			// ASC->RemoveActiveGameplayEffectBySourceEffect(OverlapGameplayEffect,ASC);
			
		}
	}

	// 6) 보스맵으로 레벨 전환 (이펙트 제거 후 즉시)
	FName BossLevelName = FName(TEXT("BossMapLevel"));
	UGameplayStatics::OpenLevel(GetWorld(), BossLevelName);
}
