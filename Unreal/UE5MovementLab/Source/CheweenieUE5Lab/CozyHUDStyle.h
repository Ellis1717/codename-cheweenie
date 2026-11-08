#pragma once
#include "CoreMinimal.h"
class AHUD;
class UFont;

// Shared Canvas presentation for solo, navigation, and LAN; no gameplay state changes.
void CozyDrawText(AHUD* HUD,const FString& Text,FLinearColor Color,float X,float Y,UFont* Font=nullptr,float Scale=1.f,bool ScalePosition=false);
void CozyDrawRect(AHUD* HUD,FLinearColor Color,float X,float Y,float Width,float Height);
void CozyDrawLine(AHUD* HUD,float X1,float Y1,float X2,float Y2,FLinearColor Color,float Thickness=0.f);
// Menu ink stays warm and legible independently of gameplay overlay palettes.
void CozyMenuText(AHUD* HUD,const FString& Text,FLinearColor Color,float X,float Y,float Scale=1.f);
void CozyMenuShade(AHUD* HUD,float X,float Y,float Width,float Height,float Opacity=.65f);
void CozyMenuRow(AHUD* HUD,bool Selected,float X,float Y,float Width,float Height=38.f);
void CozyDrawBiography(AHUD* HUD,class UCozyModeHub* Hub,float Width,float Height,const FString& HitPrefix);
