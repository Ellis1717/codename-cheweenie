#include "CozyCoop.h"
#include "CozyHUDStyle.h"
#include "CozyModeHub.h"
#include "Engine/GameInstance.h"
#include "Engine/Canvas.h"

void ACozyCoopHUD::DrawHub()
{
    auto* Hub=GetGameInstance()->GetSubsystem<UCozyModeHub>();
    const float X=FMath::Max(12.f,(Canvas->SizeX-680)*.5f),Y=FMath::Max(12.f,(Canvas->SizeY-480)*.5f);
    CozyDrawRect(this,FLinearColor(.015f,.03f,.045f,.64f),0,0,Canvas->SizeX,Canvas->SizeY);
    CozyDrawRect(this,FLinearColor(.035f,.09f,.12f,.96f),X,Y,680,480);
    CozyDrawText(this,Hub->Heading(),FLinearColor(1,.83f,.48f),X+24,Y+24,nullptr,1.5f);
    CozyDrawText(this,TEXT("Cozy Crew | Other players keep playing while your menu is open"),FLinearColor::White,X+24,Y+65);
    const auto Rows=Hub->Rows();const int32 Begin=FMath::Clamp(Hub->Selection-6,0,FMath::Max(0,Rows.Num()-7));
    for(int32 I=Begin;I<FMath::Min(Begin+7,Rows.Num());++I)
    {
        const float Row=Y+105+(I-Begin)*44;
        CozyDrawRect(this,I==Hub->Selection?FLinearColor(.2f,.38f,.42f):FLinearColor(.065f,.14f,.18f),X+18,Row,644,38);
        CozyDrawText(this,(I==Hub->Selection?TEXT(">  "):TEXT("   "))+Rows[I],FLinearColor::White,X+28,Row+10,nullptr,1.05f);
        AddHitBox(FVector2D(X+18,Row),FVector2D(644,38),FName(*FString::Printf(TEXT("HubRow_%d"),I)),true);
        if(Hub->CanAdjust(I)){AddHitBox(FVector2D(X+550,Row),FVector2D(50,38),FName(*FString::Printf(TEXT("HubMinus_%d"),I)),true,10);AddHitBox(FVector2D(X+605,Row),FVector2D(50,38),FName(*FString::Printf(TEXT("HubPlus_%d"),I)),true,10);CozyDrawText(this,TEXT("-       +"),FLinearColor::White,X+565,Row+10);}
    }
    CozyDrawText(this,TEXT("Arrows / D-pad | Enter / A-Cross select | B-Circle back"),FLinearColor(.75f,.85f,.86f),X+24,Y+435);
    CozyDrawText(this,Hub->Hint(),FLinearColor(1,.88f,.65f),20,Canvas->SizeY-40,nullptr,.95f);
}
void ACozyCoopHUD::NotifyHitBoxClick(FName Name)
{
    Super::NotifyHitBoxClick(Name);auto* PC=Cast<ACozyCoopController>(PlayerOwner);if(!PC||!PC->MenuOpen)return;
    auto* H=GetGameInstance()->GetSubsystem<UCozyModeHub>();const FString Value=Name.ToString();
    if(Value.StartsWith(TEXT("HubRow_"))){H->Selection=FCString::Atoi(*Value.Mid(7));H->Confirm();}
    else if(Value.StartsWith(TEXT("HubMinus_"))){H->Selection=FCString::Atoi(*Value.Mid(9));H->Adjust(-1);}
    else if(Value.StartsWith(TEXT("HubPlus_"))){H->Selection=FCString::Atoi(*Value.Mid(8));H->Adjust(1);}
}
