#pragma once

#include "GameFramework/Actor.h"
#include "PolygonMap.h"
#include "Maps/Elevations/ElevationDistributor.h"
#include "Maps/PointGenerators/PointGenerator.h"
#include "Maps/Biomes/BiomeManager.h"
#include "Maps/Moisture/MoistureDistributor.h"
#include "PolygonalMapHeightmap.h"
#include "Maps/IslandShapes/RadialIsland.h"
#include "IslandMapGenerator.generated.h"

USTRUCT(BlueprintType)
struct POLYGONALMAPGENERATOR_API FIslandData
{
	GENERATED_BODY()

	UPROPERTY(Category = "Island", BlueprintReadWrite, EditAnywhere)
	TSubclassOf<UIslandShape> IslandType;

	UPROPERTY(Category = "Points", BlueprintReadWrite, EditAnywhere)
	TSubclassOf<UPointGenerator> IslandPointSelector;

	UPROPERTY(Category = "Biomes", BlueprintReadWrite, EditAnywhere)
	TSubclassOf<BiomesMgr> BiomeManager;

	UPROPERTY(Category = "Biomes", BlueprintReadWrite, EditAnywhere)
	TSubclassOf<DistributorOfElevations> ElevationDistributor;

	UPROPERTY(Category = "Biomes", BlueprintReadWrite, EditAnywhere)
	TSubclassOf<DistributorOfMoistures> MoistureDistributor;

	UPROPERTY(Category = "Island", BlueprintReadWrite, EditAnywhere)
	int32 Seed;

	UPROPERTY(Category = "Island", BlueprintReadWrite, EditAnywhere)
	int32 Size;

	UPROPERTY(Category = "Points", BlueprintReadWrite, EditAnywhere)
	int32 NumberOfPoints;

	UPROPERTY(Category = "Map", BlueprintReadWrite, EditAnywhere)
	FWorldSpaceMapData PolygonMapSettings;

	FIslandData()
	{
		Size = 1024;
		NumberOfPoints = 1500;

		IslandType = URadialIsland::StaticClass();
		IslandPointSelector = UPointGenerator::StaticClass();
		BiomeManager = BiomesMgr::StaticClass();
		ElevationDistributor = DistributorOfElevations::StaticClass();
		MoistureDistributor = DistributorOfMoistures::StaticClass();
	}
};

UCLASS(Blueprintable)
class POLYGONALMAPGENERATOR_API AIslandMapGenerator : public AActor
{
	GENERATED_BODY()
public:

	AIslandMapGenerator();
	~AIslandMapGenerator() {};

	virtual void Tick(float deltaSeconds) override;
	UFUNCTION(BlueprintCallable, Category = "Island Generation")
	void SetData(FIslandData islandData);

	UFUNCTION(BlueprintCallable, Category = "Island Generation")
	void ResetMap();

	UFUNCTION(BlueprintCallable, Category = "Island Generation")
	void CreateMap(const FIslandGeneratorDelegate onComplete);

	UFUNCTION(BlueprintPure, Category = "Island Generation|Graph")
	FMapCenter GetCenter(const int32 index) const;

	UFUNCTION(BlueprintPure, Category = "Island Generation|Graph")
	FMapCorner GetCorner(const int32 index) const;

	UFUNCTION(BlueprintPure, Category = "Island Generation|Graph")
	FMapEdge GetEdge(const int32 index) const;

	UFUNCTION(BlueprintPure, Category = "Island Generation|Graph")
	FMapEdge FindEdgeFromCenters(const FMapCenter& v0, const FMapCenter& v1) const;

	UFUNCTION(BlueprintPure, Category = "Island Generation|Graph")
	FMapEdge FindEdgeFromCorners(const FMapCorner& v0, const FMapCorner& v1) const;

	UFUNCTION(BlueprintPure, Category = "Island Generation|Graph")
	int32 GetCenterNum() const;

	UFUNCTION(BlueprintPure, Category = "Island Generation|Graph")
	int32 GetCornerNum() const;

	UFUNCTION(BlueprintPure, Category = "Island Generation|Graph")
	int32 GetEdgeNum() const;

	UFUNCTION(BlueprintPure, Category = "Island Generation|Graph")
	UPolygonMap* GetGraph() const;

