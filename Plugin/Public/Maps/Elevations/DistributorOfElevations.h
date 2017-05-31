#pragma once

#include "Components/ActorComponent.h"
#include "PolygonMap.h"
#include "ElevationDistributor.generated.h"

UCLASS(Blueprintable)
class POLYGONALMAPGENERATOR_API DistributorOfElevations : public UActorComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Island Generation|Graph")
	void SetGraph(UPolygonMap* graph);

	UFUNCTION(BlueprintCallable, Category = "Island Generation|Map")
	virtual void AssignCornerElevations(UIslandShape* islandShape, bool bneedsMoreRandomness, FRandomStream& randomGenerator);

	UFUNCTION(BlueprintCallable, Category = "Island Generation|Map")
	virtual void CalculateDownslopes();

	UFUNCTION(BlueprintCallable, Category = "Island Generation|Map")
	virtual void RedistributeElevations(TArray<int32> landCorners);

	UFUNCTION(BlueprintCallable, Category = "Island Generation|Map")
	virtual void FlattenWaterElevations();

	UFUNCTION(BlueprintCallable, Category = "Island Generation|Map")
	virtual void AssignPolygonElevations();

protected:
	UPolygonMap* MapGraph;
};