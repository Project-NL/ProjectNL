#include "BaseCharacter.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "ProjectNL/Helper/EnumHelper.h"

#include "ProjectNL/Component/EquipComponent/EquipComponent.h"
#include "ProjectNL/GAS/Attribute/BaseAttributeSet.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "ProjectNL/Component/SkillComponent/TimeRecallComponent.h"
#include "ProjectNL/GAS/Ability/Active/Default/Knockback/GA_Knockback.h"
#include "ProjectNL/Helper/AbilityHelper.h"
#include "ProjectNL/Helper/GameplayTagHelper.h"


ABaseCharacter::ABaseCharacter()
{
	EntityType = EEntityCategory::Undefined;
	EquipComponent = CreateDefaultSubobject<UEquipComponent>("Equip Component");
	TimeRecallComponent=CreateDefaultSubobject<UTimeRecallComponent>("TimeRecallComponent");
}
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::ActiveDeathAbility()
{
	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("DeathAbility: 클라이언트에서는 실행되지 않습니다."));
		return;	
	}
	
	// 1. AbilitySpec 생성 및 AbilitySystemComponent에 부여
	FGameplayAbilitySpec AbilitySpec = AbilitySystemComponent->BuildAbilitySpecFromClass(
		InitializeData.DeathAbility, 1, INDEX_NONE);
	FGameplayAbilitySpecHandle AbilityHandle = AbilitySystemComponent->GiveAbility(AbilitySpec);

	// 2. AbilityHandle을 통해 AbilitySpec 포인터를 가져옵니다.
	FGameplayAbilitySpec* Spec = AbilitySystemComponent->FindAbilitySpecFromHandle(AbilityHandle);
	if (!Spec)
	{
		UE_LOG(LogTemp, Warning, TEXT("AbilityHandle에 해당하는 AbilitySpec을 찾을 수 없습니다."));
		return;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AbilityHandle에 해당하는 AbilitySpec을 찾을 수 있습니다."));
	}

	// 3. Spec에서 Primary Instance를 가져옵니다.
	UGameplayAbility* AbilityInstance = Spec->GetPrimaryInstance();

	// 4. Knockback Ability 활성화 시도
	bool bActivated = AbilitySystemComponent->TryActivateAbility(AbilityHandle);
	if (!bActivated)
	{
		UE_LOG(LogTemp, Warning, TEXT("ActiveDeathAbility 활성화에 실패했습니다."));
		return;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ActiveDeathAbility 활성화에 성공했습니다."));
	}
}


void ABaseCharacter::Initialize()
{
	if (AbilitySystemComponent)
	{
		if (GetEquipComponent()->GetIsFirstEquipWeapon())
		{
			AbilitySystemComponent->AddLooseGameplayTag(NlGameplayTags::Status_Combat);
		}
	}

}

void ABaseCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
	if (AbilitySystemComponent)
	{
		if (GetCharacterMovement()->IsFalling())
		{
			AbilitySystemComponent->
				SetLooseGameplayTagCount(NlGameplayTags::Status_IsFalling, 1);
		} else
		{
			AbilitySystemComponent->
				SetLooseGameplayTagCount(NlGameplayTags::Status_IsFalling, 0);
		}
	}
}

void ABaseCharacter::OnDamaged(const FDamagedResponse& DamagedResponse)
{
}

void ABaseCharacter::OnDamagedMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
}


void ABaseCharacter::Server_ApplyGameplayEffectToSelf_Implementation(
	TSubclassOf<UGameplayEffect> Effect, const uint32 Level)
{
	if (GetLocalRole() != ROLE_Authority)
	{
		Server_ApplyGameplayEffectToSelf(Effect);
		return;
	}

	if (!IsValid(AbilitySystemComponent))
	{
		return;
	}

	AbilitySystemComponent->ApplyGameplayEffectToSelf(
		Effect.GetDefaultObject(), Level
		, AbilitySystemComponent->MakeEffectContext());
}

