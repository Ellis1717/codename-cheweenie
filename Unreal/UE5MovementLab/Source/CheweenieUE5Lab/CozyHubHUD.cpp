#include "CozyCoop.h"
#include "CozyHUDStyle.h"
#include "CozyModeHub.h"
#include "Engine/GameInstance.h"
#include "Engine/Canvas.h"

void ACozyCoopHUD::DrawHub()
{
    auto* Hub=GetGameInstance()->GetSubsystem<UCozyModeHub>();
    const float W=Canvas->SizeX,H=Canvas->SizeY,S=FMath::Min(W/1280.f,H/720.f);
    if(Hub->Page==ECozyHubPage::Biography){CozyDrawBiography(this,Hub,W,H,TEXT("HubRow_"));return;}
    const float PW=FMath::Min(680*S,W*.55f),X=W*.045f,Y=FMath::Max(12.f,(H-480*S)*.5f);
    DrawRect(FLinearColor(.012f,.018f,.025f,.18f),0,0,W,H);
    CozyMenuShade(this,0,0,FMath::Max(W*.60f,X+PW+40*S),H,.79f);
    CozyMenuText(this,Hub->Heading(),FLinearColor(1,.85f,.57f),X+24*S,Y+24*S,1.5f*S);
    CozyMenuText(this,TEXT("Cozy Crew | Friends keep playing while this menu is open"),FLinearColor(.94f,.89f,.79f),X+24*S,Y+65*S,.9f*S);
    const auto Rows=Hub->Rows();const int32 Begin=FMath::Clamp(Hub->Selection-6,0,FMath::Max(0,Rows.Num()-7));
    for(int32 I=Begin;I<FMath::Min(Begin+7,Rows.Num());++I)
    {
        const float Row=Y+(105+(I-Begin)*44)*S;
        CozyMenuRow(this,I==Hub->Selection,X+18*S,Row,PW-36*S,38*S);
        const FString Label=(I==Hub->Selection?TEXT(">  "):TEXT("   "))+Rows[I];
        float TW=0,TH=0;GetTextSize(Label,TW,TH,nullptr,1.05f*S);
        const float Available=PW-(Hub->CanAdjust(I)?168:64)*S;
        CozyMenuText(this,Label,FLinearColor(1,.91f,.75f),X+28*S,Row+10*S,TW>Available?1.05f*S*Available/TW:1.05f*S);
        AddHitBox(FVector2D(X+18*S,Row),FVector2D(PW-36*S,38*S),FName(*FString::Printf(TEXT("HubRow_%d"),I)),true);
        if(Hub->CanAdjust(I))
        {
            const float AX=X+PW-126*S;
            AddHitBox(FVector2D(AX,Row),FVector2D(50*S,38*S),FName(*FString::Printf(TEXT("HubMinus_%d"),I)),true,10);
            AddHitBox(FVector2D(AX+55*S,Row),FVector2D(50*S,38*S),FName(*FString::Printf(TEXT("HubPlus_%d"),I)),true,10);
            CozyMenuText(this,TEXT("-       +"),FLinearColor(1,.85f,.57f),AX+15*S,Row+10*S,S);
        }
    }
    CozyMenuText(this,TEXT("Arrows / D-pad | Enter / A-Cross select | B-Circle back"),FLinearColor(.84f,.79f,.69f),X+24*S,Y+435*S,.9f*S);
    CozyMenuText(this,Hub->Hint(),FLinearColor(.91f,.85f,.72f),20*S,H-40*S,.85f*S);
}
void ACozyCoopHUD::NotifyHitBoxClick(FName Name)
{
    Super::NotifyHitBoxClick(Name);auto* PC=Cast<ACozyCoopController>(PlayerOwner);if(!PC||!PC->MenuOpen)return;
    auto* H=GetGameInstance()->GetSubsystem<UCozyModeHub>();const FString Value=Name.ToString();
    if(Value.StartsWith(TEXT("HubRow_"))){H->Selection=FCString::Atoi(*Value.Mid(7));H->Confirm();}
    else if(Value.StartsWith(TEXT("HubMinus_"))){H->Selection=FCString::Atoi(*Value.Mid(9));H->Adjust(-1);}
    else if(Value.StartsWith(TEXT("HubPlus_"))){H->Selection=FCString::Atoi(*Value.Mid(8));H->Adjust(1);}
}
