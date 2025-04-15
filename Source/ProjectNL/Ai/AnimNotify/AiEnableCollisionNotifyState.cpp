#include "AiEnableCollisionNotifyState.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "ProjectNL/Character/BaseCharacter.h"
#include "ProjectNL/Component/EquipComponent/EquipComponent.h"
#include "ProjectNL/Helper/LocateHelper.h"
#include "ProjectNL/Helper/StateHelper.h"
#include "ProjectNL/Weapon/BaseWeapon.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "ProjectNL/GAS/Ability/Active/Default/Action/GA_Action.h"
#include "ProjectNL/Helper/AbilityHelper.h"

void UAiEnableCollisionNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);
	UE_LOG(LogTemp, Log, TEXT("Notify Begin"));
	ABaseCharacter* Owner = Cast<ABaseCharacter>(MeshComp->GetOwner());
	if (!Owner)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] Owner is not a valid ABaseCharacter."), *FString(__FUNCTION__));
		return;
	}
	UEquipComponent* EquipComponent = Owner->GetEquipComponent();;
	UAbilitySystemComponent* ASC = Owner->GetAbilitySystemComponent();

	UGA_Action* GAActionAbility= AbilityHelper::FindActiveGAAbility<UGA_Action>(ASC);
	if (!GAActionAbility)
	{
		return;
	}
	

	ABaseWeapon* MainWeapon{};
	ABaseWeapon* SubWeapon{};
	if
	(EquipComponent)
	{
		MainWeapon = EquipComponent->GetMainWeapon();
		SubWeapon = EquipComponent->GetSubWeapon();
	}

	if
	(MainWeapon)
	{
		MainWeapon->SetPrevStartLocation(FVector::ZeroVector);
		MainWeapon->SetPrevEndLocation(FVector::ZeroVector);
	}

	if
	(SubWeapon)
	{
		SubWeapon->SetPrevStartLocation(FVector::ZeroVector);
		SubWeapon->SetPrevEndLocation(FVector::ZeroVector);
	}
}

void UAiEnableCollisionNotifyState::NotifyTick(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	float FrameDeltaTime,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (AActor* Owner = MeshComp->GetOwner())
	{
		ABaseCharacter* BaseCharcater = Cast<ABaseCharacter>(Owner);
		if (!BaseCharcater) {
			return;
		}
			StartTrace(Owner);
	}
}

void UAiEnableCollisionNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	ABaseCharacter* Owner = Cast<ABaseCharacter>(MeshComp->GetOwner());
	if (!Owner)
	{
		UE_LOG(LogTemp, Warning, TEXT("Owner is null in NotifyEnd"));
		return;
	}

	UEquipComponent* EquipComponent{};
	if (Owner)
	{
		EquipComponent = Owner->GetEquipComponent();
	}

	ABaseWeapon* MainWeapon{};
	ABaseWeapon* SubWeapon{};
	if (EquipComponent)
	{
		MainWeapon = EquipComponent->GetMainWeapon();
		SubWeapon = EquipComponent->GetSubWeapon();
	}
	
	if (MainWeapon)
	{
		TSet<AActor*>& HitActors = MainWeapon->GetHitActorsReference();
		HitActors.Reset();
	}
	if (SubWeapon)
	{
		TSet<AActor*>& HitActors = SubWeapon->GetHitActorsReference();
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


void UAiEnableCollisionNotifyState::StartTrace( AActor* Owner)
{
	ABaseCharacter* SourceCharacter = Cast<ABaseCharacter>(Owner);
	if (!SourceCharacter) return;

	if (UEquipComponent* EquipComponent = SourceCharacter->GetEquipComponent())
	{
		MakeSweepTrace(Owner, EquipComponent->GetMainWeapon());
		MakeSweepTrace(Owner, EquipComponent->GetSubWeapon());
	}
}

// ------------------------------
// [핵심 로직] "무기 충돌 검사"를 실행
// ------------------------------
void UAiEnableCollisionNotifyState::MakeSweepTrace(AActor* Owner, ABaseWeapon* Weapon)
{
	// 유효성 체크: Weapon이 유효하지 않으면 함수 종료
	if (!IsValid(Weapon)) return;

	// 이전 프레임에 기록해 둔 무기의 Start/End Bone 위치를 가져옴
	FVector PrevStartLocation = Weapon->GetPrevStartLocation();
	FVector PrevEndLocation = Weapon->GetPrevEndLocation();

	// 무기 스켈레탈 메쉬를 얻어옴
	USkeletalMeshComponent* SwordMesh = Weapon->GetWeaponSkeleton();

	TArray<USkeletalMeshComponent*> SkeletalMeshComponents;
	Owner->GetComponents<USkeletalMeshComponent>(SkeletalMeshComponents);
	SkeletalMeshComponents.Add(SwordMesh);
	for (USkeletalMeshComponent* MeshComponent : SkeletalMeshComponents)
	{
		// 각 SkeletalMeshComponent에 대해 소켓이 존재하는지 확인
		if (MeshComponent &&
			MeshComponent->DoesSocketExist("SwordBoneStart") &&
			MeshComponent->DoesSocketExist("SwordBoneEnd"))
		{
			// 이번(현재) 프레임의 Start/End Bone 월드 위치
			CurrentStartLocation = MeshComponent->GetSocketLocation("SwordBoneStart");
			 CurrentEndLocation = MeshComponent->GetSocketLocation("SwordBoneEnd");

			// 첫 프레임(또는 초기화 직후)에는 Prev 위치가 (0,0,0)이므로
			// 충돌 검사를 할 수 없으니 Prev 위치를 현재 위치로 세팅 후 바로 종료
			if (PrevStartLocation == FVector::ZeroVector && PrevEndLocation == FVector::ZeroVector)
			{
				Weapon->SetPrevStartLocation(CurrentStartLocation);
				Weapon->SetPrevEndLocation(CurrentEndLocation);
				return;
			}


			// 1) "StartBone"이 이동한 경로만큼 SweepTrace
			TArray<FHitResult> HitResults;

			FVector CurrentCenter = (CurrentStartLocation + CurrentEndLocation) * 0.5f;
			FVector PrevCenter = (PrevStartLocation + PrevEndLocation) * 0.5f;
			PerformShapeSweepTrace(Owner, PrevCenter, CurrentCenter, HitResults);

			// ---------------------------------------------------------------------
			// (디버그 표시) 
			// - CollisionData.DebugDrawCollision()을 이용해 무기의 충돌 영역을 그려보고 싶다면,
			//   특정 Bone을 기준으로 할 수 있도록 소켓 트랜스폼을 직접 구해 전달.
			// ---------------------------------------------------------------------
			FTransform StartBoneTransform = SwordMesh->GetSocketTransform("SwordBoneStart", ERelativeTransformSpace::RTS_World);
			
			// Sweep 경로(이전 -> 현재)를 시각적으로 보여주는 디버그 라인
			DrawDebugLine(Owner->GetWorld(), PrevStartLocation, CurrentStartLocation, FColor::Yellow, false, 1.0f);
			DrawDebugLine(Owner->GetWorld(), PrevEndLocation, CurrentEndLocation, FColor::Yellow, false, 1.0f);

			// 히트된 액터들에 대한 실제 로직(데미지 계산, 이펙트 생성 등)
			ReactToHitActor(Owner, Weapon, HitResults);

			// 이번 프레임 위치를 '다음 프레임의 이전 위치'로 저장
			Weapon->SetPrevStartLocation(CurrentStartLocation);
			Weapon->SetPrevEndLocation(CurrentEndLocation);
		}
	}
}

// ------------------------------
// [로직] 충돌체크 트레이스 실행 함수
// ------------------------------
#include "DrawDebugHelpers.h"  // DrawDebug 함수들을 사용하기 위해 포함

void UAiEnableCollisionNotifyState::PerformShapeSweepTrace(
	AActor* Owner,
	const FVector& Start,
	const FVector& End,
	TArray<FHitResult>& OutHitResults)
{
	if (!Owner || !Owner->GetWorld())
	{
		return;
	}
	
	FVector Direction = (CurrentEndLocation - CurrentStartLocation).GetSafeNormal(); // 방향 벡터
	FVector UpVector = FVector::UpVector;                              // 캡슐 기본 Z축
	FQuat CapsuleQuat = FQuat::FindBetweenNormals(UpVector, Direction); // Z축을 정렬
	
	// 캡슐형 충돌체의 크기 설정 (반지름과 반높이)
	float CapsuleRadius = 10.0f;
	float CapsuleHalfHeight = FVector::Dist(CurrentStartLocation, CurrentEndLocation)/2;
	FCollisionShape CollisionShape = FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight);
	
	// 충돌 쿼리 파라미터 설정
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = false;
	QueryParams.AddIgnoredActor(Owner);


	
	// 스윕 결과를 임시 배열에 저장
	TArray<FHitResult> TempHitResults;
	bool bHit = Owner->GetWorld()->SweepMultiByChannel(
		TempHitResults,
		CurrentStartLocation,
		CurrentEndLocation,
		CapsuleQuat,
		ECC_WorldDynamic, // 동적 월드 채널 사용
		CollisionShape,
		QueryParams
	);
	
	// 디버그 드로잉: 스윕 경로와 시작/종료 캡슐 표시
	UWorld* World = Owner->GetWorld();
	if (World)
	{
		FVector CurrentMiddleLocation=(CurrentStartLocation+CurrentEndLocation)/2;
		// 시작 위치의 캡슐 (녹색)
		DrawDebugCapsule(World, CurrentMiddleLocation, CapsuleHalfHeight, CapsuleRadius, CapsuleQuat, FColor::Green, false, 2.0f);
		// 종료 위치의 캡슐 (빨간색)
	//	DrawDebugCapsule(World, CurrentEndLocation, CapsuleHalfHeight, CapsuleRadius, CapsuleQuat, FColor::Red, false, 2.0f);
		// 시작과 종료를 잇는 선 (파란색)
		DrawDebugLine(World, CurrentStartLocation, CurrentEndLocation, FColor::Blue, false, 2.0f, 0, 1.0f);
	}

	// 히트가 발생했으면 결과를 OutHitResults에 추가하고, 히트 위치에 디버그 포인트 표시
	if (bHit)
	{
		OutHitResults.Append(TempHitResults);

		for (const FHitResult& Hit : TempHitResults)
		{
			DrawDebugPoint(World, Hit.ImpactPoint, 10.0f, FColor::Yellow, false, 1.0f);
		}
	}
}


// ------------------------------
// [로직] 충돌된 액터에 대한 처리 (중복 타격 방지, 이펙트 생성 등)
// ------------------------------
void UAiEnableCollisionNotifyState::ReactToHitActor(
	AActor* Owner,
	ABaseWeapon* Weapon,
	TArray<FHitResult>& HitResults)
{
	ABaseCharacter* SourceCharacter = Cast<ABaseCharacter>(Owner);
	if (!IsValid(SourceCharacter)) return;
	if (!IsValid(Weapon)) return;

	UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceCharacter);
	TSet<AActor*>& HitActors = Weapon->GetHitActorsReference();

	for (const FHitResult& Hit : HitResults)
	{
		if (AActor* HitActor = Hit.GetActor())
		{
			// 이미 타격된 액터는 중복 처리 방지
			if (!HitActors.Contains(HitActor))
			{
				HitActors.Add(HitActor);

				ABaseCharacter* TargetCharacter = Cast<ABaseCharacter>(HitActor);
				if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
					TargetCharacter))
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
						// 충돌 지점 시각화
					
				}
			}
		}
	}
}
