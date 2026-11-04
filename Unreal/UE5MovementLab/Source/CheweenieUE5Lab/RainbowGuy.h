#pragma once
#include "CoreMinimal.h"

class AStaticMeshActor;
class UWorld;

// Stable save indices: append new toys without renumbering existing treasures.
constexpr int32 CheweenieToyCount = 3;
constexpr int32 RainbowGuyItem = 2;
inline const TCHAR* CheweenieToyName(int32 Item)
{
    return Item == 0 ? TEXT("tennis ball") : Item == 1 ? TEXT("treasure pebble") : Item == RainbowGuyItem ? TEXT("Rainbow Guy") : TEXT("nothing");
}
AStaticMeshActor* SpawnRainbowGuy(UWorld* World, FVector Location);
