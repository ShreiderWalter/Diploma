#pragma once

#include "IslandShape.h"
#include "RadialIsland.generated.h"

UCLASS(Blueprintable)
class POLYGONALMAPGENERATOR_API URadialIsland : public UIslandShape
{
	GENERATED_BODY()

public:
	UPROPERTY(Category = "Bumps", BlueprintReadWrite, EditAnywhere)
	int32 BumpsMin = 1;
	UPROPERTY(Category = "Bumps", BlueprintReadWrite, EditAnywhere)
	int32 BumpsMax = 6;

	UPROPERTY(Category = "Angle", BlueprintReadWrite, EditAnywhere)
	float StartAngleMin = 0.0f;
	UPROPERTY(Category = "Angle", BlueprintReadWrite, EditAnywhere)
	float StartAngleMax = 2.0f;

	UPROPERTY(Category = "Angle", BlueprintReadWrite, EditAnywhere)
	float AngleOffsetMin = 0.0f;
	UPROPERTY(Category = "Angle", BlueprintReadWrite, EditAnywhere)
	float AngleOffsetMax = 2.0f;

	UPROPERTY(Category = "Angle", BlueprintReadWrite, EditAnywhere)
	float MinAngleMin = 0.2f;
	UPROPERTY(Category = "Angle", BlueprintReadWrite, EditAnywhere)
	float MinAngleMax = 0.7f;

protected:
	virtual void SetSeed_Implementation(int32 seed, int32 size) override;
	virtual bool IsPointLand_Implementation(FVector2D point) override;

	UPROPERTY(Category = "Bumps", BlueprintReadWrite, VisibleAnywhere)
	int32 Bumps;
	UPROPERTY(Category = "Angle", BlueprintReadWrite, VisibleAnywhere)
	float StartAngle;
	UPROPERTY(Category = "Angle", BlueprintReadWrite, VisibleAnywhere)
	float AngleOffset;
	UPROPERTY(Category = "Angle", BlueprintReadWrite, VisibleAnywhere)
	float MinAngle;
};
