#include "CozyHUDStyle.h"
#include "CozyModeHub.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "GameFramework/HUD.h"

void UCozyModeHub::OpenCrew(){Status.Empty();const int32 Previous=CrewIndex;for(CrewIndex=0;CrewIndex<3;++CrewIndex)CrewPortrait();CrewIndex=Previous;NewPage(ECozyHubPage::Crew);MenuBackdrop();}
UTexture2D* UCozyModeHub::MenuBackdrop()
{
    const TCHAR* Name=Page==ECozyHubPage::Crew?TEXT("SofaCuddle"):Page==ECozyHubPage::Archives?TEXT("BedtimePile"):Page==ECozyHubPage::Host||Page==ECozyHubPage::Help?TEXT("FortPeek"):TEXT("RainyWindow");
    const FString Path=FString::Printf(TEXT("/Game/Adventure/Crew/%s.%s"),Name,Name);
    if(!MenuArtwork.Contains(Path))MenuArtwork.Add(Path,LoadObject<UTexture2D>(nullptr,*Path));
    return MenuArtwork[Path];
}
UTexture2D* UCozyModeHub::CrewPortrait()
{
    const TCHAR* Names[]={TEXT("MartyPortrait"),TEXT("LakituPortrait"),TEXT("RioPortrait")};
    const auto* Name=Names[FMath::Clamp(CrewIndex,0,2)];
    const FString Path=CrewIndex==0?FString(TEXT("/Game/Adventure/Art/Cover.Cover")):FString::Printf(TEXT("/Game/Adventure/Crew/%s.%s"),Name,Name);
    if(!MenuArtwork.Contains(Path))MenuArtwork.Add(Path,LoadObject<UTexture2D>(nullptr,*Path));
    return MenuArtwork[Path];
}

void CozyDrawBiography(AHUD* HUD,UCozyModeHub* Hub,float Width,float Height,const FString& HitPrefix)
{
    const float S=FMath::Min(Width/1280.f,Height/720.f);
    const float X=Width*.055f,Y=Height*.13f;
    const TCHAR* Names[]={TEXT("Marty"),TEXT("Lakitu"),TEXT("Rio")};
    const TCHAR* Aliases[]={TEXT("Mr. Marty / Marty Man / Marty Party / Marty McFly"),TEXT("Keetz / Keeterman"),TEXT("Sis / Little Miss / Miss Priss")};
    const TArray<FString> Stories[]={
        {TEXT("A little Chiweenie named after Martin St-Louis."),TEXT("Cheween in Between: squeezing into a cuddle."),TEXT("Cheeween Cozy: snuggling under the blankets."),TEXT("Favourite toy: Rainbow Guy, the squeaky rainbow monster."),TEXT("In the game: sniff out snacks, carry toys, dig, learn tricks and explore with the cats.")},
        {TEXT("One of Marty's two cat companions."),TEXT("Look for the mostly white face, black ears and the black patch over his left eye."),TEXT("A pink nose, white chest and a little collar tag complete his look."),TEXT("In the game: ask him to follow or stay, call him over and share a head bump."),TEXT("Try Lakitu's Hockey Bag Surprise from the main menu.")},
        {TEXT("Marty's other cat companion, also known as Little Miss."),TEXT("Look for the black face, small white chin, white chest bib and long white whiskers."),TEXT("Rio and Lakitu have their own follow choices and friendship progress."),TEXT("In the game: explore together, ask her to stay or call her back for a head bump."),TEXT("At home, there is always room for another cozy companion.")}
    };
    const int32 Index=FMath::Clamp(Hub->CrewIndex,0,2);
    if(auto* Art=Hub->CrewPortrait())
    {
        const float Size=FMath::Min(Height,Width*.56f);
        HUD->DrawTexture(Art,Width-Size,(Height-Size)*.5f,Size,Size,Index==0?.53f:0.f,Index==0?.40f:0.f,Index==0?.26f:1.f,Index==0?.46f:1.f);
        HUD->DrawRect(FLinearColor(.02f,.016f,.012f),0,0,Width-Size,Height);
        CozyMenuShade(HUD,Width-Size,0,Width*.18f,Height,1.f);
    }
    CozyMenuShade(HUD,0,0,Width*.58f,Height,.92f);
    CozyMenuText(HUD,TEXT("CHARACTER BIOS"),FLinearColor(.89f,.72f,.43f),X,Y,.95f*S);
    CozyMenuText(HUD,Names[Index],FLinearColor(1,.88f,.64f),X,Y+36*S,2.6f*S);
    CozyMenuText(HUD,Aliases[Index],FLinearColor(.9f,.85f,.75f),X,Y+91*S,.85f*S);
    float RowY=Y+143*S;
    const float TextWidth=Width*.40f;
    for(const auto& Paragraph:Stories[Index])
    {
        TArray<FString> Words;Paragraph.ParseIntoArray(Words,TEXT(" "),true);FString Line;
        for(const auto& Word:Words)
        {
            const FString Next=Line.IsEmpty()?Word:Line+TEXT(" ")+Word;float TW=0,TH=0;
            HUD->GetTextSize(Next,TW,TH,nullptr,1.03f*S);
            if(TW>TextWidth&&!Line.IsEmpty()){CozyMenuText(HUD,Line,FLinearColor(.98f,.94f,.85f),X,RowY,1.03f*S);RowY+=23*S;Line=Word;}else Line=Next;
        }
        CozyMenuText(HUD,Line,FLinearColor(.98f,.94f,.85f),X,RowY,1.03f*S);RowY+=39*S;
    }
    const auto Rows=Hub->Rows();
    for(int32 I=0;I<Rows.Num();++I)
    {
        const float RY=Height-142*S+I*33*S;
        CozyMenuRow(HUD,I==Hub->Selection,X-8*S,RY,Width*.37f,30*S);
        CozyMenuText(HUD,Rows[I],FLinearColor(1,.89f,.68f),X+8*S,RY+7*S,.95f*S);
        HUD->AddHitBox(FVector2D(X-8*S,RY),FVector2D(Width*.37f,30*S),FName(*(HitPrefix+FString::FromInt(I))),true);
    }
    CozyMenuText(HUD,TEXT("Arrows / D-pad choose | Enter / A-Cross select | B-Circle back"),FLinearColor(.83f,.79f,.71f),X,Height-30*S,.8f*S);
}
