// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectNL/GAS/Ability/Active/Default/Targeting/AT_TargetingEnemy.h"

#include "Engine/OverlapResult.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "ProjectNL/Character/Enemy/EnemyCharacter.h"
#include "ProjectNL/Character/Player/PlayerCharacter.h"
#include "ProjectNL/Component/CameraComponent/PlayerCameraComponent.h"
#include "ProjectNL/Component/CameraComponent/PlayerSpringArmComponent.h"
#include "ProjectNL/GAS/Attribute/BaseAttributeSet.h"
#include "ProjectNL/Helper/GameplayTagHelper.h"

UAT_TargetingEnemy* UAT_TargetingEnemy::InitialEvent(UGameplayAbility* OwningAbility,TSubclassOf<UGameplayEffect> TargetingSpeedEffect)
{
	UAT_TargetingEnemy* NewEvent = NewAbilityTask<UAT_TargetingEnemy>(
		OwningAbility);
	NewEvent->TargetingSpeedEffect=TargetingSpeedEffect;
	return NewEvent;
}

void UAT_TargetingEnemy::Activate()
{
	Super::Activate();
	bTickingTask = true;
	
	SaveCameraSettings(GetAvatarActor()->FindComponentByClass<UPlayerCameraComponent>());//카메라의 초기 세팅을 저장해준다 복구할 때 사용
	SaveSpringArmSettings(GetAvatarActor()->FindComponentByClass<UPlayerSpringArmComponent>()); 
	TargetNearestEnemy();
}


void UAT_TargetingEnemy::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);
	
	// 예: 최대 거리 1000.0f (원하는 값으로 조절하세요)
	CheckNearestEnemyDistance(1700.0f);
	TargetingNearestEnemy(DeltaTime);
}
void UAT_TargetingEnemy::TargetingNearestEnemy(float DeltaTime)
{
	if (!NearestEnemy)
	{
		// NearestEnemy가 없으면 캐릭터 상태 원복
		BacktoSquareOne(DeltaTime);
		return;
	}

	// 플레이어 캐릭터 확인
	if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetAvatarActor()))
	{
		// ASC 확인
		if (UAbilitySystemComponent* ASC = PlayerCharacter->GetAbilitySystemComponent())
		{
			FGameplayTagContainer ActiveTags;
			ASC->GetOwnedGameplayTags(ActiveTags);

			// 태그(Status.Sprint, Status.Dodge, Status.Roll) 활성화 여부 체크
			if (ActiveTags.HasTag(FGameplayTag::RequestGameplayTag(FName("Status.Sprint"))) ||
				ActiveTags.HasTag(FGameplayTag::RequestGameplayTag(FName("Status.Dodge"))) ||
				ActiveTags.HasTag(FGameplayTag::RequestGameplayTag(FName("Status.Roll"))))
			{
				SavedLockonActorRotation = PlayerCharacter->GetActorRotation();
			}
			else
			{
				LockOnTarget(NearestEnemy, DeltaTime);
			}
		}
	}

	// 플레이어 컨트롤러 회전(카메라 혹은 캐릭터 회전)에 관한 로직
	PlayerContollerRotation(DeltaTime);
}

void UAT_TargetingEnemy::TargetNearestEnemy()
{
	if (!NearestEnemy)
	{
		NearestEnemy = FindNearestTarget();

		if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetAvatarActor()))
		{
			if (UAbilitySystemComponent* ASC = PlayerCharacter->GetAbilitySystemComponent())
			{
				if (TargetingSpeedEffect)
				{
					FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
					EffectContext.AddSourceObject(GetAvatarActor());

					FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(
						TargetingSpeedEffect, 1.0f, EffectContext);

					if (SpecHandle.IsValid())
					{
						ActiveHandle = ASC->ApplyGameplayEffectSpecToSelf(
							*SpecHandle.Data.Get());
					}
				}
				SavedLockonActorRotation=PlayerCharacter->GetActorRotation();
			}
			
		}
		UPlayerSpringArmComponent* SpringArm = GetAvatarActor()->FindComponentByClass<UPlayerSpringArmComponent>();
		SpringArm->bEnableCameraLag = true; // 카메라 랙 활성화
		SpringArm->CameraLagSpeed = 1.3f; // 카메라 랙 속도
		SpringArm->CameraLagMaxDistance = 50.0f;

		APlayerController* PlayerController = Cast<
		APlayerController>(Cast<ACharacter>(GetAvatarActor())->GetController());
		PlayerController->SetIgnoreLookInput(true); // 회전 입력 비활성화
	}
	else 
	{
		ReleaseLockOnTarget();
		NearestEnemy=nullptr;
		// 카메라 랙 활성화
	}
}


