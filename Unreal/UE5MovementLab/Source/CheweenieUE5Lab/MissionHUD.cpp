#include "OpenMissions.h"
#include "CozyAdventure.h"
#include "MartyCharacter.h"
#include "CozyHUDStyle.h"
#include "Engine/Canvas.h"

bool CozyDrawMissionJournal(AMartyHUD* H,UCozyAdventure* S,float W,float V)
{
    const float Z=FMath::Min(W/1280.f,V/720.f),X=(W-1180*Z)*.5f,Y=(V-640*Z)*.5f;
    const FLinearColor Cream(1,.94f,.82f),Gold(1,.83f,.48f),Green(.60f,.91f,.67f),Muted(.73f,.78f,.79f);
    auto Text=[&](FString T,float A,float B,float Size=1.f,FLinearColor C=FLinearColor(1,.94f,.82f)){CozyMenuText(H,T,C,X+A*Z,Y+B*Z,Size*Z);};
    auto Wrap=[&](const FString& T,float A,float B,float Width,float Size,FLinearColor C)
    {
        TArray<FString> Words;T.ParseIntoArray(Words,TEXT(" "),true);FString Line;
        for(const auto& Word:Words){const FString Try=Line.IsEmpty()?Word:Line+TEXT(" ")+Word;float TW,TH;H->GetTextSize(Try,TW,TH,nullptr,Size*Z);if(TW>Width*Z&&!Line.IsEmpty()){Text(Line,A,B,Size,C);B+=23;Line=Word;}else Line=Try;}
        if(!Line.IsEmpty()){Text(Line,A,B,Size,C);B+=23;}return B;
    };
    H->DrawRect(FLinearColor(.015f,.023f,.027f,.96f),X,Y,1180*Z,640*Z);
    Text(TEXT("Missions & progress"),28,22,1.75f,Gold);
    Text(FString::Printf(TEXT("%d / %d completed  |  %d journal pawprints"),S->FinishedMissionCount(),CozyOpenMissions().Num(),S->FinishedMissionCount()),28,62,1.f,Green);
    Text(TEXT("Your day, your pace. Every mission is optional. Progress counts while unpinned."),28,92,.95f,Muted);
    const auto Rows=S->MissionRows();const int32 Visible=11,Begin=FMath::Clamp(S->Selection-Visible+1,0,FMath::Max(0,Rows.Num()-Visible));
    for(int32 I=Begin;I<FMath::Min(Begin+Visible,Rows.Num());++I)
    {
        const float B=135+(I-Begin)*38;CozyMenuRow(H,I==S->Selection,X+20*Z,Y+B*Z,470*Z,35*Z);
        const FString Label=(I==S->Selection?FString(TEXT("> ")):FString(TEXT("  ")))+Rows[I];float TW,TH;H->GetTextSize(Label,TW,TH,nullptr,Z);
        Text(Label,30,B+9,FMath::Min(1.f,440*Z/FMath::Max(TW,1.f)),I<CozyOpenMissions().Num()&&S->MissionPercent(CozyOpenMissions()[I].Id)==100?Green:Cream);
        H->AddHitBox(FVector2D(X+20*Z,Y+B*Z),FVector2D(470*Z,35*Z),FName(*FString::Printf(TEXT("CozyRow_%d"),I)),true);
    }
    H->DrawRect(FLinearColor(.09f,.11f,.11f),X+510*Z,Y+133*Z,2*Z,427*Z);
    if(CozyOpenMissions().IsValidIndex(S->Selection))
    {
        const auto& M=CozyOpenMissions()[S->Selection];const int32 Percent=S->MissionPercent(M.Id);
        float B=Wrap(M.Title,540,138,606,1.4f,Gold)+12;
        B=Wrap(M.Description,540,B,600,.98f,Cream)+16;
        Text(FString::Printf(TEXT("%d%% complete"),Percent),540,B,1.05f,Percent==100?Green:Gold);B+=30;
        H->DrawRect(FLinearColor(.16f,.20f,.19f),X+540*Z,Y+B*Z,580*Z,10*Z);H->DrawRect(Green,X+540*Z,Y+B*Z,580*Z*Percent/100.f,10*Z);B+=31;
        for(const auto& G:M.Goals){const int32 Have=FMath::Min(S->MissionValue(G.Key),G.Target);B=Wrap(FString::Printf(TEXT("%s %s  %d / %d"),Have==G.Target?TEXT("[Done]"):TEXT("[ ]"),*G.Label,Have,G.Target),540,B,600,.98f,Have==G.Target?Green:Cream)+8;}
        Text(Percent==100?TEXT("Journal pawprint earned. Free play continues."):TEXT("Reward: a permanent completion pawprint."),540,495,.95f,Green);
        Text(S->Progress->TrackedOpenMission==M.Id?TEXT("Enter / A-Cross: unpin this mission"):TEXT("Enter / A-Cross: pin this mission"),540,533,1.f,Gold);
    }
    else
    {
        Wrap(TEXT("Pin one mission for a compact tracker and a map destination when it has a place to visit. Unpin it whenever you feel like wandering."),540,150,590,1.15f,Cream);
        Text(TEXT("Completed missions and partial progress save automatically."),540,300,.95f,Green);
        Text(TEXT("New patrol restarts only the original patrol."),540,333,.95f,Muted);
    }
    Text(TEXT("J: journal | Arrows / D-pad: choose | Enter / A-Cross: select | Esc / B-Circle: back"),28,602,.92f,Muted);
    return true;
}
