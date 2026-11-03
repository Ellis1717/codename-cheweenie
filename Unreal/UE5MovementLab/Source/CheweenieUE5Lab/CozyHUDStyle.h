#pragma once
#include "CoreMinimal.h"
class AHUD;
class UFont;

// Shared Canvas presentation for solo, navigation, and LAN; no gameplay state changes.
void CozyDrawText(AHUD* HUD,const FString& Text,FLinearColor Color,float X,float Y,UFont* Font=nullptr,float Scale=1.f,bool ScalePosition=false);
void CozyDrawRect(AHUD* HUD,FLinearColor Color,float X,float Y,float Width,float Height);
void CozyDrawLine(AHUD* HUD,float X1,float Y1,float X2,float Y2,FLinearColor Color,float Thickness=0.f);
