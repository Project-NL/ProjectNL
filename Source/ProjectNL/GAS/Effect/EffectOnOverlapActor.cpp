#include "EffectOnOverlapActor.h"
#include "Components/BoxComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"

AEffectOnOverlapActor::AEffectOnOverlapActor()
{
	PrimaryActorTick.bCanEverTick = false;

	// 박스 컴포넌트 생성 및 기본 설정
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	RootComponent = CollisionBox;
	CollisionBox->SetCollisionProfileName(TEXT("Trigger"));
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AEffectOnOverlapActor::OnOverlapBegin);
}

void AEffectOnOverlapActor::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	// 유효 검사
	if (!OtherActor || OtherActor == this || !EffectToApplyClass)
	{
		return;
	}

	// GAS 인터페이스 구현 확인
	IAbilitySystemInterface* GASActor = Cast<IAbilitySystemInterface>(OtherActor);
	if (!GASActor)
	{
		return;
	}

	UAbilitySystemComponent* OtherASC = GASActor->GetAbilitySystemComponent();
	if (!OtherASC)
	{
		return;
	}

	// 이펙트 컨텍스트 생성 및 소스 지정
	FGameplayEffectContextHandle Context = OtherASC->MakeEffectContext();
	Context.AddSourceObject(this);

	// 스펙 생성 (레벨 1.0)
	FGameplayEffectSpecHandle SpecHandle =
		OtherASC->MakeOutgoingSpec(EffectToApplyClass, 1.0f, Context);

	if (SpecHandle.IsValid())
	{
		// 상대(자기 자신 ASC)에 이펙트 적용
		OtherASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}

	//한 번만 적용하고 삭제
	 Destroy();
}
