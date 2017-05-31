#include "PolygonalMapGeneratorPrivatePCH.h"
#include "Maps/IslandShapes/RadialIsland.h"

void URadialIsland::SetSeed_Implementation(int32 seed, int32 size)
{
	StreamRNG.Initialize(seed);
	Size = size;
	Bumps = StreamRNG.RandRange(BumpsMin, BumpsMax);
	StartAngle = StreamRNG.FRandRange(StartAngleMin, StartAngleMax) * PI;
	AngleOffset = StreamRNG.FRandRange(AngleOffsetMin, AngleOffsetMax) * PI;
	MinAngle = StreamRNG.FRandRange(MinAngleMin, MinAngleMax);
}

bool URadialIsland::IsPointLand_Implementation(FVector2D point)
{
	point.X = 2.0f * ((point.X / Size) - 0.5f);
	point.Y = 2.0f * ((point.Y / Size) - 0.5f);
	float angle = FMath::Atan2(point.Y, point.X);
	float length = 0.5f * (FMath::Max(FMath::Abs(point.X), FMath::Abs(point.Y)) + FVector2D::Distance(FVector2D::ZeroVector, point));
	float innerRadius;
	float outerRadius;

	if ((FMath::Abs(angle - AngleOffset) < MinAngle
		|| FMath::Abs(angle - AngleOffset + 2.0f * PI) < MinAngle
		|| FMath::Abs(angle - AngleOffset - 2.0f * PI) < MinAngle))
	{
		innerRadius = 0.2f;
		outerRadius = 0.2f;
	}
	else
	{
		innerRadius = 0.5f + 0.4f * FMath::Sin(StartAngle + Bumps * angle + FMath::Cos((Bumps + 3) * angle));
		outerRadius = 0.7f - 0.2f * FMath::Sin(StartAngle + Bumps * angle - FMath::Sin((Bumps + 2) * angle));
	}

	return (length < innerRadius) || (length > innerRadius * IslandFactor && length < outerRadius);
}
