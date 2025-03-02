#include "ProjectNL/Character/Enemy/EnemyCharacter.h"
#include "AbilitySystemComponent.h"
#include "ProjectNL/Component/EquipComponent/EquipComponent.h"
#include "ProjectNL/GAS/Attribute/BaseAttributeSet.h"
#include "ProjectNL/GAS/NLAbilitySystemComponent.h"


AEnemyCharacter::AEnemyCharacter()
{
	AbilitySystemComponent = CreateDefaultSubobject<UNLAbilitySystemComponent>(
	"Ability System Component");
	EnemyAttributeSet = CreateDefaultSubobject<UBaseAttributeSet>(TEXT("AttributeSet"));
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


