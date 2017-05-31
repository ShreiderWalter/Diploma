#include "PolygonalMapGeneratorPrivatePCH.h"
#include "PolygonalMapHeightmap.h"
#include "Async/Async.h"
#include "Maps/Heightmap/MainHeightmapTask.h"

UPolygonalMapHeightmap* MainHeightmapTask::MapHeightmap = NULL;
UPolygonMap* MainHeightmapTask::MapGraph = NULL;
BiomesMgr* MainHeightmapTask::BiomeManager = NULL;

TArray<FMapData> MainHeightmapTask::HeightmapData = TArray<FMapData>();
TArray<FMapData> MainHeightmapTask::StartingMapDataArray = TArray<FMapData>();

FGraphEventArray MainHeightmapTask::CompletionEvents = FGraphEventArray();
int32 MainHeightmapTask::CompletedThreads = 0;
int32 MainHeightmapTask::TotalNumberOfThreads = 0;

FIslandGeneratorDelegate MainHeightmapTask::OnAllPointsComplete;

bool MainHeightmapTask::TasksAreComplete()
{
	return CompletedThreads == TotalNumberOfThreads;
}

void MainHeightmapTask::GenerateHeightmapPoints(const int32 HeightmapSize, int32 NumberOfPointsToAverage, UPolygonalMapHeightmap* HeightmapGenerator, UPolygonMap* Graph, BiomesMgr* BiomeMgr, const FIslandGeneratorDelegate OnComplete)
{
	MapHeightmap = HeightmapGenerator;
	MapGraph = Graph;
	BiomeManager = BiomeMgr;
	OnAllPointsComplete = OnComplete;

	TotalNumberOfThreads = 0;
	CompletedThreads = 0;
	HeightmapData.Empty();

	StartingMapDataArray = MainHeightmapTask::MapGraph->GetAllMapData();

	EPointSelectionMode pointSelectionMode = EPointSelectionMode::InterpolatedWithPolygonBiome;

	if (pointSelectionMode == EPointSelectionMode::Interpolated || pointSelectionMode == EPointSelectionMode::InterpolatedWithPolygonBiome)
	{
		int32 graphSize = MainHeightmapTask::MapGraph->GetGraphSize();
		for (int x = 0; x < graphSize; x++)
		{
			FMapData borderPoint = FMapData();
			borderPoint.Elevation = 0.0f;
			borderPoint.Moisture = 0.0f;
			borderPoint = UMapDataHelper::SetOcean(borderPoint);
			borderPoint = UMapDataHelper::SetBorder(borderPoint);
			borderPoint.Point = FVector2D(x, 0);
			StartingMapDataArray.Add(borderPoint);
			borderPoint.Point = FVector2D(x, graphSize - 1);
			StartingMapDataArray.Add(borderPoint);
		}
		for (int y = 0; y < graphSize; y++)
		{
			FMapData borderPoint = FMapData();
			borderPoint.Elevation = 0.0f;
			borderPoint.Moisture = 0.0f;
			borderPoint = UMapDataHelper::SetOcean(borderPoint);
			borderPoint = UMapDataHelper::SetBorder(borderPoint);
			borderPoint.Point = FVector2D(0, y);
			StartingMapDataArray.Add(borderPoint);
			borderPoint.Point = FVector2D(graphSize - 1, y);
			StartingMapDataArray.Add(borderPoint);
		}
	}

	for (int32 x = 0; x < HeightmapSize; x++)
	{
		for(int32 y = 0; y < HeightmapSize; y++)
		{
			CompletionEvents.Add(TGraphTask<MainHeightmapTask>::CreateTask(NULL, ENamedThreads::GameThread).ConstructAndDispatchWhenReady(x, y, NumberOfPointsToAverage, pointSelectionMode));
			TotalNumberOfThreads++;
		}
	}
}

void MainHeightmapTask::CheckComplete()
{
	if(TasksAreComplete())
	{
		MainHeightmapTask::TotalNumberOfThreads = 0;
		MainHeightmapTask::CompletedThreads = 0;

		UE_LOG(LogWorldGen, Log, TEXT("Heightmap is complete!"));

		AsyncTask(ENamedThreads::GameThread, []() {
			OnAllPointsComplete.Execute();
			OnAllPointsComplete.Unbind();
		});
	}
}

