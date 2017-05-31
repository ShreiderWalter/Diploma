#include "PolygonalMapGeneratorPrivatePCH.h"
#include "MapDataHelper.h"
#include "MoistureDistributor.h"

void DistributorOfMoistures::SetGraph(UPolygonMap* Graph, int32 Size)
{
	MapGraph = Graph;
	MapSize = Size;
}

void DistributorOfMoistures::AssignOceanCoastAndLand()
{
	TQueue<int32> centerQueue;
	for (int32 i = 0; i < MapGraph->GetCenterNum(); i++)
	{
		uint16 numWater = 0;
		FMapCenter center = MapGraph->GetCenter(i);
		for (int32 j = 0; j < center.Corners.Num(); j++)
		{
			FMapCorner corner = MapGraph->GetCorner(center.Corners[j]);
			if (UMapDataHelper::IsBorder(corner.CornerData))
			{
				center.CenterData = UMapDataHelper::SetBorder(center.CenterData);
				center.CenterData = UMapDataHelper::SetOcean(center.CenterData);
				centerQueue.Enqueue(i);
			}
			if (UMapDataHelper::IsWater(corner.CornerData))
			{
				numWater++;
			}
			MapGraph->UpdateCorner(corner);
		}
		if (!UMapDataHelper::IsOcean(center.CenterData) && numWater > center.Corners.Num() * LakeThreshold)
		{
			center.CenterData = UMapDataHelper::SetFreshwater(center.CenterData);
		}
		else
		{
			center.CenterData = UMapDataHelper::RemoveFreshwater(center.CenterData);
		}
		MapGraph->UpdateCenter(center);
	}

	while (!centerQueue.IsEmpty())
	{
		int32 centerIndex;
		centerQueue.Dequeue(centerIndex);
		FMapCenter center = MapGraph->GetCenter(centerIndex);
		for (int32 i = 0; i < center.Neighbors.Num(); i++)
		{
			FMapCenter neighbor = MapGraph->GetCenter(center.Neighbors[i]);
			if (UMapDataHelper::IsWater(neighbor.CenterData) && !UMapDataHelper::IsOcean(neighbor.CenterData))
			{
				neighbor.CenterData = UMapDataHelper::SetOcean(neighbor.CenterData);
				centerQueue.Enqueue(neighbor.Index);
			}
			MapGraph->UpdateCenter(neighbor);
		}
	}

	for (int32 i = 0; i < MapGraph->GetCenterNum(); i++)
	{
		uint16 numOcean = 0;
		uint16 numLand = 0;
		FMapCenter center = MapGraph->GetCenter(i);
		for (int j = 0; j < center.Neighbors.Num(); j++)
		{
			FMapCenter neighbor = MapGraph->GetCenter(center.Neighbors[j]);
			if (UMapDataHelper::IsOcean(neighbor.CenterData))
			{
				numOcean++;
			}
			else if (!UMapDataHelper::IsWater(neighbor.CenterData))
			{
				numLand++;
			}
			if (numOcean > 0 && numLand > 0)
			{
				break;
			}
		}
		if (numOcean > 0 && numLand > 0)
		{
			center.CenterData = UMapDataHelper::SetCoast(center.CenterData);
		}
		else
		{
			center.CenterData = UMapDataHelper::RemoveCoast(center.CenterData);
		}
		MapGraph->UpdateCenter(center);
	}

	for (int32 i = 0; i < MapGraph->GetCornerNum(); i++)
	{
		uint16 numOcean = 0;
		uint16 numLand = 0;
		FMapCorner corner = MapGraph->GetCorner(i);
		for (int j = 0; j < corner.Touches.Num(); j++)
		{
			FMapCenter neighbor = MapGraph->GetCenter(corner.Touches[j]);
			if (UMapDataHelper::IsOcean(neighbor.CenterData))
			{
				numOcean++;
			}
			else if (!UMapDataHelper::IsWater(neighbor.CenterData))
			{
				numLand++;
			}
		}
		if (numOcean == corner.Touches.Num())
		{
			corner.CornerData = UMapDataHelper::SetOcean(corner.CornerData);
		}
		else
		{
			corner.CornerData = UMapDataHelper::RemoveOcean(corner.CornerData);
		}
		if (numOcean > 0 && numLand > 0)
		{
			corner.CornerData = UMapDataHelper::SetCoast(corner.CornerData);
		}
		else
		{
			corner.CornerData = UMapDataHelper::RemoveCoast(corner.CornerData);
		}
		if (UMapDataHelper::IsBorder(corner.CornerData))
		{
			corner.CornerData = UMapDataHelper::SetOcean(corner.CornerData);
		}
		else if (numLand != corner.Touches.Num() && !UMapDataHelper::IsCoast(corner.CornerData))
		{
			corner.CornerData = UMapDataHelper::SetFreshwater(corner.CornerData);
		}
		MapGraph->UpdateCorner(corner);
	}
}

