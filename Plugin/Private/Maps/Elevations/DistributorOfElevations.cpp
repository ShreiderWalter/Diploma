#include "PolygonalMapGeneratorPrivatePCH.h"
#include "Maps/IslandShapes/IslandShape.h"
#include "MapDataHelper.h"
#include "Maps/Elevations/ElevationDistributor.h"


void DistributorOfElevations::SetGraph(UPolygonMap* graph)
{
	MapGraph = graph;
}

void DistributorOfElevations::AssignCornerElevations(UIslandShape* islandShape, bool bneedsMoreRandomness, FRandomStream& randomGenerator)
{
	TQueue<int32> cornerQueue;

	for (int i = 0; i < MapGraph->GetCornerNum(); i++)
	{
		FMapCorner corner = MapGraph->GetCorner(i);
		if (islandShape->IsPointLand(corner.CornerData.Point))
		{
			corner.CornerData = UMapDataHelper::RemoveOcean(corner.CornerData);
		}
		else
		{
			corner.CornerData = UMapDataHelper::SetOcean(corner.CornerData);
		}

		if (UMapDataHelper::IsBorder(corner.CornerData) || UMapDataHelper::IsWater(corner.CornerData))
		{
			corner.CornerData.Elevation = 0.0f;
			cornerQueue.Enqueue(i);
		}
		else
		{
			corner.CornerData.Elevation = 100.0f;
		}
		MapGraph->UpdateCorner(corner);
	}

	float queueOffsetAmount = 0.0f;
	while (!cornerQueue.IsEmpty())
	{
		int32 cornerIndex;
		cornerQueue.Dequeue(cornerIndex);
		FMapCorner corner = MapGraph->GetCorner(cornerIndex);

		for (int i = 0; i < corner.Adjacent.Num(); i++)
		{
			FMapCorner adjacent = MapGraph->GetCorner(corner.Adjacent[i]);
			float newElevation = 0.1f + corner.CornerData.Elevation;
			if (!UMapDataHelper::IsWater(corner.CornerData) && !UMapDataHelper::IsWater(adjacent.CornerData))
			{
				newElevation += 1.0f;

				if (bneedsMoreRandomness)
				{
					newElevation += randomGenerator.GetFraction();
				}
			}

			if (newElevation < adjacent.CornerData.Elevation)
			{
				adjacent.CornerData.Elevation = newElevation;
				cornerQueue.Enqueue(adjacent.Index);
			}

			MapGraph->UpdateCorner(adjacent);
		}

		queueOffsetAmount += 0.05f;
	}
}

void DistributorOfElevations::CalculateDownslopes()
{
	for (int i = 0; i < MapGraph->GetCornerNum(); i++)
	{
		FMapCorner corner = MapGraph->GetCorner(i);
		int32 downslopeIndex = corner.Index;
		for (int j = 0; j < corner.Adjacent.Num(); j++)
		{
			FMapCorner adjacent = MapGraph->GetCorner(corner.Adjacent[j]);
			if (adjacent.CornerData.Elevation <= MapGraph->GetCorner(downslopeIndex).CornerData.Elevation)
			{
				downslopeIndex = adjacent.Index;
			}
		}
		corner.Downslope = downslopeIndex;
		MapGraph->UpdateCorner(corner);
	}
}

void DistributorOfElevations::RedistributeElevations(TArray<int32> landCorners)
{
	TArray<FMapCorner> mapCorners;

	for (int i = 0; i < landCorners.Num(); i++)
	{
		mapCorners.Add(MapGraph->GetCorner(landCorners[i]));
	}

	mapCorners.HeapSort();

	float maxElevation = -1.0f;
	for (int i = 0; i < mapCorners.Num(); i++)
	{
		float y = i / (mapCorners.Num() - 1.0f);
		float x = FMath::Sqrt(1.1f) - FMath::Sqrt(1.1f * (1.0f - y));
		if (x > maxElevation)
		{
			maxElevation = x;
		}
		FMapCorner corner = MapGraph->GetCorner(mapCorners[i].Index);
		corner.CornerData.Elevation = x;
		MapGraph->UpdateCorner(corner);
	}

	for (int i = 0; i < mapCorners.Num(); i++)
	{
		FMapCorner corner = MapGraph->GetCorner(mapCorners[i].Index);
		corner.CornerData.Elevation /= maxElevation;
		MapGraph->UpdateCorner(corner);
	}
}

void DistributorOfElevations::FlattenWaterElevations()
{
	TArray<bool> processedCorners;
	processedCorners.SetNumZeroed(MapGraph->GetCornerNum());
	for (int i = 0; i < MapGraph->GetCornerNum(); i++)
	{
		if (processedCorners[i])
		{
			continue;
		}

		FMapCorner corner = MapGraph->GetCorner(i);
		if (UMapDataHelper::IsOcean(corner.CornerData))
		{
			corner.CornerData.Elevation = 0.0f;
			MapGraph->UpdateCorner(corner);
			processedCorners[i] = true;
		}
	}
}

void DistributorOfElevations::AssignPolygonElevations()
{
	for (int i = 0; i < MapGraph->GetCenterNum(); i++)
	{
		float sumElevation = 0.0f;
		FMapCenter center = MapGraph->GetCenter(i);
		for (int32 j = 0; j < center.Corners.Num(); j++)
		{
			FMapCorner corner = MapGraph->GetCorner(center.Corners[j]);
			sumElevation += corner.CornerData.Elevation;
		}
		float elevation = sumElevation / center.Corners.Num();
		center.CenterData.Elevation = elevation;
		MapGraph->UpdateCenter(center);
	}
}
