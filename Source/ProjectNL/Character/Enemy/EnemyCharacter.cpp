#include "ProjectNL/Character/Enemy/EnemyCharacter.h"
#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "ProjectNL/Character/Player/PlayerCharacter.h"
#include "ProjectNL/Component/EquipComponent/EquipComponent.h"
#include "ProjectNL/GAS/Attribute/BaseAttributeSet.h"
#include "ProjectNL/GAS/NLAbilitySystemComponent.h"
#include "ProjectNL/Helper/GameplayTagHelper.h"
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

	// DetectionSphere 생성
	DetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionSphere"));
	DetectionSphere->SetupAttachment(RootComponent);
	DetectionSphere->InitSphereRadius(3000.f);               // 범위: 800cm
	DetectionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	DetectionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
  
	// Overlap 이벤트 바인딩
	DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemyCharacter::OnPlayerEnter);
	DetectionSphere->OnComponentEndOverlap  .AddDynamic(this, &AEnemyCharacter::OnPlayerExit);
}

void AEnemyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// Replicated 변수 등록
	//DOREPLIFETIME(AEnemyCharacter, EnemyStatusHUDClass);
}
void AEnemyCharacter::OnPlayerEnter(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
									UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
									bool bFromSweep, const FHitResult& Sweep)
{
	if (Cast<APlayerCharacter>(OtherActor) && WidgetComponent)
	{
		WidgetComponent->SetVisibility(true);
		// 아직 위젯이 없으면 생성
	}


	if (BossEnemyStatusWidget)
	{
		APlayerCharacter* PlayerCharacter=Cast<APlayerCharacter>(OtherActor);
		if (!PlayerCharacter)
		{
			return;
		}
	//	NlGameplayTags::RemoveGameplayTag(PlayerCharacter->GetAbilitySystemComponent(),NlGameplayTags::Status_Invincibile,1,true);
		BossEnemyStatusWidget->AddToViewport();
		BossEnemyStatusWidget->SetBaseCharacter(this);
	}
	
}

void AEnemyCharacter::OnPlayerExit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
								   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Cast<APlayerCharacter>(OtherActor) && WidgetComponent)
	{
		WidgetComponent->SetVisibility(false);

		if (BossEnemyStatusWidget)
		{
			BossEnemyStatusWidget->RemoveFromParent();
			BossEnemyStatusWidget = nullptr;
		}
	}
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
		EnemyStatus = CreateWidget<UEnemyStatus>(GetWorld(), EnemyStatusHUDClass);
		if (EnemyStatus)
		{
			EnemyStatus->SetBaseCharacter(this);  // ✅ 개별 캐릭터 연결
			WidgetComponent->SetWidget(EnemyStatus); // ✅ 위젯 설정
			WidgetComponent->SetVisibility(false);
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

	FDamagedResponse DamageResponse=DamagedResponse;
	if (EnemyAttributeSet)
	{
		
		if (AbilitySystemComponent->HasMatchingGameplayTag(NlGameplayTags::Status_Guard))
		{
			DamageResponse.Damage =DamagedResponse.Damage/5;
		}
		
		EnemyAttributeSet->SetHealth(EnemyAttributeSet->GetHealth() - DamageResponse.Damage);
		
	}
	if (EnemyAttributeSet)
	{
		if(EnemyAttributeSet->GetHealth()<=0)
		{
			if (DamageResponse.SourceActor)
			{
				APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(DamageResponse.SourceActor);
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
	.GetAnimationByDirection(DamageResponse.DamagedDirection, DamageResponse.DamagedHeight));

	DamagedMontage = EquipComponent->GetDamagedAnim()
							   .GetAnimationByDirection(DamageResponse.DamagedDirection,
														DamageResponse.DamagedHeight);
	if (DamagedMontage)
	{
		float MontageLength = DamagedMontage->GetPlayLength();
		OnKnockback(DamageResponse, MontageLength);
		
	}
}




