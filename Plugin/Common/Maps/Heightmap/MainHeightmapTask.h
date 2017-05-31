
#pragma once

#include "Array.h"
#include "Maps/Biomes/BiomeManager.h"
#include "Maps/PolygonMap.h"
#include "Async/TaskGraphInterfaces.h"

struct FMapData;
class UPolygonalMapHeightmap;

enum EPointSelectionMode : uint8
{
	UsePolygon,
	Interpolated,
	InterpolatedWithPolygonBiome
};

/**
 * 
 */
class MainHeightmapTask
{

public:
	MainHeightmapTask(int32 XCoord, int32 YCoord, int32 NumberOfPoints, EPointSelectionMode SelectionMode)
	{
		X = XCoord;
		Y = YCoord;
		NumberOfPointsToAverage = NumberOfPoints;
		PointSelectionMode = SelectionMode;
	}

	int32 X;
	int32 Y;
	int32 NumberOfPointsToAverage;
	EPointSelectionMode PointSelectionMode;

	static const TCHAR* GetTaskName()
	{
		return TEXT("FHeightmapPointTask");
	}
	FORCEINLINE static TStatId GetStatId()
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(MainHeightmapTask, STATGROUP_TaskGraphTasks);
	}
	static ENamedThreads::Type GetDesiredThread()
	{
		return ENamedThreads::AnyThread;
	}

	static ESubsequentsMode::Type GetSubsequentsMode()
	{
		return ESubsequentsMode::FireAndForget;
	}

	FMapData MakeMapPoint(FVector2D PixelPosition, TArray<FMapData> MapData, BiomesMgr* BiomeManager);

	void DoTask(ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent);
};

class MainHeightmapTask
{
public:
	static UPolygonalMapHeightmap* MapHeightmap;
	static UPolygonMap* MapGraph;
	static BiomesMgr* BiomeManager;

	static TArray<FMapData> HeightmapData;
	static TArray<FMapData> StartingMapDataArray;

	static FGraphEventArray CompletionEvents;

	static int32 CompletedThreads;
	static int32 TotalNumberOfThreads;
	static FIslandGeneratorDelegate OnAllPointsComplete;

	static bool TasksAreComplete();

	static void GenerateHeightmapPoints(const int32 HeightmapSize, int32 NumberOfPointsToAverage, UPolygonalMapHeightmap* HeightmapGenerator, UPolygonMap* Graph, BiomesMgr* BiomeMgr, const FIslandGeneratorDelegate OnComplete);

	static void CheckComplete();
};