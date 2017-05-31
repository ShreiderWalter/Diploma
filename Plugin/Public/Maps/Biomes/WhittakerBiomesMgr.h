#pragma once

#include "Maps/Biomes/BiomesMgr.h"
#include "WhittakerBiomeManager.generated.h"

UENUM(BlueprintType)
enum class EWhittakerBiome : uint8
{
	Ocean						UMETA(DisplayName = "Ocean"),
	Ice							UMETA(DisplayName = "Ice"),
	Marsh						UMETA(DisplayName = "Marsh"),
	Lake						UMETA(DisplayName = "Lake"),
	Coast						UMETA(DisplayName = "Coast"),
	Snow						UMETA(DisplayName = "Snow"),
	Tundra						UMETA(DisplayName = "Tundra"),
	Bare						UMETA(DisplayName = "Bare"),
	Scorched					UMETA(DisplayName = "Scorched"),
	Taiga						UMETA(DisplayName = "Taiga"),
	Shrubland					UMETA(DisplayName = "Shrubland"),
	TemperateDesert				UMETA(DisplayName = "Temperate Desert"),
	TemperateRainForest			UMETA(DisplayName = "Temperate Rain Forest"),
	TemperateDeciduousForest	UMETA(DisplayName = "Temperate Deciduous Forest"),
	Grassland					UMETA(DisplayName = "Grassland"),
	TropicalRainForest			UMETA(DisplayName = "Tropical Rain Forest"),
	TropicalSeasonalForest		UMETA(DisplayName = "Tropical Seasonal Forest"),
	SubtropicalDesert			UMETA(DisplayName = "Subtropical Desert")
};

UCLASS(Blueprintable)
class POLYGONALMAPGENERATOR_API WhittakerBiomeaMgr : public BiomesMgr
{
	GENERATED_BODY()
protected:
	static const FName BIOME_IceTag;
	static const FName BIOME_MarshTag;
	static const FName BIOME_LakeTag;
	static const FName BIOME_SnowTag;
	static const FName BIOME_TundraTag;
	static const FName BIOME_BareTag;
	static const FName BIOME_ScorchedTag;
	static const FName BIOME_TaigaTag;
	static const FName BIOME_ShrublandTag;
	static const FName BIOME_TemperateDesertTag;
	static const FName BIOME_TemperateRainForestTag;
	static const FName BIOME_TemperateDeciduousForestTag;
	static const FName BIOME_GrasslandTag;
	static const FName BIOME_TropicalRainForestTag;
	static const FName BIOME_TropicalSeasonalForestTag;
	static const FName BIOME_SubtropicalDesertTag;

public:
	WhittakerBiomeaMgr();

	UFUNCTION(BlueprintPure, Category = "Biome")
	EWhittakerBiome ConvertToWhittakerBiomeEnum(const FGameplayTag& BiomeName);

protected:
	TMap<FGameplayTag, EWhittakerBiome> BiomeEnumMap;
};
