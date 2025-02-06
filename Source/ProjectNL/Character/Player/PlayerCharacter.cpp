#include "PlayerCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectNL/Component/CameraComponent/PlayerCameraComponent.h"
#include "ProjectNL/Component/CameraComponent/PlayerSpringArmComponent.h"
#include "ProjectNL/Component/EquipComponent/EquipComponent.h"
#include "ProjectNL/GAS/Ability/Active/Default/Knockback/GA_Knockback.h"
#include "ProjectNL/GAS/Attribute/PlayerAttributeSet.h"
#include "ProjectNL/Player/BasePlayerState.h"
#include "ProjectNL/Helper/EnumHelper.h"
#include "ProjectNL/Player/BasePlayerController.h"

APlayerCharacter::APlayerCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	GetCharacterMovement()->JumpZVelocity = 350.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 50.f;
	GetCharacterMovement()->MaxWalkSpeed = 280;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	
	//플레이어 스프링 암을 만듭니다
	PlayerCameraSpringArm = CreateDefaultSubobject<UPlayerSpringArmComponent>(TEXT("PlayerCameraSpringArm"));
	PlayerCameraSpringArm->SetupAttachment(RootComponent);
	PlayerCameraSpringArm->TargetArmLength = 400.0f; 
	PlayerCameraSpringArm->bUsePawnControlRotation = true;

	PlayerCamera = CreateDefaultSubobject<UPlayerCameraComponent>(TEXT("PlayerCamera"));
	PlayerCamera->SetupAttachment(PlayerCameraSpringArm, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	PlayerCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	SetEntityType(EEntityCategory::Player);
}

// PlayerState 변경 시에 대한 처리
// 변경되는 경우 보통 캐릭터도 같이 변하기에 한번 실행된다고 생각하면 편하다
void APlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (ABasePlayerState* PS = GetPlayerState<ABasePlayerState>())
	{
		AbilitySystemComponent = Cast<UNLAbilitySystemComponent>(
			PS->GetAbilitySystemComponent());

		AbilitySystemComponent->InitAbilityActorInfo(PS, this);

		PlayerAttributeSet = PS->AttributeSet;
		PlayerAttributeSet->InitBaseAttribute();
		
		AbilitySystemComponent->InitializeAbilitySystem(InitializeData);
		
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			PlayerAttributeSet->GetMovementSpeedAttribute()).AddUObject(
			this, &ThisClass::MovementSpeedChanged);
		
		AbilitySystemComponent->OnDamageReactNotified
		.AddDynamic(this, &APlayerCharacter::OnDamaged);
		Initialize();
	}
}

void APlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (ABasePlayerState* PS = GetPlayerState<ABasePlayerState>())
	{
		AbilitySystemComponent = Cast<UNLAbilitySystemComponent>(
			PS->GetAbilitySystemComponent());

		AbilitySystemComponent->InitAbilityActorInfo(PS, this);

		PlayerAttributeSet = PS->AttributeSet;
		PlayerAttributeSet->InitBaseAttribute();

		AbilitySystemComponent->InitializeAbilitySystem(InitializeData);
		
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			PlayerAttributeSet->GetMovementSpeedAttribute()).AddUObject(
			this, &ThisClass::MovementSpeedChanged);
		
		AbilitySystemComponent->OnDamageReactNotified
		.AddDynamic(this, &ThisClass::APlayerCharacter::OnDamaged);
		
		Initialize();
	}
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (const APlayerController* PC = Cast<APlayerController>(
		GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
				PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void APlayerCharacter::SetupPlayerInputComponent(
	UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<
		UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveInputAction, ETriggerEvent::Triggered
																			, this, &ThisClass::MoveTo);
		EnhancedInputComponent->BindAction(LookInputAction, ETriggerEvent::Triggered
																			, this, &ThisClass::Look);
	}
}


