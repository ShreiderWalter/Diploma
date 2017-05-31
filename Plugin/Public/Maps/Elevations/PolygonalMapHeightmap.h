#pragma once

#include "Object.h"
#include "PolygonMap.h"
#include "PolygonalMapHeightmap.generated.h"

class DistributorOfMoistures;

UCLASS(BlueprintType)
class POLYGONALMAPGENERATOR_API UPolygonalMapHeightmap : public UObject
{
	GENERATED_BODY()

private:
	UPROPERTY()
	TArray<FMapData> HeightmapData;
	UPROPERTY()
	DistributorOfMoistures* MoistureDistributor;

	UPROPERTY()
	float CreateHeightmapTimer;

	FIslandGeneratorDelegate OnGenerationComplete;

	UFUNCTION()
	void CheckMapPointsDone();
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Heightmap")
	int32 HeightmapSize;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Heightmap")
	int32 NumberOfPointsToAverage = 4;

	UFUNCTION(BlueprintCallable, Category = "Map Generation|Heightmap")
	void CreateHeightmap(UPolygonMap* PolygonMap, BiomesMgr* BiomeManager, DistributorOfMoistures* MoistureDist, const int32 Size, const FIslandGeneratorDelegate onComplete);
	UFUNCTION(BlueprintPure, Category = "Map Generation|Heightmap")
	TArray<FMapData> GetMapData();

	UFUNCTION(BlueprintPure, Category = "Map Generation|Heightmap")
	FMapData GetMapPoint(int32 x, int32 y);

	UFUNCTION(BlueprintCallable, Category = "Map Generation|Heightmap")
	void SetMapPoint(int32 X, int32 Y, FMapData MapData);
};