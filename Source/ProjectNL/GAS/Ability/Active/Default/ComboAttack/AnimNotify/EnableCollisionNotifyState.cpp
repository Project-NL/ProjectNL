#include "EnableCollisionNotifyState.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "ProjectNL/Character/BaseCharacter.h"
#include "ProjectNL/Component/EquipComponent/EquipComponent.h"
#include "ProjectNL/Helper/LocateHelper.h"
#include "ProjectNL/Helper/StateHelper.h"
#include "ProjectNL/Weapon/BaseWeapon.h"

void UEnableCollisionNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
    UE_LOG(LogTemp, Log, TEXT("Notify Begin"));

    ABaseCharacter* Owner = Cast<ABaseCharacter>(MeshComp->GetOwner());

    UEquipComponent* EquipComponent{};
    if(Owner)
    {
        EquipComponent= Owner->GetEquipComponent();
    }
  
    ABaseWeapon* MainWeapon{};
    ABaseWeapon* SubWeapon{};
    if (EquipComponent)
    {
        MainWeapon = EquipComponent->GetMainWeapon();
        SubWeapon = EquipComponent->GetMainWeapon();
    }
    
    if (MainWeapon)
    {
        MainWeapon->SetPrevStartLocation(FVector::ZeroVector); 
        MainWeapon->SetPrevEndLocation(FVector::ZeroVector);
    }

    if (SubWeapon)
    {
        SubWeapon->SetPrevStartLocation(FVector::ZeroVector); 
        SubWeapon->SetPrevEndLocation(FVector::ZeroVector);
    }
  

}

void UEnableCollisionNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
    
    Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
    AActor* Owner = MeshComp->GetOwner();
    if (!Owner->HasAuthority())
    {
        return;
    }
    if (Owner)
    {
        StartTraceTriangle(Owner);
    }
}

void UEnableCollisionNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
   
    ABaseCharacter* Owner = Cast<ABaseCharacter>(MeshComp->GetOwner());
    if (!Owner)
    {
        UE_LOG(LogTemp, Warning, TEXT("Owner is null in NotifyEnd"));
        return;
    }
    
    UEquipComponent* EquipComponent{};
    if(Owner)
    {
        EquipComponent= Owner->GetEquipComponent();
    }
    
    ABaseWeapon* MainWeapon{};
    ABaseWeapon* SubWeapon{};
    if (EquipComponent)
    {
        MainWeapon=EquipComponent->GetMainWeapon();
        SubWeapon =EquipComponent->GetMainWeapon();
    }
    
    if (MainWeapon)
    {
        TSet<AActor*> &HitActors = MainWeapon->GetHitActorsReference();
        HitActors.Reset();
    }

    if (SubWeapon)
    {
        TSet<AActor*> &HitActors = SubWeapon->GetHitActorsReference();
        HitActors.Reset();
    }
    
    UAbilitySystemComponent* ASC = Owner->GetAbilitySystemComponent();
    if (!ASC)
    {
        UE_LOG(LogTemp, Warning, TEXT("OwnerASC is null in NotifyEnd"));
        return;
    }
    
    UE_LOG(LogTemp, Log, TEXT("Notify End"));
}

// 비멤버 함수로 이루어진 이유가 필요할 것 같아보임.
FVector BezierCurve(const FVector& P0, const FVector& P1, const FVector& P2, float T)
{
    // 2차 베지어 곡선 공식: B(t) = (1 - t)^2 * P0 + 2 * (1 - t) * t * P1 + t^2 * P2
    FVector Point = FMath::Pow(1.0f - T, 2) * P0;  // (1 - t)^2 * P0: 시작점에서 곡선이 시작하게 만듦
    Point += 2 * (1.0f - T) * T * P1;               // 2 * (1 - t) * t * P1: 제어점의 영향을 적용하여 곡선의 형태를 조정
    Point += FMath::Pow(T, 2) * P2;                              // t^2 * P2: 끝점에 도달하게 만듦
    
    return Point;
}

void UEnableCollisionNotifyState::StartTraceTriangle(AActor* Owner)
{
    ABaseCharacter* SourceCharacter = Cast<ABaseCharacter>(Owner);
    if (!SourceCharacter) return;
    
    if (UEquipComponent* EquipComponent = SourceCharacter->GetEquipComponent())
    {
        MakeTriangleTrace(Owner, EquipComponent->GetMainWeapon());
        MakeTriangleTrace(Owner, EquipComponent->GetSubWeapon());
    }
}


