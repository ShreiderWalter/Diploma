#include "PolygonalMapGeneratorPrivatePCH.h"
#include "Maps/MapDataHelper.h"
#include "BiomeManager.h"

const FName BiomesMgr::BIOME_OceanTag = TEXT("MapData.Biome.Water.Ocean");
const FName BiomesMgr::BIOME_CoastTag = TEXT("MapData.Biome.Coast");

BiomesMgr::BiomesMgr()
{
	PrimaryComponentTick.bCanEverTick = false;
	FBiome oceanBiome;
	oceanBiome.BiomeTag = FGameplayTag::RequestGameplayTag(BIOME_OceanTag);
	oceanBiome.RequiredTags.AddTagFast(FGameplayTag::RequestGameplayTag(TEXT("MapData.MetaData.Water.Saltwater")));
	Biomes.Add(oceanBiome);

	FBiome coastalBiome;
	coastalBiome.BiomeTag = FGameplayTag::RequestGameplayTag(BIOME_CoastTag);
	coastalBiome.RequiredTags.AddTagFast(FGameplayTag::RequestGameplayTag(TEXT("MapData.MetaData.Coast")));
	Biomes.Add(coastalBiome);
}

FGameplayTag BiomesMgr::DetermineBiome_Implementation(const FMapData& MapData)
{
	for (int i = 0; i < Biomes.Num(); i++)
	{
		if (!bAllowSnow && Biomes[i].BiomeTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("MapData.Biome.Frozen"))))
		{
			continue;
		}

		if (MapData.Elevation > Biomes[i].MaxElevation || MapData.Elevation < Biomes[i].MinElevation ||
			MapData.Moisture > Biomes[i].MaxMoisture || MapData.Moisture < Biomes[i].MinMoisture)
		{
			continue;
		}

		bool bHasRequiredTags = Biomes[i].RequiredTags.Num() == 0;

		if(!bHasRequiredTags)
		{
			bHasRequiredTags = true;
			for (int j = 0; j < Biomes[i].RequiredTags.Num(); j++)
			{
				if (!UMapDataHelper::HasTag(MapData, Biomes[i].RequiredTags.GetByIndex(j)))
				{
					bHasRequiredTags = false;
					break;
				}
			}
		}
		if (bHasRequiredTags)
		{
			return Biomes[i].BiomeTag;
		}
	}
	UE_LOG(LogWorldGen, Warning, TEXT("A MapData point was missing a biome! Point: (%f, %f); Elevation: %f; Moisture: %f"), MapData.Point.X, MapData.Point.Y, MapData.Elevation, MapData.Moisture);
	return FGameplayTag::EmptyTag;
}