#pragma once

#include "Components/ActorComponent.h"
#include "Engine/World.h"
#include "GameplayTagContainer.h"
#include "PolygonMap.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogWorldGen, Log, All);
DECLARE_DYNAMIC_DELEGATE(FIslandGeneratorDelegate);

USTRUCT(BlueprintType)
struct POLYGONALMAPGENERATOR_API FMapData
{
	GENERATED_BODY()

	UPROPERTY(Category = "Map Graph", BlueprintReadWrite, EditAnywhere)
	FVector2D Point;

	UPROPERTY(Category = "Map Biome", BlueprintReadWrite, EditAnywhere)
	float Elevation;

    UPROPERTY(Category = "Map Biome", BlueprintReadWrite, EditAnywhere)
	float Moisture;

	UPROPERTY(Category = "Map Biome", BlueprintReadWrite, EditAnywhere, meta = (Categories = "MapData.Biome"))
	FGameplayTag Biome;

	UPROPERTY(Category = "Map Biome", BlueprintReadWrite, EditAnywhere, meta = (Categories = "MapData.MetaData"))
	FGameplayTagContainer Tags;

	FMapData()
	{
		Point = FVector2D::ZeroVector;

		Tags.AddTagFast(FGameplayTag::RequestGameplayTag(TEXT("MapData.MetaData.Border")));
		Tags.AddTagFast(FGameplayTag::RequestGameplayTag(TEXT("MapData.MetaData.Water.Saltwater")));

		Elevation = 0.0f;
		Moisture = 0.0f;
	}
};

struct FMapCorner;
struct FMapEdge;
class URiver;

USTRUCT(BlueprintType)
struct POLYGONALMAPGENERATOR_API FMapCenter
{
	GENERATED_BODY()

	UPROPERTY(Category = "Map Graph", BlueprintReadWrite, VisibleAnywhere)
	int32 Index;

	UPROPERTY(Category = "Map Graph", BlueprintReadWrite, EditAnywhere)
	FMapData CenterData;

	UPROPERTY(Category = "Map Graph", BlueprintReadOnly, VisibleAnywhere)
	TArray<int32> Neighbors;

    UPROPERTY(Category = "Map Graph", BlueprintReadOnly, VisibleAnywhere)
	TArray<int32> Borders;

    UPROPERTY(Category = "Map Graph", BlueprintReadOnly, VisibleAnywhere)
	TArray<int32> Corners;

	FMapCenter()
	{
		Index = -1;
	}

	FORCEINLINE	bool operator==(const FMapCenter& Other) const
	{
		return Other.Index == Index;
	}
};

USTRUCT(BlueprintType)
struct POLYGONALMAPGENERATOR_API FMapCorner
{
	GENERATED_BODY()

	UPROPERTY(Category = "Map Graph", BlueprintReadWrite, VisibleAnywhere)
	int32 Index;

	UPROPERTY(Category = "Map Graph", BlueprintReadWrite, EditAnywhere)
	FMapData CornerData;

	UPROPERTY(Category = "Map Graph", BlueprintReadOnly, VisibleAnywhere)
	TArray<int32> Touches;

    UPROPERTY(Category = "Map Graph", BlueprintReadOnly, VisibleAnywhere)
	TArray<int32> Protrudes;

    UPROPERTY(Category = "Map Graph", BlueprintReadOnly, VisibleAnywhere)
	TArray<int32> Adjacent;

	UPROPERTY(Category = "Watershed", BlueprintReadWrite, EditAnywhere)
	int32 Downslope;

    UPROPERTY(Category = "Watershed", BlueprintReadWrite, EditAnywhere)
	int32 Watershed;

    UPROPERTY(Category = "Watershed", BlueprintReadWrite, EditAnywhere)
	int32 WatershedSize;

	UPROPERTY(Category = "River", BlueprintReadWrite, EditAnywhere)
	int32 RiverSize;

	UPROPERTY(Category = "River", BlueprintReadWrite, EditAnywhere)
	int32 RiverIndex;

    UPROPERTY(Category = "River", BlueprintReadWrite, EditAnywhere)
	URiver* River;

	FMapCorner()
	{
		Index = -1;
		RiverSize = 0;
		Downslope = -1;
		Watershed = -1;
		RiverIndex = -1;
		WatershedSize = 0;
		River = NULL;
	}

	FORCEINLINE bool operator <(const FMapCorner& c) const
	{
		return CornerData.Elevation < c.CornerData.Elevation;
	}

	FORCEINLINE	bool operator==(const FMapCorner& Other) const
	{
		return Other.Index == Index;
	}
};

USTRUCT(BlueprintType)
struct POLYGONALMAPGENERATOR_API FMapEdge
{
	GENERATED_BODY()

    UPROPERTY(Category = "Map Graph", BlueprintReadWrite, VisibleAnywhere)
	int32 Index;

	UPROPERTY(Category = "Map Graph", BlueprintReadOnly, VisibleAnywhere)
	int32 DelaunayEdge0;

	UPROPERTY(Category = "Map Graph", BlueprintReadOnly, VisibleAnywhere)
	int32 DelaunayEdge1;

	UPROPERTY(Category = "Map Graph", BlueprintReadOnly, VisibleAnywhere)
	int32 VoronoiEdge0;

	UPROPERTY(Category = "Map Graph", BlueprintReadOnly, VisibleAnywhere)
	int32 VoronoiEdge1;

	UPROPERTY(Category = "Map Graph", BlueprintReadOnly, VisibleAnywhere)
	FVector2D Midpoint;

	UPROPERTY(Category = "Map Biome", BlueprintReadWrite, EditAnywhere)
	int32 RiverVolume;

