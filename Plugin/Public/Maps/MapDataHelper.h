#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "PolygonMap.h"
#include "MapDataHelper.generated.h"


UCLASS(BlueprintType)
class POLYGONALMAPGENERATOR_API UMapDataHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Map Data|Tags")
	static bool HasTag(const FMapData& MapData, const FGameplayTag& Tag);

    UFUNCTION(BlueprintPure, Category = "Map Data|Tags")
	static bool IsBorder(const FMapData& MapData);

    UFUNCTION(BlueprintPure, Category = "Map Data|Tags")
	static bool IsFreshwater(const FMapData& MapData);

    UFUNCTION(BlueprintPure, Category = "Map Data|Tags")
	static bool IsWater(const FMapData& MapData);

    UFUNCTION(BlueprintPure, Category = "Map Data|Tags")
	static bool IsOcean(const FMapData& MapData);

    UFUNCTION(BlueprintPure, Category = "Map Data|Tags")
	static bool IsCoast(const FMapData& MapData);

    UFUNCTION(BlueprintPure, Category = "Map Data|Tags")
	static bool IsRiver(const FMapData& MapData);


    UFUNCTION(BlueprintCallable, Category = "Map Data|Tags")
	static FMapData SetMapTag(FMapData& MapData, const FGameplayTag& Tag);

    UFUNCTION(BlueprintCallable, Category = "Map Data|Tags")
	static FMapData SetBorder(FMapData& MapData);

    UFUNCTION(BlueprintCallable, Category = "Map Data|Tags")
	static FMapData SetFreshwater(FMapData& MapData);

    UFUNCTION(BlueprintCallable, Category = "Map Data|Tags")
	static FMapData SetOcean(FMapData& MapData);

    UFUNCTION(BlueprintCallable, Category = "Map Data|Tags")
	static FMapData SetCoast(FMapData& MapData);

    UFUNCTION(BlueprintCallable, Category = "Map Data|Tags")
	static FMapData SetRiver(FMapData& MapData);

    UFUNCTION(BlueprintCallable, Category = "Map Data|Tags")
	static FMapData RemoveMapTag(FMapData& MapData, const FGameplayTag& Tag);

    UFUNCTION(BlueprintCallable, Category = "Map Data|Tags")
	static FMapData RemoveBorder(FMapData& MapData);

    UFUNCTION(BlueprintCallable, Category = "Map Data|Tags")
	static FMapData RemoveFreshwater(FMapData& MapData);

    UFUNCTION(BlueprintCallable, Category = "Map Data|Tags")
	static FMapData RemoveOcean(FMapData& MapData);

    UFUNCTION(BlueprintCallable, Category = "Map Data|Tags")
	static FMapData RemoveCoast(FMapData& MapData);

    UFUNCTION(BlueprintCallable, Category = "Map Data|Tags")
	static FMapData RemoveRiver(FMapData& MapData);
};