AActor* UAT_TargetingEnemy::FindNearestTarget() const
{
	FVector PlayerLocation = GetAvatarActor()->GetActorLocation();
	AActor* nearestEnemy = nullptr;
	float NearestDistance = TargetingRange;

	// 구체 반경 정의
	float SphereRadius = TargetingRange;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetAvatarActor()); // 플레이어 캐릭터는 무시
	QueryParams.bTraceComplex = false;
	QueryParams.bReturnPhysicalMaterial = false;

	// 충돌 형상(구체) 정의
	FCollisionShape CollisionShape = FCollisionShape::MakeSphere(SphereRadius);

	// 겹치는 액터들을 저장할 배열
	TArray<FOverlapResult> OverlapResults;

	UWorld* World = GetWorld();
	if (World)
	{
		// 구체 겹치기 수행
		bool bOverlapped = World->OverlapMultiByChannel(
			OverlapResults,
			PlayerLocation,
			FQuat::Identity,
			ECC_Pawn, // 적들이 Pawns이라고 가정; 필요에 따라 조정
			CollisionShape,
			QueryParams
		);

		// 겹친 액터가 있는 경우
		if (bOverlapped)
		{
			// 결과를 순회하면서 가장 가까운 적 찾기
			for (const FOverlapResult& Result : OverlapResults)
			{
				AEnemyCharacter* OverlappedActor = Cast<AEnemyCharacter>(Result.GetActor());
				if (OverlappedActor && OverlappedActor != GetAvatarActor())
				{
					// 1.화면에 표시되는지 체크**
					if (!IsActorOnScreen(OverlappedActor))
						continue;

					// 2. 시야 방해물이 없는지 체크**
					if (!HasLineOfSight(OverlappedActor))
						continue;
					float Distance = FVector::Dist(PlayerLocation, OverlappedActor->GetActorLocation());
					if (Distance < NearestDistance)
					{
						NearestDistance = Distance;
						nearestEnemy = OverlappedActor;
					}
				}
			}
		}
		// 디버그 스피어 그리기
		DrawDebugSphere(World, PlayerLocation, SphereRadius, 24, FColor::Green, false, 5.0f);

		if (nearestEnemy)
		{
			// 가장 가까운 적의 위치에 디버그 스피어 그리기
			DrawDebugSphere(World, nearestEnemy->GetActorLocation(), 50.0f, 24, FColor::Blue, false, 5.0f);
		}
	}


	return nearestEnemy;
}
void UAT_TargetingEnemy::PlayerContollerRotation(float DeltaTime)
{
	AActor* OwnerActor = GetAvatarActor();
	APlayerController* PlayerController = Cast<
		APlayerController>(Cast<ACharacter>(OwnerActor)->GetController());
	const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(
		OwnerActor->GetActorLocation(), NearestEnemy->GetActorLocation());
	const FRotator InterpRotation = UKismetMathLibrary::RInterpTo(PlayerController->GetControlRotation(),
																  LookAtRotation, DeltaTime, 10.f);

	FVector OwnerLocation = GetAvatarActor()->GetActorLocation();
	FVector TargetLocation = NearestEnemy->GetActorLocation();
	float Distance = FVector::Dist(OwnerLocation, TargetLocation);

	// 거리에 따라 Pitch 조정 (거리 조정 상수 설정 가능)
	float MaxDistance = 700.0f; // 최대 거리 (원하는 대로 설정 가능)
	float MinPitch = -18.5f; // 최소 Pitch 각도
	float MaxPitch = -2.5f; // 최대 Pitch 각도

	// 거리를 비율로 계산하여 Pitch 각도 보간
	float NormalizedDistance = FMath::Clamp(Distance / MaxDistance, 0.0f, 1.0f);
	float DynamicPitch = FMath::Lerp(MinPitch, MaxPitch, NormalizedDistance);
	//InterpRotation.Pitch+
	PlayerController->SetControlRotation(FRotator(InterpRotation.Pitch, InterpRotation.Yaw,
												  PlayerController->GetControlRotation().Roll));

}

