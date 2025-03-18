#include "ProjectNL/Character/Enemy/EnemyCharacter.h"
#include "AbilitySystemComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
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

	// UUserWidget* EnemyStatus = CreateWidget<UUserWidget>(GetWorld(), EnemyStatusHUDClass);
	//
	//EnemyStatus->AddToViewport();
	
	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetupAttachment(RootComponent);
    
	// (옵션) 위젯 공간을 'World' 또는 'Screen'으로 설정
	WidgetComponent->SetWidgetSpace(EWidgetSpace::World);
    
	// (옵션) 위젯 크기 지정
	WidgetComponent->SetDrawSize(FVector2D(300.f, 30.f));

	// (옵션) 살짝 위로 띄워서 아이템 상단에 표시하기
	WidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 700.f));
    
	// 시작 시에는 보이지 않도록 설정
	WidgetComponent->SetVisibility(true);
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
	WidgetComponentLookAtPlayer();
}

void AEnemyCharacter::OnDamaged_Implementation(const FDamagedResponse& DamagedResponse)
{
	if (EnemyAttributeSet)
	{
		EnemyAttributeSet->SetHealth(EnemyAttributeSet->GetHealth() - DamagedResponse.Damage);
	}
	// TODO: 이거 별도의 Ability로 빼는 것도 고려할 필요 있음.
	// 근데 고려만 할 것
	PlayAnimMontage(EquipComponent->GetDamagedAnim()
	.GetAnimationByDirection(DamagedResponse.DamagedDirection, DamagedResponse.DamagedHeight));
}

void AEnemyCharacter::WidgetComponentLookAtPlayer()
{
	// 혹은 InfoWidget이 유효하고 World 공간 모드일 때만 작업
	if (!WidgetComponent || WidgetComponent->GetWidgetSpace() != EWidgetSpace::World)
	{
		return;
	}

	// 1) 플레이어 카메라 매니저 가져오기
	APlayerCameraManager* CamManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	if (!CamManager) return;

	FVector CameraLocation = CamManager->GetCameraLocation();
	FVector WidgetLocation = WidgetComponent->GetComponentLocation();

	// 2) FindLookAtRotation(시작 위치, 목표 위치) 사용
	// → (Widget가 카메라를 '보게' 하려면, 시작=위젯, 목표=카메라)
	FRotator LookRot = UKismetMathLibrary::FindLookAtRotation(WidgetLocation, CameraLocation);

	// 3) 필요하다면 Pitch이나 Yaw만 사용 / Roll은 0 처리 등
	// **Pitch(상하)과 Roll(회전축 기울임)는 고정하고, Yaw만 유지**
	LookRot.Pitch = 0.f;
	LookRot.Roll = 0.f;

	// 4) 적용
	WidgetComponent->SetWorldRotation(LookRot);
}


