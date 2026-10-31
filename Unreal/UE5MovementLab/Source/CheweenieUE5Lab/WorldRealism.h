#pragma once
#include "CoreMinimal.h"
class UWorld;
// Cosmetic-only overlays. Tested collision, traversal and private layouts are retained.
void UpgradeCheweenieRoom(UWorld* World);
void UpgradeCheweenieNeighbourhood(UWorld* World,const FVector& Park);
