#include "GA_ComboAttack.h"

#include "ProjectNL/GAS/Ability/Utility/PlayMontageWithEvent.h"
#include "ProjectNL/Component/EquipComponent/EquipComponent.h"
#include "ProjectNL/Character/Player/PlayerCharacter.h"
#include "ProjectNL/Helper/GameplayTagHelper.h"
#include "ProjectNL/Helper/StateHelper.h"


UGA_ComboAttack::UGA_ComboAttack(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	ComboClearCooldown = 0;
	HoldDurationCooldown = 0.8;
}

bool UGA_ComboAttack::CanAttack() const
{
	if (!FStateHelper::IsCombatMode(GetAbilitySystemComponentFromActorInfo())) return false;
	if (!FStateHelper::IsPlayerIdle(GetAbilitySystemComponentFromActorInfo())) return false;
	return true;
}

bool UGA_ComboAttack::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo,
                                         const FGameplayTagContainer* SourceTags,
                                         const FGameplayTagContainer* TargetTags,
                                         FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags
	                               , OptionalRelevantTags))
	{
		return false;
	}

	return CanAttack();
}

void UGA_ComboAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle
                                      , const FGameplayAbilityActorInfo* ActorInfo
                                      , const FGameplayAbilityActivationInfo
                                      ActivationInfo
                                      , const FGameplayEventData* TriggerEventData)
{
	// 입력이 눌린 시각 기록
	InputPressedTime = FDateTime::Now();

	UE_LOG(LogTemp, Display, TEXT("Test Combo ComboAttackTask!!"));
}

void UGA_ComboAttack::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	if (APlayerController* PlayerController
		// TODO: 해당 코드는 Pawn 대상이 된다면 에러 발생할 수 있으니 추후 고쳐야함
		= Cast<APlayerController>(Cast<ACharacter>(GetAvatarActorFromActorInfo())->GetController()))
	{
		PlayerController->SetIgnoreMoveInput(false);
		//PlayerController->SetIgnoreLookInput(true);
		UE_LOG(LogTemp, Warning, TEXT("캐릭터 입력이 활성화 되었습니다."));
	}
	UE_LOG(LogTemp, Warning, TEXT("Test End"));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true
	           , false);
}

void UGA_ComboAttack::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true
	           , true);
	UE_LOG(LogTemp, Display, TEXT("Test Combo onCanceled"));
}

void UGA_ComboAttack::EndAbility(const FGameplayAbilitySpecHandle Handle
                                 , const FGameplayAbilityActorInfo* ActorInfo
                                 , const FGameplayAbilityActivationInfo
                                 ActivationInfo, bool bReplicateEndAbility
                                 , bool bWasCancelled)
{
	ABaseCharacter* CurrentCharacter = Cast<ABaseCharacter>(GetAvatarActorFromActorInfo());
	check(CurrentCharacter);
	if (APlayerController* PlayerController = Cast<APlayerController>(CurrentCharacter->GetController()))
	{
		PlayerController->SetIgnoreMoveInput(false);
		//PlayerController->SetIgnoreLookInput(true);
		UE_LOG(LogTemp, Warning, TEXT("캐릭터 입력이 활성화 되었습니다."));
	}
		
	UE_LOG(LogTemp, Display, TEXT("Test Combo EndAbility"));
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility
	                  , bWasCancelled);
}


void UGA_ComboAttack::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);

	if (!CanAttack())
	{
		return;
	}
	
	const FTimespan HoldDuration = FDateTime::Now() - InputPressedTime;
	// 지속 시간 초기화를 통해 이후 InputRelease에 이전 시간이 반영되지 않게 처리
	InputPressedTime = FDateTime::Now();
	// 태그에 따라 조건 분기
	if (GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(NlGameplayTags::Status_IsFalling))
	{
		ExecuteJumpAttack();
	}
	else
	{
		if (HoldDuration.GetTotalSeconds() < HoldDurationCooldown)
		{
			ExecuteComboAttack();
		}
		else
		{
			ExecuteHeavyAttack();
		}
	}
	UE_LOG(LogTemp, Display, TEXT("Test Combo InputReleased"));
}

