#pragma once
#include "CoreMinimal.h"
class UCozyAdventure;
class AMartyHUD;
struct FOpenMissionGoal { FName Key; FString Label; int32 Target=1; };
struct FOpenMission { FName Id; FString Title,Description; TArray<FOpenMissionGoal> Goals; };
const TArray<FOpenMission>& CozyOpenMissions();
const FOpenMission* CozyFindMission(FName Id);
bool CozyDrawMissionJournal(AMartyHUD* HUD,UCozyAdventure* S,float Width,float Height);
