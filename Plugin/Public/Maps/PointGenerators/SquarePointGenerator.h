#pragma once

#include "PointGenerator.h"
#include "SquarePointGenerator.generated.h"

UCLASS(Blueprintable)
class POLYGONALMAPGENERATOR_API USquarePointGenerator : public UPointGenerator
{
	GENERATED_BODY()

public:
	USquarePointGenerator() { bNeedsMoreRandomness = true; };
	~USquarePointGenerator() {};

protected:
	virtual TArray<FVector2D> GeneratePoints_Implementation(int32 numberOfPoints) override;
};
