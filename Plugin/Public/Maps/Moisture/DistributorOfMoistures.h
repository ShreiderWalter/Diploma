#pragma once

#include "Components/ActorComponent.h"
#include "PolygonMap.h"
#include "River.h"
#include "MoistureDistributor.generated.h"

UCLASS(Blueprintable)
class POLYGONALMAPGENERATOR_API DistributorOfMoistures : public UActorComponent
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lakes", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float LakeThreshold = 0.9f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rivers")
	int32 RiverCount = 50;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rivers", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MinRiverStartElevation = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rivers", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MaxRiverStartElevation = 0.9f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rivers", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float RiverDownstreamBias = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rivers", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float StandingWaterBias = 0.75f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rivers", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float RiverJoinBias = 0.825f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rivers", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float RiverLakeConversionFactor = 0.85f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rivers")
	uint8 MinRiverSectionCount = 5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rivers")
	uint8 StartingRiverSize = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rivers")
	uint8 MaxRiverSize = 4;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Rivers")
	TArray<URiver*> Rivers;

	UFUNCTION(BlueprintCallable, Category = "Island Generation|Map")
	void SetGraph(UPolygonMap* Graph, int32 Size);


	UFUNCTION(BlueprintCallable, Category = "Island Generation|Map")
	virtual void AssignOceanCoastAndLand();

	UFUNCTION(BlueprintCallable, Category = "Island Generation|Map")
	void CalculateWatersheds();
	UFUNCTION(BlueprintCallable, Category = "Island Generation|Map")
	void CreateRivers(FRandomStream& RandomGenerator);
	UFUNCTION(BlueprintCallable, Category = "Island Generation|Map")
	void AssignCornerMoisture();
	UFUNCTION(BlueprintCallable, Category = "Island Generation|Map")
	void RedistributeMoisture(TArray<int32> LandCorners);
	UFUNCTION(BlueprintCallable, Category = "Island Generation|Map")
	void AssignPolygonMoisture();

protected:
	UPolygonMap* MapGraph;
	int32 MapSize;
};
