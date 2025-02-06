// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

#include "Animation/AnimMontage.h" 
#include "Curves/CurveVector.h"
#include "ProjectNL/Helper/EnumHelper.h"
#include <Kismet/KismetMathLibrary.h>
#include "ActionAbilityData.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FCollisionData
{
	GENERATED_BODY()
public:
	/** 
	* 충돌 영역을 붙일 본(또는 소켓) 이름
	* SkeletalMesh에서 이 BoneName을 찾아 기준점으로 사용합니다.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	FName StartBoneName = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
    FName EndBoneName = NAME_None;
	/**
	 * 충돌 영역의 형태: Sphere, Box, Capsule 등
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	ECollisionShapeTypes ShapeType = ECollisionShapeTypes::Sphere;

	/**
	 * Bone을 기준으로 한 상대 위치
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	FVector RelativeLocation = FVector::ZeroVector;

	/**
	 * Bone을 기준으로 한 상대 회전값
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	FRotator RelativeRotation = FRotator::ZeroRotator;

	/**
	 * 충돌 영역의 크기(Extent, Radius 등)
	 * - Sphere 의 경우: X만 사용한다고 가정 (X = 반지름)
	 * - Box 의 경우: X, Y, Z를 모두 사용 (박스의 반절 길이)
	 * - Capsule 의 경우: X = 반지름, Z = 캡슐의 하프 높이(반), Y는 무시
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	FVector ShapeExtent = FVector(50.f, 50.f, 50.f);

	/**
	 * 추가적으로 디버그 용도로 충돌 영역을 시각화할 때 색상을 지정하고 싶다면
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	FColor DebugColor = FColor::Red;

public:
	/**
	 * 런타임(또는 C++ 로직)에서 이 데이터를 바탕으로 
	 * 실제 엔진의 FCollisionShape를 생성해 반환하는 편의 함수
	 */
	FCollisionShape ToCollisionShape() const
	{
		switch (ShapeType)
		{
		case ECollisionShapeTypes::Sphere:
			return FCollisionShape::MakeSphere(ShapeExtent.X);
		case ECollisionShapeTypes::Box:
			return FCollisionShape::MakeBox(ShapeExtent);
		case ECollisionShapeTypes::Capsule:
			return FCollisionShape::MakeCapsule(ShapeExtent.X, ShapeExtent.Z);
		default:
			// 기본값: Sphere
				return FCollisionShape::MakeSphere(ShapeExtent.X);
		}
	}

	void DebugDrawCollision(const UWorld* World, const USkeletalMeshComponent* MeshComponent, float Duration = 0.f, float Thickness = 1.f) const
    {
        if (!World) return;


        FTransform StartBoneTransform = MeshComponent->GetSocketTransform(StartBoneName, ERelativeTransformSpace::RTS_World);
        FTransform EndBoneTransform = MeshComponent->GetSocketTransform(EndBoneName, ERelativeTransformSpace::RTS_World);
        // Relative 로컬 변환(Offset)과 본의 월드 변환을 합쳐 최종 위치/회전 계산
         FVector StartLocation = StartBoneTransform.GetLocation();
          FVector EndLocation = EndBoneTransform.GetLocation();

          // 캡슐의 방향 계산
          FVector Direction = (EndLocation - StartLocation).GetSafeNormal(); // 방향 벡터
          FVector UpVector = FVector::UpVector;                              // 캡슐 기본 Z축
          FQuat CapsuleQuat = FQuat::FindBetweenNormals(UpVector, Direction); // Z축을 정렬

    
        // 캡슐 중심 위치
        FVector Center = (StartLocation + EndLocation) * 0.5f;
    
        // 캡슐의 길이와 반지름 설정
        float CapsuleLength = FVector::Distance(StartLocation, EndLocation) * 0.5f; // 하프-길이

        switch (ShapeType)
        {
        case ECollisionShapeTypes::Sphere:
        {
            float Radius = ShapeExtent.X;  // Sphere의 경우 X를 반지름으로 사용
            DrawDebugSphere(
                World,
                Center,
                Radius,
                16,                // 세그먼트 수
                DebugColor,
                false,             // 지속 여부 (true면 Duration 무시하고 영구 표시)
                Duration,
                0,                 // DepthPriority
                Thickness
            );
            break;
        }
        case ECollisionShapeTypes::Box:
        {
            // Box의 경우 ShapeExtent가 "박스의 반Extent"라 가정
            FVector HalfExtents = ShapeExtent;
            DrawDebugBox(
                World,
                Center,
                HalfExtents,
                CapsuleQuat,
                DebugColor,
                false,
                Duration,
                0,
                Thickness
            );
            break;
        }
        case ECollisionShapeTypes::Capsule:
        {
            float Radius = ShapeExtent.X;      // Capsule 반지름
            float HalfHeight = ShapeExtent.Z;  // Capsule 하프 높이
            DrawDebugCapsule(
                World,
                Center,
                HalfHeight,
                Radius,
                CapsuleQuat,
                DebugColor,
                false,
                Duration,
                0,
                Thickness
            );
            break;
        }
        default:
        {
            // 기본: Sphere
            float Radius = ShapeExtent.X;
            DrawDebugSphere(
                World,
                Center,
                Radius,
                16,
                DebugColor,
                false,
                Duration,
                0,
                Thickness
            );
            break;
        }
        }
    }
};
USTRUCT(BlueprintType)
struct FActionSequenceData
{
	GENERATED_BODY()


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> ArrActionAnim; // 어떤 액션을 진행할 것인지

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FCollisionData CollisionData;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EMovementDirection ArrMovementDirection; // 이동 방향

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UCurveVector> CurveVector;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DelayTime; // 딜레이 시간

};

USTRUCT(BlueprintType)
struct FActionAbilityData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FActionSequenceData> ArrActionSequenceDatas;
};