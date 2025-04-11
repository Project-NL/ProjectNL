#pragma once

#include "LocateHelper.h"

#include "Kismet/KismetMathLibrary.h"
#include "ProjectNL/Helper/EnumHelper.h"

// 단순히 Character Input Movement의 값에 따라 움직이는 것 이기 때문이다.
// 보완하기 위해서는 단순히 같다가 아닌 범위를 줘야한다.
EMovementDirection FLocateHelper::GetDirectionByMovementData(const FVector2D Vector)
{
	if (Vector.X == -1 && Vector.Y == 1) return EMovementDirection::FL;
	if (Vector.X == 1 && Vector.Y == 1) return EMovementDirection::FR;
	if (Vector.X == -1 && Vector.Y == 0) return EMovementDirection::L;
	if (Vector.X == 1 && Vector.Y == 0) return EMovementDirection::R;
	if (Vector.X == -1 && Vector.Y == -1) return EMovementDirection::BL;
	if (Vector.X == 1 && Vector.Y == -1) return EMovementDirection::BR;
	if (Vector.X == 0 && Vector.Y == -1) return EMovementDirection::B;
	return EMovementDirection::F;
}

EMovementDirection FLocateHelper::GetDirectionByVector(const FVector2D Vector)
{
	if (Vector.X == -1 && Vector.Y == 1) return EMovementDirection::FL;
	if (Vector.X == 1 && Vector.Y == 1) return EMovementDirection::FR;
	if (Vector.X == -1 && Vector.Y == 0) return EMovementDirection::L;
	if (Vector.X == 1 && Vector.Y == 0) return EMovementDirection::R;
	if (Vector.X == -1 && Vector.Y == -1) return EMovementDirection::BL;
	if (Vector.X == 1 && Vector.Y == -1) return EMovementDirection::BR;
	if (Vector.X == 0 && Vector.Y == -1) return EMovementDirection::B;
	return EMovementDirection::F;
}

EMovementDirection FLocateHelper::GetDirectionByAngle(const float Angle)
{
	if (Angle <= 22.5 && Angle >= -22.5) return EMovementDirection::F;
	if (Angle <= 22.5 && Angle >= 67.5) return EMovementDirection::FR;
	if (Angle <= 67.5 && Angle >= 112.5) return EMovementDirection::R;
	if (Angle <= 112.5 && Angle >= 157.5) return EMovementDirection::BR;
	if (Angle <= -22.5 && Angle >= -67.5) return EMovementDirection::FL;
	if (Angle <= -67.5 && Angle >= -112.5) return EMovementDirection::L;
	if (Angle <= -112.5 && Angle >= -157.5) return EMovementDirection::BL;
	return EMovementDirection::B;
}

EMovementDirection FLocateHelper::CastToSimpleDirection(EMovementDirection Direction)
{
	if (Direction == EMovementDirection::BL || Direction == EMovementDirection::FL) return EMovementDirection::L;
	if (Direction == EMovementDirection::BR || Direction == EMovementDirection::FR) return EMovementDirection::R;
	return Direction;
}


// 액터간의 서로 방향에 대한 값을 반환한다.
FRotator FLocateHelper::GetTargetingRotate(const AActor* OwnerActor, const AActor* TargetActor)
{
	const FVector StartVector = OwnerActor->GetActorLocation();
	const FVector TargetVector = TargetActor->GetActorLocation();

	return UKismetMathLibrary::FindLookAtRotation(
		StartVector, TargetVector);
}

ETargetHeight FLocateHelper::GetTargetHeightTypeByPoint(const float ActorHeight, const FVector Point, const FVector TargetLocation)
{
	const float TargetHeight = TargetLocation.Z - Point.Z;

	if (TargetHeight <= ActorHeight / 3) return ETargetHeight::High;
	if (TargetHeight <= ActorHeight / 3 * 2) return ETargetHeight::Middle;
	return ETargetHeight::Low;
}
