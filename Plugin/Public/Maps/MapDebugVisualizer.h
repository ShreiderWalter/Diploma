#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "PolygonMap.h"
#include "Moisture/River.h"
#include "MapDebugVisualizer.generated.h"
UCLASS(BlueprintType)
class POLYGONALMAPGENERATOR_API UMapDebugVisualizer : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()	
public:
	UFUNCTION(BlueprintCallable, Category = "Map Generation|Debug")
	static void DrawDebugPixelGrid(AActor* Actor, const FWorldSpaceMapData& MapData, const TArray<FMapData>& HeightmapData, int32 HeightmapSize, float PixelSize);
	
	UFUNCTION(BlueprintCallable, Category = "Map Generation|Debug")
	static void DrawDebugPixelRivers(AActor* Actor, const FWorldSpaceMapData& MapData, const TArray<FMapData>& HeightmapData, int32 HeightmapSize, float PixelSize);
	
	UFUNCTION(BlueprintCallable, Category = "Map Generation|Debug")
	static void DrawDebugVoronoiGrid(AActor* Actor, const FWorldSpaceMapData& MapData, const TArray<FMapCorner>& Corners, const TArray<FMapEdge>& Edges, int32 MapSize);

	UFUNCTION(BlueprintCallable, Category = "Map Generation|Debug")
	static void DrawDebugDelaunayGrid(AActor* Actor, const FWorldSpaceMapData& MapData, const TArray<FMapCenter>& Centers, const TArray<FMapEdge>& Edges, int32 MapSize);
	
	UFUNCTION(BlueprintCallable, Category = "Map Generation|Debug")
	static void DrawRivers(AActor* Actor, const FWorldSpaceMapData& MapData, UPolygonMap* MapGraph, const TArray<URiver*>& Rivers, int32 MapSize);

    UFUNCTION(BlueprintCallable, Category = "Map Generation|Debug")
	static void DrawBeizerCurve(AActor* Actor, const FWorldSpaceMapData& WorldData, FVector2D v0, FVector2D control0, FVector2D v1, FVector2D control1, FColor color, int32 MapSize);

private:
	UFUNCTION(BlueprintPure, Category = "Map Generation|Debug")
	static FVector2D CalculateBezierPoint(float t, FVector2D p0, FVector2D p1, FVector2D p2, FVector2D p3);
};
