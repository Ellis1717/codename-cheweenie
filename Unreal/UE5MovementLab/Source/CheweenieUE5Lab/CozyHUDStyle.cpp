#include "CozyHUDStyle.h"
#include "ControllerGuide.h"
#include "CozyModeHub.h"
#include "GameFramework/HUD.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"

struct FCozyPalette
{
    FLinearColor Panel,Selected,Ink,Accent,Muted,Success,Danger,Outline;
    float Opacity;
    bool Border;
};
static int32 Style(AHUD* HUD)
{
    auto* GI=HUD&&HUD->GetWorld()?HUD->GetWorld()->GetGameInstance():nullptr;
    const auto* Hub=GI?GI->GetSubsystem<UCozyModeHub>():nullptr;return Hub?Hub->OverlayStyle():0;
}
static FCozyPalette Palette(int32 Id)
{
    if(Id==1)return {FLinearColor(.94f,.87f,.72f),FLinearColor(.79f,.67f,.47f),FLinearColor(.12f,.075f,.035f),FLinearColor(.43f,.14f,.025f),FLinearColor(.25f,.19f,.12f),FLinearColor(.06f,.27f,.13f),FLinearColor(.55f,.045f,.035f),FLinearColor(.98f,.94f,.84f),1.f,true};
    if(Id==2)return {FLinearColor(.045f,.025f,.12f),FLinearColor(.25f,.08f,.34f),FLinearColor(.97f,.95f,1),FLinearColor(1,.42f,.76f),FLinearColor(.73f,.72f,.91f),FLinearColor(.4f,1,.77f),FLinearColor(1,.42f,.38f),FLinearColor(.015f,.008f,.035f),1.f,true};
    return {FLinearColor(.025f,.035f,.045f),FLinearColor(.12f,.20f,.25f),FLinearColor(.95f,.98f,1),FLinearColor(.35f,.86f,1),FLinearColor(.73f,.82f,.87f),FLinearColor(.52f,.94f,.66f),FLinearColor(1,.43f,.4f),FLinearColor(.005f,.01f,.015f),.86f,false};
}
void CozyDrawText(AHUD* HUD,const FString& SourceText,FLinearColor Color,float X,float Y,UFont* Font,float Scale,bool ScalePosition)
{
    const auto* GI=HUD&&HUD->GetWorld()?HUD->GetWorld()->GetGameInstance():nullptr;
    const auto* Guide=GI?GI->GetSubsystem<UControllerGuide>():nullptr;
    const FString Text=Guide?Guide->Format(SourceText):SourceText;
    const int32 Id=Style(HUD);if(!Id){HUD->DrawText(Text,Color,X,Y,Font,Scale,ScalePosition);return;}
    const auto P=Palette(Id);const float Alpha=Color.A;
    if(Color.R>Color.G*1.3f&&Color.R>Color.B*1.3f)Color=P.Danger;
    else if(Color.G>Color.R*1.2f&&Color.G>Color.B*1.1f)Color=P.Success;
    else if(Color.R>.8f&&Color.G>.5f&&Color.B<.75f)Color=P.Accent;
    else Color=FMath::Max3(Color.R,Color.G,Color.B)<.9f?P.Muted:P.Ink;
    Color.A=Alpha;
    // Light outline keeps Paper's dark ink readable on scenery outside panels.
    if(Id==1)for(const FVector2D Offset:{FVector2D(-1,0),FVector2D(1,0),FVector2D(0,-1),FVector2D(0,1)})HUD->DrawText(Text,P.Outline,X+Offset.X,Y+Offset.Y,Font,Scale,ScalePosition);
    else HUD->DrawText(Text,P.Outline,X+1,Y+1,Font,Scale,ScalePosition);
    HUD->DrawText(Text,Color,X,Y,Font,Scale,ScalePosition);
}
void CozyDrawRect(AHUD* HUD,FLinearColor Color,float X,float Y,float Width,float Height)
{
    const int32 Id=Style(HUD);
    if(Id==1&&Width==6&&Height==6){Color=Color.R>.8f?Palette(Id).Accent:FLinearColor(.04f,.18f,.43f);HUD->DrawRect(Color,X,Y,Width,Height);return;}
    // Keep health/energy fills, map symbols, stamp pips, and full-screen dimmers semantic.
    const bool Panel=Id&&Height>18&&Height<600&&Color.A>.7f&&FMath::Max3(Color.R,Color.G,Color.B)<.6f;
    if(!Panel){HUD->DrawRect(Color,X,Y,Width,Height);return;}
    const auto P=Palette(Id);const float Alpha=Color.A*P.Opacity;
    Color=Color.G>.25f||Color.R>.3f?P.Selected:P.Panel;Color.A=Alpha;
    HUD->DrawRect(Color,X,Y,Width,Height);
    if(P.Border){auto Edge=P.Accent;Edge.A=.8f;HUD->DrawRect(Edge,X,Y,Width,2);HUD->DrawRect(Edge,X,Y,2,Height);}
}
void CozyDrawLine(AHUD* HUD,float X1,float Y1,float X2,float Y2,FLinearColor Color,float Thickness)
{
    const int32 Id=Style(HUD);
    if(Id)
    {
        const auto P=Palette(Id);
        if(Color.R>.8f&&Color.G>.5f&&Color.B<.75f)Color=P.Accent;
        else if(Color.G>Color.R*1.2f&&Color.G>Color.B*1.1f)Color=P.Success;
        else if(Color.B>Color.R*1.3f&&Color.B>Color.G*.9f)Color=Id==1?FLinearColor(.04f,.18f,.43f):FLinearColor(.3f,.7f,1);
        else Color=P.Muted;
    }
    HUD->DrawLine(X1,Y1,X2,Y2,Color,Thickness);
}
