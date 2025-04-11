// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Execution.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AT_Execution.h"
#include "ExecutionCameraActor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ProjectNL/Character/Player/PlayerCharacter.h"
#include "ProjectNL/Character/Enemy/EnemyCharacter.h"
#include "ProjectNL/GAS/Ability/Active/Default/DrinkPotion/GA_DrinkPotion.h"
#include "ProjectNL/GAS/Ability/Utility/PlayMontageWithEvent.h"
#include "ProjectNL/GAS/Attribute/BaseAttributeSet.h"

UGA_Execution::UGA_Execution(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
    //InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_Execution::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    if (!HasAuthority(&ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }
    
    
    //if (CanExecute(Target))
 //   {
        PerformExecution();
  //  }
    
   
}

void UGA_Execution::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
    APlayerCharacter* OwnerCharacter = Cast<APlayerCharacter>(GetAvatarActorFromActorInfo());
    AEnemyCharacter* TargetCharacter = OwnerCharacter->GetTargetingCharacter();
    UAbilitySystemComponent* TargetAsc = TargetCharacter->GetAbilitySystemComponent();
    const FGameplayEffectContextHandle EffectContext = TargetAsc->MakeEffectContext();

    FGameplayEffectSpecHandle SpecHandle = TargetAsc->MakeOutgoingSpec(ExecutionEffectClass, 1.0f, EffectContext);
    
    if (SpecHandle.IsValid())
    {
        TargetAsc->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetAsc);
    
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true
                        , false);
    }
}

void UGA_Execution::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true
                    , false);
}

void UGA_Execution::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
    EndExecution();
}

bool UGA_Execution::CanExecute(AActor* Target) const
{
    if (!Target) return false;

    // 예제: 체력이 일정 이하일 때만 처형 가능
   // UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
    // if (TargetASC)
    // {
    //     float Health = 0.0f;
    //     //TargetASC->GetNumericAttribute(FGameplayAttribute(), Health);
    //     return Health <= 20.0f; // 체력이 20 이하일 때 처형 가능
    // }

    return true;
}

void UGA_Execution::PerformExecution()
{

    APlayerCharacter* OwnerCharacter = Cast<APlayerCharacter>(GetAvatarActorFromActorInfo());
    AEnemyCharacter* TargetCharacter = OwnerCharacter->GetTargetingCharacter();

    if (TargetCharacter)
    {
        if(TargetCharacter->EnemyAttributeSet.Get()->GetHealth()>300){
            EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true
                    , false);
            return;
        }
    }
    if (OwnerCharacter && TargetCharacter)
    {
        UAT_Execution* AdjustTask = UAT_Execution::InitialEvent(this, OwnerCharacter, TargetCharacter, 150.0f);
        AdjustTask->OnCompleted.AddDynamic(this, &UGA_Execution::OnPositionAdjusted);
        AdjustTask->ReadyForActivation();
    }
}
void UGA_Execution::PlayExcutionAnimation(int8 arrNum,const TArray<UAnimMontage*> ArrExecutionAnimMontage,const ABaseCharacter* CurrentCharacter)
{
    UAbilitySystemComponent* TargetASC = CurrentCharacter->GetAbilitySystemComponent();

    PlayMontageWithEventTask = UPlayMontageWithEvent::InitialEvent(this, NAME_None
                                                                                        , ArrExecutionAnimMontage[arrNum]
                                                                                        , FGameplayTagContainer());
    PlayMontageWithEventTask->SetAbilitySystemComponent(TargetASC);
    PlayMontageWithEventTask->OnCancelled.AddDynamic(this, &ThisClass::OnCancelled);
    PlayMontageWithEventTask->OnInterrupted.AddDynamic(this, &ThisClass::OnCancelled);
    PlayMontageWithEventTask->OnBlendOut.AddDynamic(this, &ThisClass::OnCompleted);
    PlayMontageWithEventTask->OnCompleted.AddDynamic(this, &ThisClass::OnCompleted);
    PlayMontageWithEventTask->ReadyForActivation();
    
}



