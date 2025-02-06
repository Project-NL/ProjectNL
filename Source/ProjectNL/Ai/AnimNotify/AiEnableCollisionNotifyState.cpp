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
	ActionSequenceData = GAActionAbility->GetActionSequenceData();
	

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
		UAbilitySystemComponent* ASC = BaseCharcater->GetAbilitySystemComponent();
		if (!ActionSequenceData) {
			UGA_Action* GAActionAbility = AbilityHelper::FindActiveGAAbility<UGA_Action>(ASC);
			if (!GAActionAbility)
			{
				return;
			}
			ActionSequenceData = GAActionAbility->GetActionSequenceData();
		}
		else {
			
			StartTrace(Owner);
		}
	
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
			MeshComponent->DoesSocketExist(ActionSequenceData->CollisionData.StartBoneName) &&
			MeshComponent->DoesSocketExist(ActionSequenceData->CollisionData.EndBoneName))
		{
			// 이번(현재) 프레임의 Start/End Bone 월드 위치
			FVector CurrentStartLocation = MeshComponent->GetSocketLocation(ActionSequenceData->CollisionData.StartBoneName);
			FVector CurrentEndLocation = MeshComponent->GetSocketLocation(ActionSequenceData->CollisionData.EndBoneName);

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
			FTransform StartBoneTransform = SwordMesh->GetSocketTransform(ActionSequenceData->CollisionData.StartBoneName, ERelativeTransformSpace::RTS_World);

			ActionSequenceData->CollisionData.DebugDrawCollision(
				Owner->GetWorld(),
				SwordMesh,  // 디버그 그리기에 사용할 Bone 기준 트랜스폼
				10.0f,                // 디버그 유지 시간(1초)
				2.0f                 // 라인 두께
			);

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
void UAiEnableCollisionNotifyState::PerformShapeSweepTrace(
	AActor* Owner,
	const FVector& Start,
	const FVector& End,
	TArray<FHitResult>& OutHitResults)
{
	if (!Owner || !Owner->GetWorld()) return;

	// 충돌체의 크기(구체형) 정의 (반지름 값 설정)
	float SweepRadius = 10.0f;
	FCollisionShape CollisionShape = ActionSequenceData->CollisionData.ToCollisionShape();

	// 트레이스 쿼리 파라미터 설정
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = false;
	QueryParams.AddIgnoredActor(Owner);

	// 임시 배열에 결과를 저장한 뒤, OutHitResults에 Append
	TArray<FHitResult> TempHitResults;
	bool bHit = Owner->GetWorld()->SweepMultiByChannel(
		TempHitResults,
		Start,
		End,
		FQuat::Identity,
		ECC_WorldDynamic, // 동적 월드 채널을 사용
		CollisionShape,
		QueryParams
	);

	if (bHit)
	{
		OutHitResults.Append(TempHitResults);
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
					FGameplayEffectContextHandle EffectContext = TargetASC->MakeEffectContext();
					EffectContext.AddSourceObject(Owner);

					FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(
						Weapon->GetAttackEffect(),
						1.0f,
						EffectContext
					);

					// 회전값 계산
					FRotator RotateValue = UKismetMathLibrary::FindLookAtRotation(
						TargetCharacter->GetActorLocation(),
						Hit.ImpactPoint
					);
					// 공격 방향 설정 (데이터 기반)
					SpecHandle.Data.Get()->SetByCallerNameMagnitudes.Add(
						NlGameplayTags::Data_AttackDirection.GetModuleName(),
						static_cast<uint8>(FLocateHelper::GetDirectionByAngle(RotateValue.Yaw))
					);

					// 적용
					if (SpecHandle.IsValid())
					{
						SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
					}

					// 충돌 지점을 디버그로 표시
					DrawDebugSphere(Owner->GetWorld(), Hit.ImpactPoint, 10.0f, 12, FColor::Yellow, false, 1.0f);
				}
			}
		}
	}
}
