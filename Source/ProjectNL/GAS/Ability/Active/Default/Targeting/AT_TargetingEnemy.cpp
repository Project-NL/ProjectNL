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
UAT_TargetingEnemy* UAT_TargetingEnemy::InitialEvent(UGameplayAbility* OwningAbility)
{
	UAT_TargetingEnemy* NewEvent = NewAbilityTask<UAT_TargetingEnemy>(
		OwningAbility);

	return NewEvent;
}

void UAT_TargetingEnemy::Activate()
{
	Super::Activate();
	OriginalCharacterSpeed = Cast<ACharacter>(GetAvatarActor())->GetCharacterMovement()->MaxWalkSpeed;
	SaveSpringArmSettings(GetAvatarActor()->FindComponentByClass<UPlayerSpringArmComponent>());
	bTickingTask = true;
	TargetNearestEnemy();
}


void UAT_TargetingEnemy::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);
//	UE_LOG(LogTemp, Warning, TEXT("UAT_TargetingEnemy Tasking"));
	if (NearestEnemy && NearestEnemyCheck)
	{
		// FGameplayTagContainer ActiveTags;
		// //활성화된 컨테이너태그를 ActiveTAgs에 저장
		// Cast<APlayerCharacter>(GetAvatarActor())->GetAbilitySystemComponent()->GetOwnedGameplayTags(ActiveTags);

		LockOnTarget(NearestEnemy);

		CameraRotation(DeltaTime);
	}
}