FMapData MainHeightmapTask::MakeMapPoint(FVector2D PixelPosition, TArray<FMapData> MapData, BiomesMgr* BiomeManager)
{
	FMapData pixelData;
	if (PointSelectionMode == Interpolated || PointSelectionMode == InterpolatedWithPolygonBiome)
	{
		TArray<FMapData> closestPoints;
		for (int i = 0; i < MapData.Num(); i++)
		{
			if (closestPoints.Num() == 0)
			{
				closestPoints.Add(MapData[i]);
				continue;
			}
			float distance = FVector2D::DistSquared(PixelPosition, MapData[i].Point);
			if (distance <= 0.001f)
			{
				pixelData = MapData[i];
				return pixelData;
			}

			int addPointIndex = -1;
			for (int j = 0; j < closestPoints.Num(); j++)
			{
				float pointDist = FVector2D::DistSquared(PixelPosition, closestPoints[j].Point);
				if (distance < pointDist)
				{
					addPointIndex = j;
					break;
				}
			}

			if (addPointIndex >= 0)
			{
				FMapData last = MapData[i];
				for (int j = addPointIndex; j < closestPoints.Num(); j++)
				{
					FMapData temp = closestPoints[j];
					closestPoints[j] = last;
					last = temp;
				}
				if (closestPoints.Num() < NumberOfPointsToAverage)
				{
					closestPoints.Add(last);
				}
			}
			else if (closestPoints.Num() < NumberOfPointsToAverage)
			{
				closestPoints.Add(MapData[i]);
			}
		}

		TArray<float> closestPointDistances;
		float totalDistance = 0.0f;
		for (int i = 0; i < closestPoints.Num(); i++)
		{
			float distance = FVector2D::DistSquared(PixelPosition, closestPoints[i].Point);
			totalDistance += distance;
			closestPointDistances.Add(distance);
		}

		float inversePercentageTotal = 0.0f;

		for (int i = 0; i < closestPoints.Num(); i++)
		{
			float percentageOfDistance = closestPointDistances[i] / totalDistance;

			float inversePercentage = 1.0f - percentageOfDistance;

			closestPointDistances[i] = inversePercentage;
			inversePercentageTotal += inversePercentage;
		}

		TArray<TPair<FMapData, float>> pointWeights;
		for (int i = 0; i < closestPoints.Num(); i++)
		{
			TPair<FMapData, float> weight;
			weight.Key = closestPoints[i];
			weight.Value = closestPointDistances[i] / inversePercentageTotal;
			pointWeights.Add(weight);
		}

		float elevation = 0.0f;
		float moisture = 0.0f;

		TMap<FGameplayTag, float> tagWeights;

		for (int i = 0; i < pointWeights.Num(); i++)
		{
			FMapData curPoint = pointWeights[i].Key;
			float weight = pointWeights[i].Value;

			elevation += (curPoint.Elevation * weight);
			moisture += (curPoint.Moisture * weight);

			if(PointSelectionMode == EPointSelectionMode::Interpolated)
			{
				for (int j = 0; j < curPoint.Tags.Num(); j++)
				{
					FGameplayTag tag = curPoint.Tags.GetByIndex(i);
					if (tag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("MapData.MetaData.Water.River"))))
					{
						continue;
					}
					float currentTagWeight = tagWeights.FindOrAdd(tag);
					currentTagWeight += weight;
					tagWeights[tag] = currentTagWeight;
				}
			}
		}
		pixelData.Elevation = elevation;
		pixelData.Moisture = moisture;
		pixelData.Point = PixelPosition;
		if (PointSelectionMode == EPointSelectionMode::Interpolated)
		{
			pixelData.Tags.Reset();
			for (auto& elem : tagWeights)
			{
				if (elem.Value >= 0.5f)
				{
					pixelData.Tags.AddTagFast(elem.Key);
				}
			}
			pixelData.Biome = BiomeManager->DetermineBiome(pixelData);
		}
		else if(PointSelectionMode == EPointSelectionMode::InterpolatedWithPolygonBiome)
		{
			FMapCenter center = MainHeightmapTask::MapGraph->FindPolygonLocalSpace(PixelPosition);
			if (center.Index < 0)
			{
				return pixelData;
			}
			pixelData.Tags = center.CenterData.Tags;
			pixelData.Biome = center.CenterData.Biome;
		}
		return pixelData;
	}
	else if(PointSelectionMode == EPointSelectionMode::UsePolygon)
	{
		FMapCenter center = MainHeightmapTask::MapGraph->FindPolygonLocalSpace(PixelPosition);
		if (center.Index < 0)
		{
			return FMapData();
		}

		pixelData = center.CenterData;
		pixelData.Point = PixelPosition;
		return pixelData;
	}
	else
	{
		unimplemented();
		return FMapData();
	}
}

void MainHeightmapTask::DoTask(ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent)
{
	FVector2D point = FVector2D(X, Y);

	FMapData mapData = MakeMapPoint(point, MainHeightmapTask::StartingMapDataArray, MainHeightmapTask::BiomeManager);
	MainHeightmapTask::HeightmapData.Add(mapData);
	MainHeightmapTask::CompletedThreads++;

	float percentComplete = (float)MainHeightmapTask::CompletedThreads / (float)MainHeightmapTask::TotalNumberOfThreads;
	UE_LOG(LogWorldGen, Log, TEXT("Heightmap completion percent: %f percent."), percentComplete);
	if (MainHeightmapTask::CompletedThreads == MainHeightmapTask::TotalNumberOfThreads)
	{
		MainHeightmapTask::CheckComplete();
	}
}