	FMapEdge()
	{
		Index = -1;
		DelaunayEdge0 = -1;
		DelaunayEdge1 = -1;
		VoronoiEdge0 = -1;
		VoronoiEdge1 = -1;

		RiverVolume = 0;
	}

	FORCEINLINE	bool operator==(const FMapEdge& Other) const
	{
		return Other.Index == Index;
	}
};

USTRUCT(BlueprintType)
struct POLYGONALMAPGENERATOR_API FWorldSpaceMapData
{
	GENERATED_BODY()

	UPROPERTY(Category = "Map", BlueprintReadWrite, EditAnywhere)
	int32 PointSize;

	UPROPERTY(Category = "Map", BlueprintReadWrite, EditAnywhere)
	float ElevationOffset;

    UPROPERTY(Category = "Map", BlueprintReadWrite, EditAnywhere)
	float XYScaleFactor;

	UPROPERTY(Category = "Map", BlueprintReadWrite, EditAnywhere)
	float ElevationScale;

	FWorldSpaceMapData()
	{
		PointSize = 16;
		ElevationOffset = 0.0f;
		XYScaleFactor = 100.0f;
		ElevationScale = 3200.0f;
	}
};

UCLASS(Blueprintable)
class POLYGONALMAPGENERATOR_API UPolygonMap : public UActorComponent
{
	GENERATED_BODY()
	friend class AIslandMapGenerator;

public:
	UFUNCTION(BlueprintCallable, Category = "Island Generation|Graph")
	void CreatePoints(UPointGenerator* PointSelector, const int32& numberOfPoints);

	UFUNCTION(BlueprintCallable, Category = "Island Generation|Graph")
	void BuildGraph(const int32& mapSize, const FWorldSpaceMapData& data);

	UFUNCTION(BlueprintCallable, Category = "Island Generation|Graph")
	FMapCenter MakeCenter(const FVector2D& point);
	UFUNCTION(BlueprintCallable, Category = "Island Generation|Graph")
	FMapCorner MakeCorner(const FVector2D& point);

	UFUNCTION(BlueprintCallable, Category = "Island Generation|Graph")
	void ImproveCorners();

	UFUNCTION(BlueprintPure, Category = "Island Generation|Graph")
	FMapCenter GetCenter(const int32& index) const;

	UFUNCTION(BlueprintPure, Category = "Island Generation|Graph")
	FMapCorner GetCorner(const int32& index) const;

	UFUNCTION(BlueprintPure, Category = "Island Generation|Graph")
	FMapEdge GetEdge(const int32& index) const;

	UFUNCTION(BlueprintCallable, Category = "Island Generation|Graph")
	void UpdateCenter(const FMapCenter& center);

	UFUNCTION(BlueprintCallable, Category = "Island Generation|Graph")
	void UpdateCorner(const FMapCorner& corner);

	UFUNCTION(BlueprintCallable, Category = "Island Generation|Graph")
	void UpdateEdge(const FMapEdge& edge);

	UFUNCTION(BlueprintPure, Category = "Island Generation|Graph")
	int32 GetCenterNum() const;

	UFUNCTION(BlueprintPure, Category = "Island Generation|Graph")
	int32 GetCornerNum() const;

    UFUNCTION(BlueprintPure, Category = "Island Generation|Graph")
	int32 GetEdgeNum() const;

	UFUNCTION(BlueprintPure, Category = "Island Generation|Graph")
	int32 GetGraphSize() const;

	UFUNCTION(BlueprintPure, Category = "Island Generation|MapData")
	TArray<FMapData>& GetAllMapData();

	UFUNCTION(BlueprintPure, Category = "Island Generation|Graph")
	TArray<int32> FindLandCorners() const;

	UFUNCTION(BlueprintPure, Category = "Island Generation|Graph")
	FMapEdge FindEdgeFromCenters(const FMapCenter& v0, const FMapCenter& v1) const;

	UFUNCTION(BlueprintPure, Category = "Island Generation|Graph")
	FMapEdge FindEdgeFromCorners(const FMapCorner& v0, const FMapCorner& v1) const;

	UFUNCTION(BlueprintPure, Category = "Island Generation|Graph")
	FMapCenter FindCenterFromCorners(FMapCorner CornerA, FMapCorner CornerB) const;

	UFUNCTION(BlueprintCallable, Category = "Island Generation|Graph")
	void CompileMapData();

	UFUNCTION(BlueprintPure, Category = "Island Generation")
	static FVector ConvertGraphPointToWorldSpace(const FMapData& MapData, const FWorldSpaceMapData& WorldData, int32 MapSize);

	UFUNCTION(BlueprintPure, Category = "Island Generation|Graph")
	FMapCenter FindPolygonLocalSpace(const FVector2D& Point) const;

	UFUNCTION(BlueprintPure, Category = "Island Generation|Graph")
	bool PolygonContainsPoint(const FVector2D& Point, const FMapCenter& Center) const;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Graph")
	TArray<FVector2D> Points;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Graph")
	TArray<FMapCenter> Centers;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Graph")
	TArray<FMapCorner> Corners;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Graph")
	TArray<FMapEdge> Edges;

private:
	UPROPERTY()
	UPointGenerator* PointSelector;

    UPROPERTY()
	int32 MapSize;

    UPROPERTY()
	FWorldSpaceMapData MapData;

	UPROPERTY()
	TArray<FMapData> CachedMapData;

    UPROPERTY()
	TMap<FVector2D, int32> CenterLookup;

    UPROPERTY()
	TMap<FVector2D, int32> CornerLookup;

	UFUNCTION()
	bool SegementsIntersect(const FMapEdge& Edge, const FVector2D& StartPoint, const FVector2D& EndPoint) const;
};