void UAT_TargetingEnemy::TargetNearestEnemy()
{
	NearestEnemyCheck = !NearestEnemyCheck;
	NearestEnemy = FindNearestTarget();
	if (!NearestEnemyCheck)
	{
		ReleaseLockOnTarget(); //어빌리티를 종료시킵니다.
	}
	else if (NearestEnemyCheck && NearestEnemy) //클릭과 타겟 캐릭터 상대방이 둘다 있을경우
	{
		APlayerController* PlayerController = Cast<
			APlayerController>(Cast<ACharacter>(GetAvatarActor())->GetController());
		PlayerController->SetIgnoreLookInput(true); // 회전 입력 비활성화
		Cast<ACharacter>(GetAvatarActor())->GetCharacterMovement()->MaxWalkSpeed = 250.0f; //속도 감소
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

void UAT_TargetingEnemy::CameraRotation(float DeltaTime)
{
	AActor* OwnerActor = GetAvatarActor();
	APlayerController* PlayerController = Cast<
		APlayerController>(Cast<ACharacter>(OwnerActor)->GetController());
	const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(
		OwnerActor->GetActorLocation(), NearestEnemy->GetActorLocation());
	const FRotator InterpRotation = UKismetMathLibrary::RInterpTo(PlayerController->GetControlRotation(),
	                                                              LookAtRotation, DeltaTime, 10.f);


	PlayerController->SetControlRotation(FRotator(InterpRotation.Pitch, InterpRotation.Yaw,
	                                              PlayerController->GetControlRotation().Roll));

	UPlayerSpringArmComponent* SpringArm = OwnerActor->FindComponentByClass<UPlayerSpringArmComponent>();
	if (SpringArm && CurrentTarget)
	{
		// 타겟과의 거리 계산
		FVector CameraLocation = SpringArm->GetComponentLocation();
		FVector TargetLocation = CurrentTarget->GetActorLocation();
		float Distance = FVector::Dist(CameraLocation, TargetLocation);

		// 거리에 따라 Pitch 조정 (거리 조정 상수 설정 가능)
		float MaxDistance = 700.0f; // 최대 거리 (원하는 대로 설정 가능)
		float MinPitch = -18.5f; // 최소 Pitch 각도
		float MaxPitch = -2.5f; // 최대 Pitch 각도

		// 거리를 비율로 계산하여 Pitch 각도 보간
		float NormalizedDistance = FMath::Clamp(Distance / MaxDistance, 0.0f, 1.0f);
		float DynamicPitch = FMath::Lerp(MinPitch, MaxPitch, NormalizedDistance);

		// 계산된 DynamicPitch로 회전 설정
		FRotator TargetRotation(DynamicPitch, InterpRotation.Yaw, 0.0f);
		SpringArm->SetWorldRotation(TargetRotation);

		SpringArm->TargetArmLength = 400.0f; // 카메라와 캐릭터 간 거리
		SpringArm->bUsePawnControlRotation = false; // 플레이어 컨트롤러의 회전 사용 안함
		//SpringArm->bInheritPitch = false;
		//SpringArm->bInheritYaw = true;
		//SpringArm->bInheritRoll = false;
		SpringArm->bEnableCameraLag = true; // 카메라 랙 활성화
		SpringArm->CameraLagSpeed = 2.0f; // 카메라 랙 속도
		SpringArm->CameraLagMaxDistance = 500.0f;
	}
	UPlayerCameraComponent* Camera = OwnerActor->FindComponentByClass<UPlayerCameraComponent>();
	if (Camera && CurrentTarget)
	{
		FRotator CurrentCameraRotation = Camera->GetComponentRotation();

		// 타겟의 위치를 바라보는 회전 계산
		FVector TargetLocation = CurrentTarget->GetActorLocation();
		FVector CameraLocation = Camera->GetComponentLocation();
		FRotator CameraLookAtRotation = UKismetMathLibrary::FindLookAtRotation(CameraLocation, TargetLocation);

		// 보간하여 카메라 회전 설정
		FRotator Rotation = FMath::RInterpTo(CurrentCameraRotation, CameraLookAtRotation, DeltaTime, 20.0f);

		Camera->SetWorldRotation(Rotation);
	}
}

void UAT_TargetingEnemy::ReleaseLockOnTarget()
{
	NearestEnemyCheck = false;
	AActor* OwnerActor = GetAvatarActor();
	//캐릭터 속도를 다시 높임
	float BaseSpeed=Cast<UBaseAttributeSet>(Cast<APlayerCharacter>(GetAvatarActor())->GetAbilitySystemComponent()->GetAttributeSet(UBaseAttributeSet::StaticClass()))->GetMovementSpeed();
	Cast<ACharacter>(GetAvatarActor())->GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
	APlayerController* PlayerController = Cast<APlayerController>(Cast<ACharacter>(OwnerActor)->GetController());
	if(PlayerController)
	{
		// 플레이어 컨트롤러의 회전 입력을 다시 활성화
		PlayerController->SetIgnoreLookInput(false);
		// 플레이어 컨트롤러의 뷰 타겟을 플레이어로 설정 (필요시)
		PlayerController->SetViewTarget(OwnerActor);
	}
	if(UPlayerSpringArmComponent* playerspringarm=GetAvatarActor()->FindComponentByClass<UPlayerSpringArmComponent>())
	{
		RestoreSpringArmSettings(playerspringarm);
	}
	OnCanceled.Broadcast();
}

void UAT_TargetingEnemy::LockOnTarget(AActor* NewTarget)
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
			OwnerActor->SetActorRotation(NewRotation);
			UWorld* World = GetWorld();
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
		SavedRotation = SpringArm->GetRelativeRotation();
		SavedLocation = SpringArm->GetComponentLocation();
		SavedTargetArmLength = SpringArm->TargetArmLength;
		bSavedUsePawnControlRotation = SpringArm->bUsePawnControlRotation;
		bSavedInheritPitch = SpringArm->bInheritPitch;
		bSavedInheritYaw = SpringArm->bInheritYaw;
		bSavedInheritRoll = SpringArm->bInheritRoll;
		bSavedEnableCameraLag = SpringArm->bEnableCameraLag;
	}
}

void UAT_TargetingEnemy::RestoreSpringArmSettings(USpringArmComponent* SpringArm)
{
	if (SpringArm)
	{
		SpringArm->TargetArmLength = SavedTargetArmLength;
		SpringArm->bUsePawnControlRotation = bSavedUsePawnControlRotation;
		SpringArm->bInheritPitch = bSavedInheritPitch;
		SpringArm->bInheritYaw = bSavedInheritYaw;
		SpringArm->bInheritRoll = bSavedInheritRoll;
		SpringArm->bEnableCameraLag = bSavedEnableCameraLag;
	}
}
