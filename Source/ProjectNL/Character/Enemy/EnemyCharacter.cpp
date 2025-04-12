#include "ProjectNL/Character/Enemy/EnemyCharacter.h"
#include "AbilitySystemComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "ProjectNL/Character/Player/PlayerCharacter.h"
#include "ProjectNL/Component/EquipComponent/EquipComponent.h"
#include "ProjectNL/GAS/Attribute/BaseAttributeSet.h"
#include "ProjectNL/GAS/NLAbilitySystemComponent.h"
#include "ProjectNL/UI/Widget/Enemy/EnemyStatus.h"
#include "ProjectNL/UI/Widget/PlayerStatus/PlayerStatus.h"



AEnemyCharacter::AEnemyCharacter()
{
	AbilitySystemComponent = CreateDefaultSubobject<UNLAbilitySystemComponent>(
	"Ability System Component");
	EnemyAttributeSet = CreateDefaultSubobject<UBaseAttributeSet>(TEXT("AttributeSet"));
	
	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetupAttachment(RootComponent);
    
	// (옵션) 위젯 공간을 'World' 또는 'Screen'으로 설정
	WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
    
	// (옵션) 위젯 크기 지정
	WidgetComponent->SetDrawSize(FVector2D(300.f, 30.f));

	// (옵션) 살짝 위로 띄워서 아이템 상단에 표시하기
	WidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 100.f));

	WidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	// 시작 시에는 보이지 않도록 설정
	WidgetComponent->SetVisibility(true);

	bReplicates = true;
}

void AEnemyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// Replicated 변수 등록
	//DOREPLIFETIME(AEnemyCharacter, EnemyStatusHUDClass);
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	
	EnemyAttributeSet->InitBaseAttribute();
	EnemyAttributeSet->OnOutOfHealth.AddDynamic(this, &AEnemyCharacter::Die);

	Initialize();
	
	AbilitySystemComponent->OnDamageReactNotified
	.AddDynamic(this, &ThisClass::OnDamaged);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			EnemyAttributeSet->GetMovementSpeedAttribute()).AddUObject(
			this, &ThisClass::MovementSpeedChanged);

	if (!EnemyStatusHUDClass)
	{
		return;
	}
	// ✅ 기존 위젯을 제거하고 개별적으로 생성
	if (WidgetComponent)
	{
		// 기존 위젯 제거 (중복 방지)
		WidgetComponent->SetWidget(nullptr);

		// 새로운 위젯 생성
		EnemyStatus = CreateWidget<UEnemyStatus>(GetWorld(), EnemyStatusHUDClass);
		if (EnemyStatus)
		{
			EnemyStatus->SetBaseCharacter(this);  // ✅ 개별 캐릭터 연결
			WidgetComponent->SetWidget(EnemyStatus); // ✅ 위젯 설정
		}
	}
}

void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnemyCharacter::Die()
{
	//Super::Die();

	UE_LOG(LogTemp, Warning, TEXT("Die() called on %s. Role: %s"), 
		*GetName(), 
		*UEnum::GetValueAsString(GetLocalRole())); // 실행된 네트워크 역할 확인

	
	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Die on SERVER for %s"), *GetName());
	//	SetLifeSpan(3.0f);
		ActiveDeathAbility();

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Die from CLIENT for %s"), *GetName());
		ServerDestroy();
	}
}
void AEnemyCharacter::ServerDestroy_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("ServerDestroy_Implementation() called on SERVER for %s"), *GetName());
	//MulticastDestroy();
	ActiveDeathAbility();
	//SetLifeSpan(3.0f);
}
void AEnemyCharacter::MulticastDestroy_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("MulticastDestroy() called on %s. Role: %s"), 
		*GetName(), 
		*UEnum::GetValueAsString(GetLocalRole()));

	Destroy();
}
bool AEnemyCharacter::ServerDestroy_Validate()
{
	return true; // 보안 체크가 필요하면 추가 가능
}

void AEnemyCharacter::OnDamaged_Implementation(const FDamagedResponse& DamagedResponse)
{
	if (EnemyAttributeSet)
	{
		EnemyAttributeSet->SetHealth(EnemyAttributeSet->GetHealth() - DamagedResponse.Damage);
	}
	if (EnemyAttributeSet)
	{
		if(EnemyAttributeSet->GetHealth()<=0)
		{
			if (DamagedResponse.SourceActor)
			{
				APlayerCharacter* PlayerCharacter = (APlayerCharacter*)DamagedResponse.SourceActor;
				if (PlayerCharacter)
				{
					PlayerCharacter->SetTargetingCharacter(nullptr);
				}
			}
		}
	}
	// TODO: 이거 별도의 Ability로 빼는 것도 고려할 필요 있음.
	// 근데 고려만 할 것
	PlayAnimMontage(EquipComponent->GetDamagedAnim()
	.GetAnimationByDirection(DamagedResponse.DamagedDirection, DamagedResponse.DamagedHeight));

	DamagedMontage = EquipComponent->GetDamagedAnim()
							   .GetAnimationByDirection(DamagedResponse.DamagedDirection,
														DamagedResponse.DamagedHeight);
	if (DamagedMontage)
	{
		float MontageLength = DamagedMontage->GetPlayLength();
		OnKnockback(DamagedResponse, MontageLength);
		
	}
}