void UGA_ComboAttack::ExecuteHeavyAttack()
{
	ABaseCharacter* CurrentCharacter = Cast<ABaseCharacter>(GetAvatarActorFromActorInfo());

	check(CurrentCharacter);
	check(CurrentCharacter->GetEquipComponent());
	
	CurrentCharacter->GetEquipComponent()->ClearCurrentComboCount();
	const TObjectPtr<UAnimMontage> HeavyAttack = CurrentCharacter->
	                                             GetEquipComponent()->GetHeavyAttackAnim();
	check(HeavyAttack)
	if (IsValid(AttackAnimTask))
	{
		AttackAnimTask->EndTask();
		AttackAnimTask = nullptr;
	}
	
	FStateHelper::ChangePlayerState(GetAbilitySystemComponentFromActorInfo(), NlGameplayTags::State_Idle, NlGameplayTags::State_Attack_Heavy, true);

	AttackAnimTask = UPlayMontageWithEvent::InitialEvent(this, NAME_None
	                                                      , HeavyAttack
	                                                      , FGameplayTagContainer());
	AttackAnimTask->OnCancelled.AddDynamic(this, &UGA_ComboAttack::OnCancelled);
	AttackAnimTask->OnBlendOut.AddDynamic(this, &UGA_ComboAttack::OnCompleted);
	AttackAnimTask->OnCompleted.AddDynamic(this, &UGA_ComboAttack::OnCompleted);
	AttackAnimTask->OnInterrupted.AddDynamic(this, &UGA_ComboAttack::OnCompleted);
	AttackAnimTask->ReadyForActivation();
}

void UGA_ComboAttack::ExecuteComboAttack()
{
	if (ABaseCharacter* CurrentCharacter = Cast<ABaseCharacter>(GetAvatarActorFromActorInfo()))
	{
		if (IsValid(AttackAnimTask))
		{
			AttackAnimTask->EndTask();
			AttackAnimTask = nullptr;
		}

		UEquipComponent* CharEquipInfo = CurrentCharacter->GetEquipComponent();
		check(CharEquipInfo);
		const TArray<TObjectPtr<UAnimMontage>> ComboAttack = CharEquipInfo->GetComboAttackAnim();

		if (!IsValid(ComboAttack[CharEquipInfo->GetAttackComboIndex()]))
		{
			UE_LOG(LogTemp, Error, TEXT("Fail to Load Combo Animation Try Again"));
			CharEquipInfo->ClearCurrentComboCount();
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo
			           , true, true);
		}

		SetCurrentMontage(ComboAttack[CharEquipInfo->GetAttackComboIndex()]);
		FStateHelper::ChangePlayerState(GetAbilitySystemComponentFromActorInfo(), NlGameplayTags::State_Idle, NlGameplayTags::State_Attack_Combo, true);
		
		AttackAnimTask = UPlayMontageWithEvent::InitialEvent(this, NAME_None
		                                                      , GetCurrentMontage()
		                                                      , FGameplayTagContainer());
		AttackAnimTask->OnCancelled.AddDynamic(this, &UGA_ComboAttack::OnCancelled);
		AttackAnimTask->OnBlendOut.AddDynamic(this, &UGA_ComboAttack::OnCompleted);
		AttackAnimTask->OnCompleted.AddDynamic(this, &UGA_ComboAttack::OnCompleted);
		AttackAnimTask->OnInterrupted.AddDynamic(this, &UGA_ComboAttack::OnCompleted);

		AttackAnimTask->ReadyForActivation();
	}
}

