

#include "SmokeVolume.h"

#include "Kismet/KismetSystemLibrary.h"

ASmokeVolume::ASmokeVolume()
{
}

void ASmokeVolume::OnConstruction()
{
	Super::OnConstruction(this->GetActorTransform());
}

void ASmokeVolume::FloodFill()
{
	TArray<FVector> PositionQueue;
	TArray<FVector> CheckedPositions;
	TArray<FVector> AcceptedPositions;
	FVector CurrentQueryPosition;
	FVector CurrentNeighborPosition;
	int TotalEnergy = GridResolution * GridResolution * GridResolution;
	int TotalCollisions = 0;
	float CellSize = TargetVolumeDiameter / GridResolution;
	FHitResult TraceHit;

	// Declare per-index axis values for use in each position nested loop
	TArray<FVector> CheckAxes;
	CheckAxes.Add(FVector(1.0, 0.0, 0.0));
	CheckAxes.Add(FVector(-1.0, 0.0, 0.0));
	CheckAxes.Add(FVector(0.0, 1.0, 0.0));
	CheckAxes.Add(FVector(0.0, -1.0, 0.0));
	CheckAxes.Add(FVector(0.0, 0.0, 1.0));
	CheckAxes.Add(FVector(0.0, 0.0, -1.0));

	PositionQueue.Add(this->GetActorLocation());
	TotalEnergy--;

	while (PositionQueue.Num() > 0 && TotalEnergy > 0)
	{
		CurrentQueryPosition = PositionQueue[0];
		if (SnapToGrid)
		{
			CurrentQueryPosition = CurrentQueryPosition.GridSnap(CellSize);
		}

		for (int i = 0; i <6 ; i++)
		{
			CurrentNeighborPosition = CurrentQueryPosition + (CheckAxes[i] * FVector(CellSize));
			if (!CheckedPositions.Contains(CurrentNeighborPosition))
			{
				static const TArray<AActor*> NullArray;
				UKismetSystemLibrary::LineTraceSingle(this, CurrentQueryPosition, CurrentNeighborPosition, TraceTypeQuery1, false, NullArray, EDrawDebugTrace::None, TraceHit, true);

				if (!TraceHit.bBlockingHit)
				{
					PositionQueue.Add(CurrentNeighborPosition);
					AcceptedPositions.Add(CurrentNeighborPosition);
					CheckedPositions.Add(CurrentNeighborPosition);
					TotalEnergy--;
				}
				else
				{
					TotalCollisions++;
				}
			
			}
		}
		PositionQueue.RemoveAt(0, 1, true);
	}

	if (SmoothGridPositions)
	{
		VoxelPositions = SmoothPointsToSphere(AcceptedPositions, TotalCollisions);
	}
	else
	{
		VoxelPositions = AcceptedPositions;
	}

	if (DebugDrawTime > 0.0)
	{	static const UWorld* World = GetWorld();
		static const FColor Color = FColor(0.0, 0.66 * 255, 0.66 * 255);
		FlushPersistentDebugLines(World);
		for (FVector VoxelPosition : VoxelPositions)
		{
			DrawDebugPoint(World, VoxelPosition, (CellSize * 0.33), Color, false, DebugDrawTime, 0);
		}
	}
}

TArray<FVector> ASmokeVolume::SmoothPointsToSphere(TArray<FVector> GridPoints, int CollisionCount)
{
	TArray<float> PointLengths;
	float LengthAverage = 0.0f;
	TArray<FVector> AveragedPoints;

	for(FVector GridPoint:GridPoints)
	{
		FVector TransformedPoint = this->GetTransform().InverseTransformPosition(GridPoint);
		float TempPoint = TransformedPoint.Length();
		PointLengths.Add(TempPoint);
	}

	for(float Length:PointLengths)
	{
		LengthAverage = Length + LengthAverage;
	}

	float PointCount = float(PointLengths.Num());
	float CollisionCountMultiplier = float(FMath::Clamp((CollisionCount - PointLengths.Num()), 0, (CollisionCount - PointLengths.Num())));
	LengthAverage = (LengthAverage / PointCount) + (LengthAverage * CollisionCountMultiplier);

	for (FVector GridPoint:GridPoints)
	{
		FVector TransformedPoint = this->GetTransform().InverseTransformPosition(GridPoint);
		FVector Direction = TransformedPoint.GetUnsafeNormal();
		float DistanceThreshold = (LengthAverage / TransformedPoint.Length() * TransformedPoint.Length());
		if (TransformedPoint.Length() <= DistanceThreshold)
		{
			FVector AveragedPoint = this->GetTransform().TransformPosition(Direction * TransformedPoint.Length());
			AveragedPoints.Add(AveragedPoint);
		}
	}
	return AveragedPoints;
}
