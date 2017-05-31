#pragma once

#include "PointGenerator.h"
#include "RandomPointGenerator.generated.h"

UCLASS(Blueprintable)
class POLYGONALMAPGENERATOR_API URandomPointGenerator : public UPointGenerator
{
	GENERATED_BODY()

public:
	URandomPointGenerator() { bNeedsMoreRandomness = false; };
	~URandomPointGenerator() {};

protected:
	virtual TArray<FVector2D> GeneratePoints_Implementation(int32 numberOfPoints) override;
};