bool UAT_TargetingEnemy::BacktoSquareOne(float DeltaTime)
{
	
	AActor* AvatarActor = GetAvatarActor();
	//RestoreCameraRotation(AvatarActor->FindComponentByClass<UPlayerCameraComponent>(),DeltaTime);
	RestoreSpringArmRotation(AvatarActor->FindComponentByClass<UPlayerSpringArmComponent>(),DeltaTime);
	


	return true;
}

void UAT_TargetingEnemy::CameraRotation(float DeltaTime)
{
	AActor* OwnerActor = GetAvatarActor();
	APlayerController* PlayerController = Cast<
		APlayerController>(Cast<ACharacter>(OwnerActor)->GetController());
	const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(
		OwnerActor->GetActorLocation(), NearestEnemy->GetActorLocation());
	const FRotator InterpRotation = UKismetMathLibrary::RInterpTo(PlayerController->GetControlRotation(),
																  LookAtRotation, DeltaTime, 10.f);
}

void UAT_TargetingEnemy::ReleaseLockOnTarget()
{
	AActor* OwnerActor = GetAvatarActor();
	//캐릭터 속도를 다시 높임
	APlayerController* PlayerController = Cast<APlayerController>(Cast<ACharacter>(OwnerActor)->GetController());
	if(PlayerController)
	{
		// 플레이어 컨트롤러의 회전 입력을 다시 활성화
		PlayerController->SetIgnoreLookInput(false);
		// 플레이어 컨트롤러의 뷰 타겟을 플레이어로 설정 (필요시)
		PlayerController->SetViewTarget(OwnerActor);
	}
	UPlayerSpringArmComponent* SpringArm = GetAvatarActor()->FindComponentByClass<UPlayerSpringArmComponent>();
	SpringArm->bEnableCameraLag = false;
	
	if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetAvatarActor()))
	{
		if (UAbilitySystemComponent* ASC = PlayerCharacter->GetAbilitySystemComponent())
		{
			ASC->RemoveActiveGameplayEffectBySourceEffect(TargetingSpeedEffect, ASC);
		}
	}
	
	OnCanceled.Broadcast();
}

void UAT_TargetingEnemy::LockOnTarget(AActor* NewTarget,float DeltaTime)
{
	CurrentTarget = NewTarget;
	if (CurrentTarget)
	{
		AActor* OwnerActor = GetAvatarActor();
		if (OwnerActor)
		{
			FVector TargetLocation = CurrentTarget->GetActorLocation();
			FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(
				OwnerActor->GetActorLocation(), TargetLocation);
			NewRotation.Pitch = 0.0f;
			UWorld* World = GetWorld();

			SavedLockonActorRotation = UKismetMathLibrary::RInterpTo(SavedLockonActorRotation,
															  NewRotation, DeltaTime, 10.f);
			OwnerActor->SetActorRotation(SavedLockonActorRotation);
			if (CurrentTarget && World)
			{
				// 가장 가까운 적의 위치에 디버그 스피어 그리기
				DrawDebugSphere(World, CurrentTarget->GetActorLocation(), 50.0f, 24, FColor::Blue, false, 0.1f);
				DrawDebugLine(World, GetAvatarActor()->GetActorLocation(), CurrentTarget->GetActorLocation(),
				              FColor::Yellow,
				              false, 0.01f, 0, 2.0f);
			}
		}
	}
}

void UAT_TargetingEnemy::SaveSpringArmSettings(USpringArmComponent* SpringArm)
{
	if (SpringArm)
	{
		
		SavedTargetArmLength = SpringArm->TargetArmLength;
		bSavedUsePawnControlRotation = SpringArm->bUsePawnControlRotation;
		bSavedInheritPitch = SpringArm->bInheritPitch;
		bSavedInheritYaw = SpringArm->bInheritYaw;
		bSavedInheritRoll = SpringArm->bInheritRoll;
		bSavedEnableCameraLag = SpringArm->bEnableCameraLag;
	}
}