void UEnableCollisionNotifyState::MakeTriangleTrace(AActor* Owner, ABaseWeapon* Weapon)
{
    if (!IsValid(Weapon))
    {
        return;
    }
    
    FVector PrevStartLocation = Weapon->GetPrevStartLocation();
    FVector PrevEndLocation = Weapon->GetPrevEndLocation();
    
    USkeletalMeshComponent* SwordMesh = Weapon->GetWeaponSkeleton();
    if (SwordMesh && SwordMesh->DoesSocketExist(FName("SwordBoneStart")) && SwordMesh->DoesSocketExist(FName("SwordBoneEnd")))
    {
        FVector CurrentStartLocation = SwordMesh->GetSocketLocation(FName("SwordBoneStart"));
        FVector CurrentEndLocation = SwordMesh->GetSocketLocation(FName("SwordBoneEnd"));
        
        // 현재 Vector가 Zero 상태인 것은 즉 계산을 시도하지 않았다는 의미기에 별도의 처리 없이 끝낸다.
        // 즉 NotifyEnd때 초기화 해주면 불필요한 계산을 1회 줄일 수 있다.
        if (PrevStartLocation == FVector::ZeroVector && PrevEndLocation == FVector::ZeroVector)
        {
            Weapon->SetPrevStartLocation(CurrentStartLocation); 
            Weapon->SetPrevEndLocation(CurrentEndLocation);
            return;
        }

        // 충돌 검사할 액터들을 담을 배열
        TArray<FHitResult> HitResults;

        // Start와 End 위치의 중간 지점 계산
        FVector PrevMiddleLocation = (PrevStartLocation + PrevEndLocation) / 2;
        FVector CurrentMiddleLocation = (CurrentStartLocation + CurrentEndLocation) / 2;
        
        // 보간을 위해 이전 지점과 현재 지점 사이의 거리 계산
        float DistanceStart = FVector::Dist(PrevStartLocation, CurrentStartLocation);
        float DistanceEnd = FVector::Dist(PrevEndLocation, CurrentEndLocation);

        FVector EndDirection = (CurrentEndLocation - PrevEndLocation).GetSafeNormal();
        FVector StartDirection = (CurrentStartLocation - PrevStartLocation).GetSafeNormal();
        
        float DotProduct = FVector::DotProduct(EndDirection, StartDirection);
        float AngleRadians = FMath::Acos(DotProduct);

        // 라디안 값을 도 단위로 변환
        float AngleDegrees = FMath::RadiansToDegrees(AngleRadians);
        float AngleRatio = AngleDegrees / 45;

        // 보간할 간격(거리가 일정 이상일 경우 보간 수행)
        float InterpolationStep = 5.0f; // 간격을 설정 

        float NumStepsStart = DistanceStart / 10 + AngleRatio;
        float NumStepsEnd = DistanceEnd / 10 + AngleRatio;
        int32 NumSteps = FMath::CeilToInt(DistanceStart / InterpolationStep);
      
        // 2차 베지어 곡선을 위한 제어점 계산
        FVector ControlPointStart = (PrevStartLocation + CurrentStartLocation) / 2 ; // 임의로 위로 올린 제어점
        FVector ControlPointEnd = (PrevEndLocation + CurrentEndLocation) / 2 ;
        FVector ControlPointDirection = (ControlPointStart - ControlPointEnd).GetSafeNormal();
        
        ControlPointStart = ControlPointStart+ControlPointDirection * -NumStepsStart;
        ControlPointEnd = ControlPointEnd+ControlPointDirection * -NumStepsEnd;

        // 보간을 이용한 중간 지점들을 생성하여 충돌 처리 
        FVector InterPrevStartLocation;
        FVector InterPrevEndLocation;
        
        for (int32 i = 1; i <= NumSteps; i++)
        {
            // TODO: NumSteps는 이미 Int인데 굳이 float으로 바꿀 이유가 있는가?
            float Alpha = i / static_cast<float>(NumSteps);
  
            FVector InterpolatedStart = BezierCurve(PrevStartLocation, ControlPointStart, CurrentStartLocation, Alpha);
            FVector InterpolatedEnd = BezierCurve(PrevEndLocation,ControlPointEnd,CurrentEndLocation, Alpha);
            FVector InterpolatedMiddle = (InterpolatedStart + InterpolatedEnd) / 2;
            
            // 2차 베지어 곡선을 위한 제어점 계산
            if (InterPrevStartLocation != FVector::ZeroVector && InterPrevEndLocation != FVector::ZeroVector)
            {
                FVector InterPrevMiddleLocation=(InterPrevStartLocation + InterPrevEndLocation)/2;
                // 첫 번째 삼각형: PrevStart -> CurrentStart -> PrevEnd
                PerformTriangleTrace(Owner, InterPrevStartLocation, InterpolatedStart, InterPrevMiddleLocation, HitResults);

                // 두 번째 삼각형: CurrentStart -> CurrentEnd -> PrevEnd
                PerformTriangleTrace(Owner, InterpolatedStart, InterpolatedMiddle, InterPrevMiddleLocation, HitResults);

                // 추가 삼각형 1: 이전 Start, 현재 Start, 이전 Middle을 연결하는 삼각형
                PerformTriangleTrace(Owner, InterPrevMiddleLocation, InterpolatedMiddle, InterPrevEndLocation, HitResults);

                // 추가 삼각형 2: 현재 Start, 현재 Middle, 이전 Middle을 연결하는 삼각형
                PerformTriangleTrace(Owner, InterpolatedMiddle, InterPrevEndLocation, InterpolatedMiddle, HitResults);
            }
            

            // 보간된 지점에서 삼각형 충돌 처리
                PerformTriangleTrace(Owner, InterpolatedStart, InterpolatedEnd, PrevEndLocation, HitResults);
            
                // 디버그 라인 그리기
                DrawDebugLine(Owner->GetWorld(), InterpolatedStart, InterpolatedEnd, FColor::Yellow, false, 2.0f);
              //  //디버그 스피어 그리기 
                DrawDebugSphere(Owner->GetWorld(), InterpolatedStart, 2.0f, 12, FColor::Yellow, false, 2.0f); // 현재 프레임 시작지점
                DrawDebugSphere(Owner->GetWorld(), InterpolatedEnd, 2.0f, 12, FColor::Yellow, false, 2.0f); // 현재 프레임 끝지점
            
                InterPrevStartLocation=InterpolatedStart;
                InterPrevStartLocation=InterpolatedEnd;
           
        }
        ReactToHitActor(Owner, Weapon, HitResults);
    
        // 추가 삼각형 2: CurrentStartLocation -> PrevEnd -> CurrentEnd
        PerformTriangleTrace(Owner, CurrentMiddleLocation, PrevEndLocation, CurrentEndLocation, HitResults);
        
        // 추가 삼각형 1: PrevMiddle -> CurrentMiddle -> PrevEnd
        PerformTriangleTrace(Owner, PrevMiddleLocation, CurrentMiddleLocation, PrevEndLocation, HitResults);
        
        // 두 번째 삼각형: CurrentStart -> CurrentMiddle -> PrevMiddle
        PerformTriangleTrace(Owner, CurrentStartLocation, CurrentMiddleLocation, PrevMiddleLocation, HitResults);
        
        // 첫 번째 삼각형: PrevStart -> CurrentStart -> PrevMiddle
        PerformTriangleTrace(Owner, PrevStartLocation, CurrentStartLocation, PrevMiddleLocation, HitResults);


    
        
        // 디버그 라인 그리기
         DrawDebugLine(Owner->GetWorld(), PrevStartLocation, CurrentStartLocation, FColor::Blue, false, 2.0f);
         DrawDebugLine(Owner->GetWorld(), PrevEndLocation, CurrentEndLocation, FColor::Blue, false, 2.0f);
         DrawDebugLine(Owner->GetWorld(), PrevStartLocation, PrevEndLocation, FColor::Blue, false, 2.0f);
         DrawDebugLine(Owner->GetWorld(), CurrentStartLocation, CurrentEndLocation, FColor::Blue, false, 2.0f);
        
         DrawDebugSphere(Owner->GetWorld(), PrevStartLocation, 2.3f, 12, FColor::Red, false, 2.0f); // 현재 프레임 시작지점
         DrawDebugSphere(Owner->GetWorld(), PrevEndLocation, 2.3f, 12, FColor::Red, false, 2.0f); // 현재 프레임 끝지점
        
         DrawDebugSphere(Owner->GetWorld(), CurrentStartLocation, 2.3f, 12, FColor::Red, false, 2.0f); // 현재 프레임 시작지점
         DrawDebugSphere(Owner->GetWorld(), CurrentEndLocation, 2.3f, 12, FColor::Red, false, 2.0f); // 현재 프레임 끝지점
        
        // 현재 프레임 위치를 이전 프레임 위치로 업데이트
        Weapon->SetPrevStartLocation(CurrentStartLocation); 
        Weapon->SetPrevEndLocation(CurrentEndLocation);
    }
}