void UGA_Execution::ApplyExecutionDamage(UAbilitySystemComponent* TargetASC)
{
    if (!TargetASC) return;

    FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(ExecutionEffectClass, 1.0f);
    if (SpecHandle.IsValid())
    {
        TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
    }
}
void UGA_Execution::OnPositionAdjusted()
{
    
    APlayerCharacter* OwnerCharacter = Cast<APlayerCharacter>(GetAvatarActorFromActorInfo());
    AEnemyCharacter* TargetCharacter = OwnerCharacter->GetTargetingCharacter();

    if (OwnerCharacter && TargetCharacter)
    {
        StartExecution();
        // 내 캐릭터 애니메이션 실행
        PlayExcutionAnimation(0, ArrPlayerExecutionAnimMontage, Cast<ABaseCharacter>(OwnerCharacter));

        // 적 캐릭터 애니메이션 실행
        PlayExcutionAnimation(0, ArrEnemyExecutionAnimMontage, Cast<ABaseCharacter>(TargetCharacter));
    }
}
void UGA_Execution::EndExecution()
{
    APlayerCharacter* OwnerCharacter = Cast<APlayerCharacter>(GetAvatarActorFromActorInfo());
    if (!OwnerCharacter) return;
    OwnerCharacter->SetTargetingCharacter(nullptr);
    APlayerController* PC = Cast<APlayerController>(OwnerCharacter->GetController());
    if (PC)
    {
        PC->SetViewTargetWithBlend(OwnerCharacter, 0.5f);
    }

    // 추가적인 정리 로직
}
void UGA_Execution::StartExecution()
{
    APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetAvatarActorFromActorInfo());
    AEnemyCharacter* TargetEnemyCharacter = PlayerCharacter->GetTargetingCharacter();
    if (!PlayerCharacter || !TargetEnemyCharacter) return;

    // 실행 중인 카메라가 없다면 생성
    if (!ExecutionCameraActor)
    {
        ExecutionCameraActor = GetWorld()->SpawnActor<AExecutionCameraActor>(AExecutionCameraActor::StaticClass());
    }

    if (!ExecutionCameraActor) return;

    // 1️⃣ 플레이어와 적의 중간 지점 계산
    FVector PlayerLocation = PlayerCharacter->GetActorLocation();
    FVector EnemyLocation = TargetEnemyCharacter->GetActorLocation();
    FVector MidPoint = (PlayerLocation + EnemyLocation) * 0.5f;

    // 2️⃣ 방향 벡터 구하기 (플레이어 → 적)
    FVector DirectionBetween = (EnemyLocation - PlayerLocation).GetSafeNormal();

    // 3️⃣ 오른쪽 방향 벡터 계산 (90도 회전)
    FVector RightDirection = DirectionBetween.RotateAngleAxis(90.0f, FVector::UpVector);

    // 4️⃣ 카메라 위치 설정 (중간 지점 + 오른쪽으로 300, 위로 250)
    FVector CameraLocation = MidPoint + (RightDirection * 300.0f) + FVector(0, 0, 250.0f);

    // 5️⃣ 카메라가 중간 지점을 바라보도록 회전
    FVector LookAtDirection = (MidPoint - CameraLocation).GetSafeNormal();
    FRotator CameraRotation = FRotationMatrix::MakeFromX(LookAtDirection).Rotator();

    // 6️⃣ 카메라 위치 및 회전 적용
    ExecutionCameraActor->SetExecutionCameraTransform(CameraLocation, CameraRotation);

    // 7️⃣ 플레이어의 카메라를 ExecutionCameraActor로 변경
    APlayerController* PC = Cast<APlayerController>(PlayerCharacter->GetController());
    if (PC)
    {
        PC->SetViewTargetWithBlend(ExecutionCameraActor, 0.5f);
    }
}
