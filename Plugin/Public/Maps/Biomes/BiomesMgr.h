#pragma once

#include "Components/ActorComponent.h"
#include "PolygonMap.h"
#include "GameplayTagContainer.h"
#include "BiomeManager.generated.h"

USTRUCT(BlueprintType)
struct FBiome
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Map Biome", meta = (Categories = "MapData.Biome"))
	FGameplayTag BiomeTag;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Elevation", meta = (ClampMin = "0.0", ClampMax = "1.0"))
        float MinElevation;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Elevation", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MaxElevation;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Moisture", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MinMoisture;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Moisture", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MaxMoisture;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Map Biome", meta = (Categories = "MapData.MetaData"))
	FGameplayTagContainer RequiredTags;

	FBiome()
	{
		MinElevation = 0.0f;
		MaxElevation = 1.0f;

		MinMoisture = 0.0f;
		MaxMoisture = 1.0f;
	}
};

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class POLYGONALMAPGENERATOR_API BiomesMgr : public UActorComponent
{
	GENERATED_BODY()
protected:
	virtual FGameplayTag DetermineBiome_Implementation(const FMapData& MapData);

public:	
	BiomesMgr();

	static const FName BIOME_OceanTag;
	static const FName BIOME_CoastTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Biome")
	TArray<FBiome> Biomes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Biome")
	bool bAllowSnow = true;
	
	UFUNCTION(BlueprintNativeEvent, Category="Biome")
	FGameplayTag DetermineBiome(const FMapData& MapData);
};