void UEnableCollisionNotifyState::ReactToHitActor(AActor* Owner, ABaseWeapon* Weapon, TArray<FHitResult>& HitResults)
{
    ABaseCharacter* SourceCharacter = Cast<ABaseCharacter>(Owner);
    if (!IsValid(SourceCharacter)) return;
    if (!IsValid(Weapon)) return;
    
    UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceCharacter);
    TSet<AActor*> &HitActors = Weapon->GetHitActorsReference();
    
    // 각 충돌된 액터에 대해 처리
    for (const FHitResult& Hit : HitResults)
    {
        if (AActor* HitActor = Hit.GetActor())
        {
            if (!HitActors.Contains(HitActor))
            {
                HitActors.Add(HitActor);
                ABaseCharacter* TargetCharacter = Cast<ABaseCharacter>(HitActor);
                // 적에게 충돌 시 효과 적용
                if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetCharacter))
                {
                    FGameplayEffectContextHandle EffectContext = SourceASC->MakeEffectContext();
                    EffectContext.AddHitResult(Hit); // 🔥 핵심 추가!

                    FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(Weapon->GetAttackEffect(), 1.0f, EffectContext);

                    const FRotator RotateValue = UKismetMathLibrary::FindLookAtRotation(TargetCharacter->GetActorLocation(), Hit.ImpactPoint);

                    SpecHandle.Data.Get()->SetByCallerNameMagnitudes.Add(NlGameplayTags::Data_AttackDirection.GetModuleName(), static_cast<uint8>(FLocateHelper::GetDirectionByAngle(RotateValue.Yaw)));
                    SpecHandle.Data.Get()->SetByCallerNameMagnitudes.Add(NlGameplayTags::Data_IsHitStop.GetModuleName(), IsHitStop);

                    if (SpecHandle.IsValid())
                    {
                        SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);

                        // CueParams도 동일한 Context를 넘겨줌
                        FGameplayCueParameters CueParams;
                        CueParams.Location = Hit.ImpactPoint;
                        CueParams.Normal = Hit.ImpactNormal;
                        CueParams.Instigator = Owner;
                        CueParams.EffectCauser = Owner;
                        CueParams.EffectContext = EffectContext; // ✅ 같이 넘기기
                        if (TargetASC->HasMatchingGameplayTag(NlGameplayTags::Status_Guard))
                        {
                            SourceASC->ExecuteGameplayCue(NlGameplayTags::GameplayCue_Particle_GaurdHit, CueParams);    
                        }
                        else
                        {
                            SourceASC->ExecuteGameplayCue(NlGameplayTags::GameplayCue_Particle_Hit, CueParams);    
                        }
                        

                        DrawDebugSphere(Owner->GetWorld(), Hit.ImpactPoint, 10, 12, FColor::Yellow, false, 1.0f);
                    }
                }
            }
        }
    }
}


void UEnableCollisionNotifyState::PerformTriangleTrace(AActor* Owner, FVector Point1, FVector Point2, FVector Point3, TArray<FHitResult>& OutHitResults)
{
    FCollisionQueryParams CollisionParams;
    CollisionParams.AddIgnoredActor(Owner);

    // 각 변을 라인 트레이스로 검사
    Owner->GetWorld()->LineTraceMultiByChannel(OutHitResults, Point2, Point3, ECC_WorldDynamic, CollisionParams);
    Owner->GetWorld()->LineTraceMultiByChannel(OutHitResults, Point3, Point1, ECC_WorldDynamic, CollisionParams);
    Owner->GetWorld()->LineTraceMultiByChannel(OutHitResults, Point1, Point2, ECC_WorldDynamic, CollisionParams);
}
