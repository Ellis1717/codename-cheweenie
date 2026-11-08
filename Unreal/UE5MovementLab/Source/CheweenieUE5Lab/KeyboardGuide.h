#pragma once
#include "CoreMinimal.h"
#include "InputCoreTypes.h"

class AHUD;
class UWorld;
struct FCozyKeyboardCap
{
    FKey Key;
    FString Label;
    float X,Y,Width,Height;
};
const TArray<FCozyKeyboardCap>& CozyKeyboardLayout();
TSet<FKey> CozyActiveKeyboardKeys(UWorld* World);
TArray<FString> CozyKeyboardControls(UWorld* World);
void CozyDrawKeyboard(AHUD* HUD,float X,float Y,float Scale=1.f);