	UFUNCTION(BlueprintPure, Category = "Island Generation|Graph")
	UPolygonalMapHeightmap* GetHeightmap() const;

	UFUNCTION(BlueprintCallable, Category = "Island Generation|Graph")
	void UpdateCenter(const FMapCenter& center);
	
	UFUNCTION(BlueprintCallable, Category = "Island Generation|Graph")
	void UpdateCorner(const FMapCorner& corner);
	
	UFUNCTION(BlueprintCallable, Category = "Island Generation|Graph")
	void UpdateEdge(const FMapEdge& edge);

	UFUNCTION(BlueprintCallable, Category = "Island Generation")
	void AddGenerationStep(const FIslandGeneratorDelegate step);

	UFUNCTION(BlueprintCallable, Category = "Island Generation")
	void ClearAllGenerationSteps();

	UFUNCTION(BlueprintCallable, Category = "Island Generation|Map")
	void CreateHeightmap(const int32 HeightmapSize, const FIslandGeneratorDelegate OnComplete);

	UFUNCTION(BlueprintCallable, Category = "Island Generation|Debug")
	void DrawVoronoiGraph();

	UFUNCTION(BlueprintCallable, Category = "Island Generation|Debug")
	void DrawDelaunayGraph();

	UFUNCTION(BlueprintCallable, Category = "Island Generation|Debug")
	void DrawHeightmap(float PixelSize = 100.0f);

	UPROPERTY(Category = "Island", BlueprintReadWrite, EditAnywhere)
	FIslandData IslandData;

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Island Generation")
	void AddMapSteps();
	virtual void AddMapSteps_Implementation();

	UFUNCTION(BlueprintCallable, Category = "Island Generation")
	void ExecuteNextMapStep();

	UFUNCTION(BlueprintImplementableEvent, Category = "Island Generation|Map")
	void ResetMapEvent();

	UFUNCTION(BlueprintImplementableEvent, Category = "Island Generation|Map")
	void InitializeMapEvent();
	UFUNCTION(BlueprintCallable, Category = "Island Generation|Map")
	void InitializeMapClasses();

	UFUNCTION(BlueprintImplementableEvent, Category = "Island Generation|Map")
	void BuildGraphEvent();
	UFUNCTION(BlueprintCallable, Category = "Island Generation|Map")
	void BuildGraph();

	UFUNCTION(BlueprintImplementableEvent, Category = "Island Generation|Map")
	void AssignElevationEvent();

	UFUNCTION(BlueprintCallable, Category = "Island Generation|Map")
	void AssignElevation();

	UFUNCTION(BlueprintImplementableEvent, Category = "Island Generation|Map")
	void AssignMoistureEvent();

    UFUNCTION(BlueprintCallable, Category = "Island Generation|Map")
	void AssignMoisture();

	UFUNCTION(BlueprintCallable, Category = "Island Generation|Map")
	virtual void DoPointPostProcess();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Island Generation|Map")
	void NormalizePointsEvent();

	UFUNCTION(BlueprintCallable, Category = "Island Generation|Map")
	void NormalizePoints();

	UFUNCTION(BlueprintImplementableEvent, Category = "Island Generation|Map")
	void DetermineBiomesEvent();

	UFUNCTION(BlueprintCallable, Category = "Island Generation|Map")
	void DetermineBiomes();

	UFUNCTION()
	void OnHeightmapFinished();
private:
	UPROPERTY()
	UPolygonalMapHeightmap* MapHeightmap;

	UPROPERTY()
	FRandomStream RandomGenerator;

    UPROPERTY()
	float MapStartGenerationTime;

    UPROPERTY()
	float CurrentGenerationTime;

	UPROPERTY()
	bool bCurrentStepIsDone;

    UPROPERTY()
	bool bHasGeneratedHeightmap;

	TQueue<FIslandGeneratorDelegate> IslandGeneratorSteps;

	FIslandGeneratorDelegate OnGenerationComplete;

    FIslandGeneratorDelegate OnHeightmapComplete;
public:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Island Generation")
	UPolygonMap* MapGraph;
 
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Island Generation")
	UIslandShape* IslandShape;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Island Generation")
	UPointGenerator* PointSelector;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Island Generation")
	DistributorOfElevations* ElevationDistributor;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Island Generation")
	DistributorOfMoistures* MoistureDistributor;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Island Generation")
	BiomesMgr* BiomeManager;
};