void APlayerCharacter::MoveTo(const FInputActionValue& Value)
{
	MovementVector = Value.Get<FVector2D>();
	if (const TObjectPtr<AController> ActorController = GetController())
	{
		const FRotator Rotation = ActorController->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		/**
		 * Move를 하나로 통일시켜도 정상 동작 하는 이유는 Input Mapping에서 전달하는 X,Y,Z값을
		 * 상하좌우에 따라 X,Y,Z 정렬 순서를 바꾸기 때문에 동일 동작이 가능하다
		 * ex. 상의 경우 Y,X,Z 순서대로 제공함으로써 EAxis::X = Y가 매핑되는 방식이다
		 * ex. 좌의 경우 X,Z,Y 순서대로 매핑되어서 EAxis:X = X가 매핑된다
		 */
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X) * MovementVector.Y;
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y) * MovementVector.X;

		// TODO: 둘다 동시에 누를 경우 1,1이 되기 때문에 대각선으로 더 멀리갈 수도 있다. (확인 필요)
		// 그렇기에 값을 조정해줘야할 필요가 있을 수도 있다.
		const FVector MoveToVector = (ForwardDirection + RightDirection).GetSafeNormal(1);
		
		this->AddMovementInput(MoveToVector);
	}
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		const float NewPitch = LookAxisVector.Y *
			UGameplayStatics::GetWorldDeltaSeconds(this) * 60.f;
		const float NewYaw = LookAxisVector.X *
			UGameplayStatics::GetWorldDeltaSeconds(this) * 60.f;

		AddControllerYawInput(NewYaw);
		AddControllerPitchInput(NewPitch);
	}
}

void APlayerCharacter::OnDamaged(const FDamagedResponse& DamagedResponse)
{
	if (PlayerAttributeSet)
	{
		// 체력 감소 처리
		PlayerAttributeSet->SetHealth(PlayerAttributeSet->GetHealth() - DamagedResponse.Damage);
	}
	FGameplayTagContainer ActiveTags;
	AbilitySystemComponent->GetOwnedGameplayTags(ActiveTags);
	DamageResponse=DamagedResponse;
	// 태그(Status.Sprint, Status.Dodge, Status.Roll) 활성화 여부 체크
	if (ActiveTags.HasTag(FGameplayTag::RequestGameplayTag(FName("Status.UnderAttack"))))
	{
		return;
	};
	// 플레이할 애니메이션 몽타주
	UAnimMontage* DamagedMontage = EquipComponent->GetDamagedAnim()
		.GetAnimationByDirection(DamagedResponse.DamagedDirection, DamagedResponse.DamagedHeight);
    
	if (!DamagedMontage) return;

	// 애니메이션 재생
	float MontageDuration = PlayAnimMontage(DamagedMontage);
	
	if (MontageDuration > 0.f)
	{
		// 애니메이션 인스턴스 가져오기
		UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
		if (AnimInstance)
		{
			// 플레이어의 입력 비활성화
			ABasePlayerController* PC = Cast<ABasePlayerController>(GetController());
			if (PC)
			{
				PC->SetIgnoreMoveInput(true);  // 키보드 이동 무시
			}
			if (DamagedResponse.DamageEffect)
			{
				// 이펙트 컨텍스트를 생성
				FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
        
				// 이 이펙트의 소스 오브젝트 설정 (보통 '소유자(Owner)'를 설정)
				EffectContext.AddSourceObject(Owner);
				
					AbilitySystemComponent->BP_ApplyGameplayEffectToSelf(DamagedResponse.DamageEffect,
					1.0f,    
					EffectContext);
				
			}

			// Anim Montage의 End에 대한 Delegate 등록
			MontageEndedDelegate.BindUObject(
				this, &APlayerCharacter::OnDamagedMontageEnded);
			AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate
																					, DamagedMontage);
		}
	}
	float MontageLength = DamagedMontage->GetPlayLength();
	OnKnockback(DamagedResponse,MontageLength);
}

// Montage가 끝나면 호출되는 함수
void APlayerCharacter::OnDamagedMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// 여기서 다시 입력 활성화
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		PC->SetIgnoreMoveInput(false);  // 키보드 이동 무시
	}
	// 다음에 중복으로 호출되지 않도록 델리게이트 해제(선택사항)
	AbilitySystemComponent->
			RemoveActiveGameplayEffectBySourceEffect(DamageResponse.DamageEffect, AbilitySystemComponent);
	
	UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
	if (AnimInstance)
	{
		AnimInstance->OnMontageEnded.RemoveDynamic(this, &APlayerCharacter::OnDamagedMontageEnded);
	}
}

void APlayerCharacter::OnKnockback(const FDamagedResponse& DamagedResponse,	float DamageMontageLength)
{
	
	UGA_Knockback* ActivatedKnockbackAbility =Cast<UGA_Knockback>( 	AbilitySystemComponent->FindAbilitySpecFromClass(InitializeData.KnockAbility)->GetPrimaryInstance());

	ActivatedKnockbackAbility->SetDamageResponse(DamageResponse);
	ActivatedKnockbackAbility->SetDamageMontageLength(DamageMontageLength);
	bool bActivated = AbilitySystemComponent->TryActivateAbilityByClass(InitializeData.KnockAbility);
	if(!bActivated)
	{
		return;
	}
}
