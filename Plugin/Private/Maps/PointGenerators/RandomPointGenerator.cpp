#include "PolygonalMapGeneratorPrivatePCH.h"
#include "Maps/PointGenerators/RandomPointGenerator.h"

TArray<FVector2D> URandomPointGenerator::GeneratePoints_Implementation(int32 numberOfPoints)
{
	TArray<FVector2D> pointArray;
	for (int32 i = 0; i < numberOfPoints; i++)
	{
		float randomX = RandomGenerator.FRandRange(Border, MapSize - Border);
		float randomY = RandomGenerator.FRandRange(Border, MapSize - Border);
		pointArray.Add(FVector2D(randomX, randomY));
	}
	return pointArray;
}