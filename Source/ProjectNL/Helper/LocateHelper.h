#pragma once

#include "CoreMinimal.h"

enum class EMovementDirection : uint8;
enum class ETargetHeight: uint8;

class PROJECTNL_API FLocateHelper
{
public:
	static EMovementDirection GetDirectionByMovementData(const FVector2D Vector);
	
	static EMovementDirection GetDirectionByVector(const FVector2D Vector);
	
	static EMovementDirection GetDirectionByAngle(const float Angle);

	static EMovementDirection CastToSimpleDirection(EMovementDirection Direction);

	static FRotator GetTargetingRotate(const AActor* OwnerActor, const AActor* TargetActor);

	static ETargetHeight GetTargetHeightTypeByPoint(const float ActorHeight, const FVector Point, const FVector TargetLocation);
};
