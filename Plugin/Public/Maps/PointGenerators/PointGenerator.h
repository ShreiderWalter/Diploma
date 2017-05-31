#pragma once

#include "Components/ActorComponent.h"
#include "PointGenerator.generated.h"
using namespace UP;

UCLASS(Blueprintable)
class POLYGONALMAPGENERATOR_API UPointGenerator : public UActorComponent
{
	GENERATED_BODY()

public:
	UPointGenerator() {};
	~UPointGenerator() {};

	UFUNCTION(BlueprintCallable, Category = "Island Generation|Graph|Points")
		void InitializeSelector(int32 mapSize, int32 seed, int32 border = 10);

	UFUNCTION(BlueprintNativeEvent, Category = "Island Generation|Graph|Points")
		TArray<FVector2D> GeneratePoints(int32 numberOfPoints);
	UFUNCTION(BlueprintPure, Category = "Island Generation|Graph|Points")
		bool NeedsMoreRandomness();

	UFUNCTION(BlueprintPure, Category = "Island Generation|Graph|Points")
		bool PointIsOnBorder(FVector2D point);
protected:
	UPROPERTY(Category = "Island", BlueprintReadWrite, EditAnywhere)
		int32 MapSize;
	UPROPERTY(Category = "Island", BlueprintReadWrite, EditAnywhere)
		FRandomStream RandomGenerator;
	UPROPERTY(Category = "Points", BlueprintReadWrite, EditAnywhere)
		int32 Border;
	UPROPERTY(Category = "Points", BlueprintReadWrite, EditAnywhere)
		bool bNeedsMoreRandomness;

	virtual TArray<FVector2D> GeneratePoints_Implementation(int32 numberOfPoints);
};