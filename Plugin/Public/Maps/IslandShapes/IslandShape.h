#pragma once

#include "Components/ActorComponent.h"
#include "IslandShape.generated.h"

using namespace UP;

UCLASS(Blueprintable)
class POLYGONALMAPGENERATOR_API UIslandShape : public UActorComponent
{
	GENERATED_BODY()
public:
	UIslandShape() {};
	~UIslandShape() {};

	UFUNCTION(BlueprintNativeEvent, Category = "Island Generation|Graph|Points")
	void SetSeed(int32 seed, int32 size);

	UFUNCTION(BlueprintNativeEvent, Category = "Island Generation|Graph|Points")
	bool IsPointLand(FVector2D point);

	UPROPERTY(Category = "Island Size", BlueprintReadWrite, EditAnywhere)
	float IslandFactor = 1.07f;

	UPROPERTY(Category = "Island Size", BlueprintReadWrite, EditAnywhere)
		int32 Size;

protected:
	virtual void SetSeed_Implementation(int32 seed, int32 size);
	virtual bool IsPointLand_Implementation(FVector2D point);

	UPROPERTY(Category = "Island Randomness", BlueprintReadWrite, EditAnywhere)
	FRandomStream StreamRNG;
};