void
ABaseCharacter::Server_RemoveActiveGameplayEffectBySourceEffect_Implementation(
	TSubclassOf<UGameplayEffect> Effect)
{
	if (GetLocalRole() != ROLE_Authority)
	{
		Server_ApplyGameplayEffectToSelf(Effect);
		return;
	}

	if (!IsValid(AbilitySystemComponent))
	{
		return;
	}

	AbilitySystemComponent->RemoveActiveGameplayEffectBySourceEffect(
		Effect, AbilitySystemComponent);
}


void ABaseCharacter::MovementSpeedChanged(const FOnAttributeChangeData& Data)
{
	const float MovementSpeed = Data.NewValue;

	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
}

void ABaseCharacter::SetViewItemOnHand(const FItemInfoData& NewItemInfo)
{
	
}
void ABaseCharacter::OnKnockback(const FDamagedResponse& DamagedResponse, float DamageMontageLength)
{
	// 서버에서만 실행 (클라이언트에서 실행 시 바로 리턴)
	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("OnKnockback: 클라이언트에서는 실행되지 않습니다."));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[Server] OnKnockback 호출됨: Damage=%f, IsHitStop=%s, SourceActor=%s"),
	       DamagedResponse.Damage,
	       DamagedResponse.IsHitStop ? TEXT("true") : TEXT("false"),
	       *GetNameSafe(DamagedResponse.SourceActor));

	// 1. AbilitySpec 생성 및 AbilitySystemComponent에 부여
	FGameplayAbilitySpec AbilitySpec = AbilitySystemComponent->BuildAbilitySpecFromClass(
		DamagedResponse.KnockbackAbility, 1, INDEX_NONE);
	FGameplayAbilitySpecHandle AbilityHandle = AbilitySystemComponent->GiveAbility(AbilitySpec);

	// 2. AbilityHandle을 통해 AbilitySpec 포인터를 가져옵니다.
	FGameplayAbilitySpec* Spec = AbilitySystemComponent->FindAbilitySpecFromHandle(AbilityHandle);
	if (!Spec)
	{
		UE_LOG(LogTemp, Warning, TEXT("AbilityHandle에 해당하는 AbilitySpec을 찾을 수 없습니다."));
		return;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AbilityHandle에 해당하는 AbilitySpec을 찾을 수 있습니다."));
		//return;
	}
	// 3. Spec에서 Primary Instance를 가져옵니다.
	UGameplayAbility* AbilityInstance = Spec->GetPrimaryInstance();


	// 4. UGA_Knockback 타입으로 캐스팅합니다.
	UGA_Knockback* ActivatedKnockbackAbility = Cast<UGA_Knockback>(AbilityInstance);
	if (!ActivatedKnockbackAbility)
	{
		UE_LOG(LogTemp, Warning, TEXT("AbilityInstance를 UGA_Knockback으로 캐스팅하는 데 실패했습니다."));
		return;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AbilityInstance를 UGA_Knockback으로 캐스팅하는 데 성공."));
		//return;
	}
	// 5. Knockback Ability에 데미지 관련 정보 및 애니메이션 길이 설정
	ActivatedKnockbackAbility->SetDamageResponse(DamagedResponse);
	ActivatedKnockbackAbility->SetDamageMontageLength(DamageMontageLength);
	ActivatedKnockbackAbility->SetDamageMontage(DamagedMontage);
	//ActivatedKnockbackAbility->OnPlayMontageWithEventDelegate.Clear();

	// 6. Knockback Ability 활성화 시도
	bool bActivated = AbilitySystemComponent->TryActivateAbilityByClass(DamagedResponse.KnockbackAbility);
	if (!bActivated)
	{
		UE_LOG(LogTemp, Warning, TEXT("Knockback Ability 활성화에 실패했습니다."));
		return;
	}

}
