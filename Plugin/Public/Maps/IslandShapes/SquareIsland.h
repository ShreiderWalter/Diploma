#pragma once

#include "IslandShape.h"
#include "SquareIsland.generated.h"

UCLASS(Blueprintable)
class POLYGONALMAPGENERATOR_API USquareIsland : public UIslandShape
{
	GENERATED_BODY()

protected:
	virtual bool IsPointLand_Implementation(FVector2D point) override;
};