void DistributorOfMoistures::CalculateWatersheds()
{
	for (int i = 0; i < MapGraph->GetCornerNum(); i++)
	{
		FMapCorner corner = MapGraph->GetCorner(i);
		corner.Watershed = corner.Index;
		if (!UMapDataHelper::IsOcean(corner.CornerData) && !UMapDataHelper::IsCoast(corner.CornerData))
		{
			corner.Watershed = corner.Downslope;
		}
		MapGraph->UpdateCorner(corner);
	}

	for (int i = 0; i < 100; i++)
	{
		bool bChanged = false;
		for (int j = 0; j < MapGraph->GetCornerNum(); j++)
		{
			FMapCorner corner = MapGraph->GetCorner(j);
			FMapCorner watershed = MapGraph->GetCorner(corner.Watershed);
			if (!UMapDataHelper::IsOcean(corner.CornerData) && !UMapDataHelper::IsCoast(corner.CornerData) && !UMapDataHelper::IsCoast(watershed.CornerData))
			{
				FMapCorner downstreamWatershed = MapGraph->GetCorner(watershed.Watershed);
				if (!UMapDataHelper::IsOcean(downstreamWatershed.CornerData))
				{
					corner.Watershed = downstreamWatershed.Index;
					MapGraph->UpdateCorner(corner);
					bChanged = true;
				}
			}
		}
		if (!bChanged)
		{
			break;
		}
	}

	for (int i = 0; i < MapGraph->GetCornerNum(); i++)
	{
		FMapCorner corner = MapGraph->GetCorner(i);
		FMapCorner watershed = MapGraph->GetCorner(corner.Watershed);
		watershed.WatershedSize += 1;
		MapGraph->UpdateCorner(watershed);
	}
}