void UAT_TargetingEnemy::SaveCameraSettings(UPlayerCameraComponent* CameraComponent)
{
	// 1. 타겟(캐릭터)의 위치를 구한다.
	FVector TargetLocation = GetAvatarActor()->GetActorLocation();
	FVector ForwardVector =  GetAvatarActor()->GetActorForwardVector();

	// 1) 카메라 위치 설정
	//    - 캐릭터를 바라보는 방향의 반대(-ForwardVector)로 400만큼 뒤로 이동
	//    - Z축(Up)으로 200만큼 올립니다.
	 SavedCameraWorldLocation = TargetLocation 
		- (ForwardVector * 400.0f) // 뒤로 400
		+ FVector(0.0f, 0.0f, 200.0f); // 위로 200
	SavedCameraWorldRotation = UKismetMathLibrary::FindLookAtRotation(
	  SavedCameraWorldLocation, 
	  TargetLocation
  );

	SavedTargetActorWorldLocation = GetAvatarActor()->GetActorLocation();
	SavedTargetActorWorldLocation += GetAvatarActor()->GetActorForwardVector() * 300.0f;
	SavedOwnerActorWorldLocation= GetAvatarActor()->GetActorLocation();
}



void UAT_TargetingEnemy::RestoreCameraRotation(UPlayerCameraComponent* CameraComponent, float DeltaTime)
{
	AActor* AvatarActor = GetAvatarActor();
	if (!AvatarActor || !CameraComponent)
	{
		return;
	}
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	// if (PC)
	// {
	// 	// 카메라가 보간되는 동안 마우스 Look 입력을 무시
	// 	PC->SetIgnoreLookInput(true);
	// }

	// 1. 캐릭터 위치 / Forward Vector 구하기
	FVector TargetLocation = AvatarActor->GetActorLocation();
	FVector ForwardVector = AvatarActor->GetActorForwardVector();

	// 2. 목표 카메라 위치(DesiredLocation) 계산
	//    캐릭터 바라보는 반대 방향(-ForwardVector)으로 400 뒤, 위로 200
	FVector DesiredLocation = TargetLocation
						   - (ForwardVector * 400.0f)
						   + FVector(0.0f, 0.0f, 200.0f);

	// 3. 카메라 현재 위치 / 목표 위치 보간(VInterpTo)
	FVector CurrentLocation = CameraComponent->GetComponentLocation();
	FVector InterpLocation  = FMath::VInterpTo(
		CurrentLocation,    // 시작
		SavedCameraWorldLocation,    // 목표
		DeltaTime,          // 델타타임
		5.0f                // 보간 속도(값 조절 가능)
	);
	CameraComponent->SetWorldLocation(InterpLocation);

	// 4. 목표 회전(DesiredRotation) 계산(캐릭터를 바라보는 방향)
	FRotator CurrentRotation = CameraComponent->GetComponentRotation();
	FRotator DesiredRotation = UKismetMathLibrary::FindLookAtRotation(
		CurrentLocation, // 카메라(보간된) 위치
		TargetLocation  // 캐릭터 위치
	);

	// 5. 회전도 보간(RInterpTo)
	FRotator InterpRotation = FMath::RInterpTo(
		CurrentRotation,  // 시작 회전
		DesiredRotation,  // 목표 회전
		DeltaTime,
		5.0f              // 보간 속도(값 조절 가능)
	);
	CameraComponent->SetWorldRotation(DesiredRotation);

	
	// 1) FRotator::Equals 사용
	//    두 회전이 (Yaw, Pitch, Roll) 각각 오차 1e-3(0.001) 이내면 같은 회전으로 봄
	if ( InterpLocation.Equals(SavedCameraWorldLocation, 0.8f))
	{
		UE_LOG(LogTemp, Log, TEXT("InterpLocation 성공적"));
		ReleaseLockOnTarget();
	}
}

