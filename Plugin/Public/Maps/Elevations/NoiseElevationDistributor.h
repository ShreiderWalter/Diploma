#pragma once

#include "Maps/Elevations/DistributorOfElevations.h"
#include "Noise/GeneratorNoise.h"
#include "SimplexNoiseElevationDistributor.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class POLYGONALMAPGENERATOR_API NoiseElevationDistributor : public DistributorOfElevations
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise")
	NoiseSettings NoiseSettings;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise")
	float NoiseScale = 0.0033f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
	float OceanThreshold = 0.05f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
	int32 GradientIterations = 1;

	NoiseElevationDistributor();

	UFUNCTION(BlueprintCallable, Category = "Island Generation|Map")
	virtual void AssignCornerElevations(UIslandShape* islandShape, bool bneedsMoreRandomness, FRandomStream& randomGenerator) override;


	UFUNCTION(BlueprintCallable, Category = "Island Generation|Map")
	virtual void RedistributeElevations(TArray<int32> landCorners) override;

	UFUNCTION(BlueprintPure, Category = "Island Generation|Map")
	virtual float GetGradient(float XCoord, float YCoord, int32 IslandSize) const;

};