void DistributorOfMoistures::CreateRivers(FRandomStream& RandomGenerator)
{
	TArray<FMapCorner> cornerList;
	cornerList.Append(MapGraph->Corners);
	while (Rivers.Num() < RiverCount)
	{
		if (cornerList.Num() == 0)
		{
			break;
		}

		int randomRiver = RandomGenerator.RandRange(0, cornerList.Num() - 1);
		FMapCorner riverSource = cornerList[randomRiver];
		cornerList.RemoveAt(randomRiver);

		if (UMapDataHelper::IsOcean(riverSource.CornerData))
		{
			continue;
		}
		if (UMapDataHelper::IsCoast(riverSource.CornerData))
		{
			continue;
		}
		if (riverSource.River != nullptr)
		{
			continue;
		}
		if (riverSource.CornerData.Elevation < MinRiverStartElevation || riverSource.CornerData.Elevation > MaxRiverStartElevation)
		{
			continue;
		}
		if (riverSource.Adjacent.Num() <= 1)
		{
			continue;
		}

		URiver* river = NewObject<URiver>();
		river->MapGraph = MapGraph;

		FString invalidRiverReason;
		FMapCorner current = riverSource;
		while (!UMapDataHelper::IsOcean(current.CornerData))
		{
			if (current.River != nullptr)
			{
				current = river->AddCorner(current, 0);
				if (!current.River->JoinRiver(river, current))
				{
					invalidRiverReason = "river could not be joined.";
				}
				break;
			}
			else
			{
				river->AddCorner(current);
			}

			FMapCorner next;
			float randomValue = RandomGenerator.GetFraction();
			if (randomValue <= RiverDownstreamBias &&
				current.Downslope != -1 &&
				current.Index != current.Downslope &&
				MapGraph->GetCorner(current.Downslope).River != river)
			{
				next = MapGraph->GetCorner(current.Downslope);
			}
			else
			{
				TArray<int32> adjacent;
				adjacent.Append(current.Adjacent);
				if (adjacent.Num() == 0)
				{
					invalidRiverReason = "there were no adjacent corners.";
					break;
				}

				for(int i = 0; i < adjacent.Num(); i++)
				{
					FMapCorner neighbor = MapGraph->GetCorner(adjacent[i]);
					if (neighbor.River == river)
					{
						continue;
					}
					randomValue = RandomGenerator.GetFraction();
					if (UMapDataHelper::IsWater(neighbor.CornerData) && randomValue <= StandingWaterBias)
					{
						next = neighbor;
						break;
					}
				}

				if (next.Index == -1)
				{
					while (adjacent.Num() >= 1)
					{
						int randomIndex = RandomGenerator.RandRange(0, adjacent.Num() - 1);
						FMapCorner possibleNext = MapGraph->GetCorner(adjacent[randomIndex]);
						adjacent.RemoveAt(randomIndex);
						if (possibleNext.River == river)
						{
							continue;
						}
						if (possibleNext.Adjacent.Num() < 1)
						{
							continue;
						}

						next = possibleNext;

						if (next.River != nullptr)
						{
							randomValue = RandomGenerator.GetFraction();
							if (randomValue > RiverJoinBias)
							{
								continue;
							}
						}
						break;
					}
				}
			}

			if (next.Index == -1)
			{
				invalidRiverReason = "there was no valid candidate for the next node.";
				break;
			}
			else if (next.River == river)
			{
				invalidRiverReason = "the river would feed into itself.";
				break;
			}

			current = next;
		}

		if (!invalidRiverReason.IsEmpty())
		{
			river->Clear();
			continue;
		}
		else
		{
			river->AddCorner(current);
			if (river->Terminates())
			{
				if (river->RiverCorners.Num() <= (int32)MinRiverSectionCount)
				{
					if (river->FeedsInto.Key == NULL)
					{
						river->Clear();
						continue;
					}
					else
					{
						river->MakeTributary();
					}
				}
				Rivers.Add(river);
			}
			else
			{
				river->Clear();
				continue;
			}
		}

		TArray<URiver*> riverConnectionList;
		riverConnectionList.Append(Rivers);
		while (riverConnectionList.Num() > 0)
		{
			URiver* r1 = riverConnectionList[0];
			riverConnectionList.RemoveAt(0);
			FMapCorner source1 = r1->GetCorner(0);
			if (UMapDataHelper::IsCoast(source1.CornerData))
			{
				continue;
			}

			TArray<URiver*> remove;
			for(int i = 0; i < riverConnectionList.Num(); i++)
			{
				URiver* r2 = riverConnectionList[i];
				if (r1->FeedsInto.Key == r2 || r2->FeedsInto.Key == r1)
				{
					continue;
				}


				FMapCorner adjacent;
				FMapCorner source2 = r2->GetCorner(0);
				if (UMapDataHelper::IsCoast(source2.CornerData))
				{
					continue;
				}
				for(int j = 0; j < source1.Adjacent.Num(); j++)
				{
					FMapCorner adjacent1 = MapGraph->GetCorner(source1.Adjacent[j]);
					if (UMapDataHelper::IsCoast(adjacent1.CornerData))
					{
						continue;
					}
					if (adjacent1 == source2)
					{
						adjacent = adjacent1;
						break;
					}
				}

				if (adjacent.Index != -1)
				{
					if (RiverLakeConversionFactor >= RandomGenerator.GetFraction())
					{
						FMapCenter center = MapGraph->FindCenterFromCorners(source1, adjacent);
						if (UMapDataHelper::IsCoast(center.CenterData))
						{
							continue;
						}

						center.CenterData = UMapDataHelper::SetFreshwater(center.CenterData);
						for(int j = 0; j < center.Corners.Num(); j++)
						{
							FMapCorner c = MapGraph->GetCorner(center.Corners[j]);
							if (UMapDataHelper::IsCoast(c.CornerData))
							{
								continue;
							}
							c.CornerData = UMapDataHelper::SetFreshwater(c.CornerData);
							MapGraph->UpdateCorner(c);
						}
						MapGraph->UpdateCenter(center);
					}
					remove.Add(r2);
				}
			}
			for(int i = 0; i < remove.Num(); i++)
			{
				riverConnectionList.Remove(remove[i]);
			}
		}
	}

	for (int i = 0; i < Rivers.Num(); i++)
	{
		URiver* r = Rivers[i];
		for (int j = 0; j < r->RiverCorners.Num(); j++)
		{
			FMapCorner corner = r->GetCorner(j);
			corner.RiverSize = FMath::Clamp(corner.RiverSize, 0, (int32)MaxRiverSize);
			if(corner.RiverSize > 0)
			{
				corner.CornerData = UMapDataHelper::SetRiver(corner.CornerData);
			}
			MapGraph->UpdateCorner(corner);
		}
	}
}
void DistributorOfMoistures::AssignCornerMoisture()
{
	TQueue<FMapCorner> moistureQueue;
	for (int i = 0; i < MapGraph->GetCornerNum(); i++)
	{
		FMapCorner corner = MapGraph->GetCorner(i);
		if ((UMapDataHelper::IsWater(corner.CornerData) || UMapDataHelper::IsRiver(corner.CornerData)) && !UMapDataHelper::IsOcean(corner.CornerData))
		{
			corner.CornerData.Moisture = UMapDataHelper::IsRiver(corner.CornerData) ? FMath::Min(3.0f, (0.2f * corner.RiverSize)) : 1.0f;
			moistureQueue.Enqueue(corner);
		}
		else
		{
			corner.CornerData.Moisture = 0.0f;
			MapGraph->UpdateCorner(corner);
		}
	}

	while (!moistureQueue.IsEmpty())
	{
		FMapCorner corner;
		moistureQueue.Dequeue(corner);
		for (int i = 0; i < corner.Adjacent.Num(); i++)
		{
			FMapCorner neighbor = MapGraph->GetCorner(corner.Adjacent[i]);
			float newMoisture = corner.CornerData.Moisture * 0.9f;
			if (newMoisture > neighbor.CornerData.Moisture)
			{
				neighbor.CornerData.Moisture = newMoisture;
				moistureQueue.Enqueue(neighbor);
			}
		}
		MapGraph->UpdateCorner(corner);
	}

	for (int i = 0; i < MapGraph->GetCornerNum(); i++)
	{
		FMapCorner corner = MapGraph->GetCorner(i);
		if (UMapDataHelper::IsCoast(corner.CornerData) || UMapDataHelper::IsOcean(corner.CornerData))
		{
			corner.CornerData.Moisture = 1.0f;
			MapGraph->UpdateCorner(corner);
		}
	}
}

void DistributorOfMoistures::RedistributeMoisture(TArray<int32> landCorners)
{
	float maxMoisture = -1.0f;
	for (int i = 0; i < landCorners.Num(); i++)
	{
		FMapCorner corner = MapGraph->GetCorner(landCorners[i]);
		if (corner.CornerData.Moisture > maxMoisture)
		{
			maxMoisture = corner.CornerData.Moisture;
		}
	}

	for (int i = 0; i < landCorners.Num(); i++)
	{
		FMapCorner corner = MapGraph->GetCorner(landCorners[i]);
		corner.CornerData.Moisture /= maxMoisture;
		MapGraph->UpdateCorner(corner);
	}
}

void DistributorOfMoistures::AssignPolygonMoisture()
{
	for (int i = 0; i < MapGraph->GetCenterNum(); i++)
	{
		FMapCenter center = MapGraph->GetCenter(i);
		float sumMoisture = 0.0f;
		for (int j = 0; j < center.Corners.Num(); j++)
		{
			FMapCorner corner = MapGraph->GetCorner(center.Corners[j]);
			sumMoisture = corner.CornerData.Moisture;
		}
		center.CenterData.Moisture = sumMoisture / center.Corners.Num();
		MapGraph->UpdateCenter(center);
	}
}