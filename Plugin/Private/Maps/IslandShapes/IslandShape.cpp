#include "PolygonalMapGeneratorPrivatePCH.h"
#include "Maps/IslandShapes/IslandShape.h"

void UIslandShape::SetSeed_Implementation(int32 seed, int32 size)
{
	StreamRNG.Initialize(seed);
	Size = size;
}

bool UIslandShape::IsPointLand_Implementation(FVector2D point)
{
	return false;
}