void UGA_ComboAttack::ExecuteJumpAttack()
{
    // 캐릭터 유효성 체크
	
	ABaseCharacter* CurrentCharacter = Cast<ABaseCharacter>(GetAvatarActorFromActorInfo());
    if (!CurrentCharacter)
    {
        UE_LOG(LogTemp, Error, TEXT("ExecuteJumpAttack: CurrentCharacter가 null입니다."));
        return;
    }

    // 레이캐스트로 바닥 거리 체크
    const FVector CharacterLocation = CurrentCharacter->GetActorLocation();
    const FVector TraceStart = CharacterLocation;
    const FVector TraceEnd = CharacterLocation - FVector(0, 0, 1000.0f); // 레이 길이 (1000 유닛)
    FHitResult HitResult;

    FCollisionQueryParams CollisionParams;
    CollisionParams.AddIgnoredActor(CurrentCharacter); // 캐릭터를 레이캐스트에서 제외

    // 레이캐스트 실행
    const bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitResult,
        TraceStart,
        TraceEnd,
        ECC_Visibility, // 바닥 감지용 충돌 채널
        CollisionParams
    );

    if (bHit)
    {
        const FVector HitLocation = HitResult.ImpactPoint; // 바닥에 맞은 지점
        const float DistanceToGround = (CharacterLocation - HitLocation).Size(); // 바닥과의 거리 계산

        // 디버그 스피어 표시 (히트 위치)
        DrawDebugSphere(GetWorld(), HitLocation, 20.0f, 12, FColor::Red, false, 2.0f); // 20.0f 반지름의 빨간색 스피어

        // 디버그 라인 표시 (캐릭터 위치 -> 바닥 히트 위치)
        DrawDebugLine(GetWorld(), TraceStart, HitLocation, FColor::Green, false, 2.0f, 0, 1.0f);

        // 거리 디버그 메시지 출력
        UE_LOG(LogTemp, Warning, TEXT("ExecuteJumpAttack: 바닥까지 거리 = %.2f"), DistanceToGround);

        // 최소 거리 조건 확인
        const float MinDistanceToAttack = 130.0f; // 공격 허용 최소 거리 (130 유닛)
        if (DistanceToGround < MinDistanceToAttack)
        {
            UE_LOG(LogTemp, Warning, TEXT("ExecuteJumpAttack: 바닥까지의 거리가 너무 가까워 공격을 실행할 수 없습니다."));
            return;
        }
    }
    else
    {
        // 바닥 감지 실패 처리
        UE_LOG(LogTemp, Warning, TEXT("ExecuteJumpAttack: 바닥이 감지되지 않았습니다. 공격 실행이 취소됩니다."));

        // 디버그 라인 표시 (바닥 미감지 시)
        DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Blue, false, 2.0f, 0, 1.0f);
        return;
    }

	// 캐릭터 입력 비활성화
	if (APlayerController* PlayerController = Cast<APlayerController>(CurrentCharacter->GetController()))
	{
    	PlayerController->SetIgnoreMoveInput(true);
    	//PlayerController->SetIgnoreLookInput(true);
    	UE_LOG(LogTemp, Warning, TEXT("ExecuteJumpAttack: 캐릭터 입력이 차단되었습니다."));
		// 점프 차단
		CurrentCharacter->StopJumping();
	}

    // 점프 공격 실행
	check(CurrentCharacter->
		GetEquipComponent());
	CurrentCharacter->GetEquipComponent()->ClearCurrentComboCount();
    const TObjectPtr<UAnimMontage> JumpAttackAnim = CurrentCharacter->
        GetEquipComponent()->GetJumpAttackAnim();
	check(JumpAttackAnim)
	
	GetAbilitySystemComponentFromActorInfo()
		->SetLooseGameplayTagCount(NlGameplayTags::State_Attack_Jump, 1);

    if (IsValid(AttackAnimTask))
    {
        AttackAnimTask->EndTask();
        AttackAnimTask = nullptr;
    }

    AttackAnimTask = UPlayMontageWithEvent::InitialEvent(this, NAME_None,
                                                         JumpAttackAnim,
                                                         FGameplayTagContainer());
    AttackAnimTask->OnCancelled.AddDynamic(this, &UGA_ComboAttack::OnCancelled);
    AttackAnimTask->OnBlendOut.AddDynamic(this, &UGA_ComboAttack::OnCompleted);
    AttackAnimTask->OnCompleted.AddDynamic(this, &UGA_ComboAttack::OnCompleted);
    AttackAnimTask->OnInterrupted.AddDynamic(this, &UGA_ComboAttack::OnCompleted);
    AttackAnimTask->ReadyForActivation();
}