void UAT_TargetingEnemy::RestoreSpringArmRotation(USpringArmComponent* SpringArm,float DeltaTime)
{
	AActor* OwnerActor = GetAvatarActor();
	APlayerController* PlayerController = Cast<
		APlayerController>(Cast<ACharacter>(OwnerActor)->GetController());
	const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(
		SavedOwnerActorWorldLocation, SavedTargetActorWorldLocation);
	const FRotator InterpRotation = UKismetMathLibrary::RInterpTo(PlayerController->GetControlRotation(),
																  LookAtRotation, DeltaTime, 10.f);


	PlayerController->SetControlRotation(FRotator(InterpRotation.Pitch, InterpRotation.Yaw,
												  PlayerController->GetControlRotation().Roll));
	if ( InterpRotation.Equals(LookAtRotation, 0.8f))
	{
		UE_LOG(LogTemp, Log, TEXT("InterpLocation 성공적"));
		ReleaseLockOnTarget();
		//OnCanceled.Broadcast();
	}
	
}
bool UAT_TargetingEnemy::IsActorOnScreen(AActor* Actor) const
{
	if (Actor == nullptr) 
		return false;

	// 플레이어 컨트롤러 가져오기
	AActor* OwnerActor = GetAvatarActor();
	APlayerController* PlayerController = Cast<
		APlayerController>(Cast<ACharacter>(OwnerActor)->GetController());
	if (PlayerController == nullptr) 
		return false;

	// 월드 위치 -> 스크린 좌표 변환
	FVector2D ScreenPosition;
	bool bProjected = PlayerController->ProjectWorldLocationToScreen(
		Actor->GetActorLocation(),
		ScreenPosition
	);

	// 변환 실패 시 화면에 안 보인다고 판단
	if (!bProjected)
		return false;

	// 현재 뷰포트 사이즈 얻기
	int32 ScreenX, ScreenY;
	PlayerController->GetViewportSize(ScreenX, ScreenY);

	// 스크린 좌표가 0~ScreenX, 0~ScreenY 안쪽이면 보인다고 간주
	return (ScreenPosition.X >= 0 && ScreenPosition.X <= ScreenX &&
			ScreenPosition.Y >= 0 && ScreenPosition.Y <= ScreenY);
}
bool UAT_TargetingEnemy::HasLineOfSight(AActor* Actor) const
{
	if (!Actor || !GetWorld())
		return false;

	// 플레이어 컨트롤러(카메라) 뷰포인트 가져오기
	// 플레이어 컨트롤러 가져오기
	AActor* OwnerActor = GetAvatarActor();
	APlayerController* PlayerController = Cast<
		APlayerController>(Cast<ACharacter>(OwnerActor)->GetController());
	if (!PlayerController)
		return false;

	FVector CamLoc;
	FRotator CamRot;
	PlayerController->GetPlayerViewPoint(CamLoc, CamRot);

	// 라인 트레이스 세팅
	FHitResult Hit;
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(LineOfSight), /*bTraceComplex=*/ true);
	// 자기 자신(플레이어)이나 Actor는 충돌 무시(필요에 맞게 추가)
	TraceParams.AddIgnoredActor(GetAvatarActor());
	TraceParams.AddIgnoredActor(Actor);

	// 카메라 -> 대상 액터까지 라인 트레이스
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		Hit,
		CamLoc,
		Actor->GetActorLocation(),
		ECC_Visibility,
		TraceParams
	);

	if (bHit)
	{
		// 맞은 액터가 타겟 액터 자신이 아니라면, 중간에 무언가 막고 있다는 의미
		if (Hit.GetActor() != Actor)
		{
			return false;
		}
	}

	return true;
}
void UAT_TargetingEnemy::CheckNearestEnemyDistance(float MaxDistance)
{
	// NearestEnemy가 유효한지 먼저 확인
	if (!NearestEnemy)
	{
		return;
	}

	// Avatar(플레이어) 액터가 유효한지 확인
	AActor* AvatarActor = GetAvatarActor();
	if (!AvatarActor)
	{
		return;
	}

	// 플레이어와 적 간의 거리 계산
	float Distance = FVector::Distance(NearestEnemy->GetActorLocation(), AvatarActor->GetActorLocation());

	// 지정된 MaxDistance 이상 멀어졌다면 NearestEnemy를 nullptr로
	if (Distance > MaxDistance)
	{
		ReleaseLockOnTarget();
		NearestEnemy=nullptr;
	}
}