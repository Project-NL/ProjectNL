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
	if (HasAuthority())
	{
		UE_LOG(LogTemp, Log, TEXT("[Server] ReceiveDamage 호출됨: Damage=%f, IsHitStop=%s, SourceActor=%s"),
			DamagedResponse.Damage,
			DamagedResponse.IsHitStop ? TEXT("true") : TEXT("false"),
			*GetNameSafe(DamagedResponse.SourceActor));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[Client] ReceiveDamage 호출됨: Damage=%f, IsHitStop=%s, SourceActor=%s"),
			DamagedResponse.Damage,
			DamagedResponse.IsHitStop ? TEXT("true") : TEXT("false"),
			*GetNameSafe(DamagedResponse.SourceActor));
	}
	// 플레이할 애니메이션 몽타주
	DamagedMontage = EquipComponent->GetDamagedAnim()
		.GetAnimationByDirection(DamagedResponse.DamagedDirection, DamagedResponse.DamagedHeight);
	if (DamagedMontage)
	{
		float MontageLength = DamagedMontage->GetPlayLength();
		OnKnockback(DamagedResponse,MontageLength);
	}
}



// Montage가 끝나면 호출되는 함수
void APlayerCharacter::OnDamagedMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	
	// 다음에 중복으로 호출되지 않도록 델리게이트 해제(선택사항)

	
	UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
	if (AnimInstance)
	{
		AnimInstance->OnMontageEnded.RemoveDynamic(this, &APlayerCharacter::OnDamagedMontageEnded);
	}
}

void APlayerCharacter::OnKnockback(const FDamagedResponse& DamagedResponse, float DamageMontageLength)
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
    FGameplayAbilitySpec AbilitySpec = AbilitySystemComponent->BuildAbilitySpecFromClass(DamagedResponse.KnockbackAbility, 1, INDEX_NONE);
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



