#include "MartyCharacter.h"
#include "CozyAdventure.h"
#include "Neighbourhood.h"
#include "DogLife.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "GameFramework/PlayerController.h"

bool AMartyHUD::DrawAdventureHUD()
{
    auto* Mode=GetWorld()->GetAuthGameMode<AMartyPlayMode>();
    auto* Session=Mode ? Mode->Adventure.Get() : nullptr;
    if (!Session || !Session->Enabled || !Session->Progress) return false;
    const float Width=Canvas->SizeX, Height=Canvas->SizeY;
    if(Session->Screen != ECozyScreen::Playing)
    {
        const bool Title=Session->Screen==ECozyScreen::Title;
        if(Title)
        {
            if(!Cover) Cover=LoadObject<UTexture2D>(nullptr,TEXT("/Game/Adventure/Art/Cover.Cover"));
            if(Cover) DrawTexture(Cover,0,0,Width,Height,0,0,1,1);
        }
        DrawRect(FLinearColor(.015f,.03f,.045f,Title?.12f:.64f),0,0,Width,Height);
        const float PanelWidth=Title?500.f:680.f;
        const float X=Title?32.f:FMath::Max(12.f,(Width-680)*.5f), Y=FMath::Max(12.f,(Height-480)*.5f);
        DrawRect(FLinearColor(.035f,.09f,.12f,.96f),X,Y,PanelWidth,480);
        DrawText(Session->Heading(),FLinearColor(1,.83f,.48f),X+24,Y+24,nullptr,1.65f);
        DrawText(TEXT("Marty, Lakitu & Rio  |  The Great Cozy Patrol"),FLinearColor::White,X+24,Y+62);
        if(Session->Screen == ECozyScreen::Controls)
        {
            const TArray<FString> Lines={TEXT("WASD / arrows / left stick: walk"),TEXT("Mouse / right stick: look around"),TEXT("Shift / A-Cross: hurry   |   E / X-Square: interact"),TEXT("V / R3: camera   |   R / RB-R1: recenter"),TEXT("Space / Y-Triangle: jump | F / RT: dig | Q / LT: drop | B / LB: bark"),TEXT("Esc / Menu-Options: pause   |   H: return home"),TEXT("Menus: arrows / D-pad, Enter / A-Cross, Backspace / B-Circle"),TEXT("Sniff golden snacks, visit both cats, then get Cheeween Cozy.")};
            for(int32 I=0;I<Lines.Num();++I) DrawText(Lines[I],FLinearColor::White,X+24,Y+105+I*28);
        }
        const auto Items=Session->Menu();
        const float First=Session->Screen==ECozyScreen::Controls ? Y+350 : Y+115;
        for(int32 I=0;I<Items.Num();++I)
        {
            const float Row=First+I*44;
            DrawRect(I==Session->Selection ? FLinearColor(.20f,.38f,.42f,1) : FLinearColor(.065f,.14f,.18f,1),X+18,Row,PanelWidth-36,38);
            DrawText((I==Session->Selection ? FString(TEXT(">  ")) : FString(TEXT("   ")))+Items[I],FLinearColor::White,X+28,Row+10,nullptr,1.15f);
            AddHitBox(FVector2D(X+18,Row),FVector2D(PanelWidth-36,38),FName(*FString::Printf(TEXT("CozyRow_%d"),I)),true);
            if(Session->Screen==ECozyScreen::Settings && I<2)
            {
                AddHitBox(FVector2D(X+550,Row),FVector2D(50,38),FName(*FString::Printf(TEXT("CozyMinus_%d"),I)),true,10);
                AddHitBox(FVector2D(X+605,Row),FVector2D(50,38),FName(*FString::Printf(TEXT("CozyPlus_%d"),I)),true,10);
                DrawText(TEXT("-       +"),FLinearColor::White,X+565,Row+10);
            }
        }
        DrawText(TEXT("Arrows / D-pad to choose  |  Enter / A-Cross to select"),FLinearColor(.75f,.85f,.86f),X+24,Y+435);
        if(Title) DrawText(TEXT("Cover illustration. In-game pets are stylized 3D models."),FLinearColor(.85f,.9f,.9f),Width-410,Height-28);
    }
    else
    {
        DrawRect(FLinearColor(.025f,.055f,.07f,.88f),16,16,760,120);
        const bool Outside=Mode->Neighbourhood&&Mode->Neighbourhood->Outside;
        DrawText(Outside?TEXT("Neighbourhood walk: meet the dogs, visit the park and school"):Session->Objective(),FLinearColor(1,.86f,.5f),28,28,nullptr,1.25f);
        DrawText(TEXT("Move: WASD / left stick | Interact: E / X-Square | Pause: Esc / Options"),FLinearColor::White,28,57);
        DrawText(TEXT("Look: mouse / right stick | Hurry: Shift / A-Cross | Camera: V / R3"),FLinearColor::White,28,79);
        const auto* OwnerPC=GetWorld()->GetFirstPlayerController();const auto* Pet=OwnerPC?Cast<AMartyCharacter>(OwnerPC->GetPawn()):nullptr;
        if(Pet&&Pet->Life)DrawText(Pet->Life->Status(),Pet->Life->Energy()<25?FLinearColor(1,.6f,.3f):FLinearColor(.7f,1,.7f),28,105,nullptr,1.1f);
        const FString Prompt=Session->Prompt();
        if(!Prompt.IsEmpty()) DrawText(Prompt,FLinearColor(1,.9f,.5f),Width*.5f-140,Height-110,nullptr,1.6f);
        if(!Session->Progress->Complete&&!Outside)
        {
            for(int32 I=0;I<Session->Mission->Snacks.Num();++I) if(!(Session->Progress->SnackMask & (1<<I)))
            {
                const auto& Stop=Session->Mission->Snacks[I]; const FVector P=Project(Stop.Position+FVector(0,0,32));
                if(P.Z>0 && P.X>0 && P.X<Width && P.Y>145 && P.Y<Height-60) DrawText(TEXT("SNACK"),FLinearColor(1,.85f,.35f),P.X-22,P.Y);
            }
            if(Session->Progress->SnackMask==(1<<Session->Mission->Snacks.Num())-1)
            {
                const auto& Stop=Session->Mission->Visits[Session->Progress->VisitIndex]; const FVector P=Project(Stop.Position+FVector(0,0,70));
                DrawText(Stop.Label,FLinearColor(.6f,1,1),FMath::Clamp(float(P.X),20.f,Width-180),FMath::Clamp(float(P.Y),120.f,Height-100),nullptr,1.3f);
                const auto* PC=Session->GetWorld()->GetFirstPlayerController(); const APawn* Pawn=PC ? PC->GetPawn() : nullptr;
                if(Pawn) DrawText(FString::Printf(TEXT("Destination: %.1f m"),FVector::Dist2D(Pawn->GetActorLocation(),Stop.Position)/100),FLinearColor::White,28,118);
            }
        }
    }
    if(Session->Screen==ECozyScreen::Playing&&Mode->Neighbourhood)
    {
        const auto* PC=GetWorld()->GetFirstPlayerController();const auto* Pawn=PC?Cast<AMartyCharacter>(PC->GetPawn()):nullptr;
        if(Pawn){const FString LifePrompt=Pawn->Life?Pawn->Life->Prompt():FString();DrawText(LifePrompt.IsEmpty()?Mode->Neighbourhood->Prompt(Pawn):LifePrompt,FLinearColor(.8f,1,.8f),28,Height-90,nullptr,1.1f);}
        DrawText(TEXT("Space / Y-Triangle: jump | F / RT: dig | Q / LT: drop | B / LB: bark"),FLinearColor(.8f,.9f,1),28,Height-145);
    }
    if(!Session->Message.IsEmpty()) { DrawRect(FLinearColor(.025f,.055f,.07f,.9f),16,Height-65,Width-32,48); DrawText(Session->Message,FLinearColor::White,28,Height-49,nullptr,1.1f); }
    return true;
}

void AMartyHUD::NotifyHitBoxClick(FName BoxName)
{
    Super::NotifyHitBoxClick(BoxName);
    auto* Mode=GetWorld()->GetAuthGameMode<AMartyPlayMode>(); if(!Mode || !Mode->Adventure) return;
    auto* Session=Mode->Adventure.Get(); const FString Name=BoxName.ToString();
    if(Name.StartsWith(TEXT("CozyRow_"))) { Session->Selection=FCString::Atoi(*Name.Mid(8)); Session->Confirm(); }
    else if(Name.StartsWith(TEXT("CozyMinus_"))) { Session->Selection=FCString::Atoi(*Name.Mid(10)); Session->Adjust(-1); }
    else if(Name.StartsWith(TEXT("CozyPlus_"))) { Session->Selection=FCString::Atoi(*Name.Mid(9)); Session->Adjust(1); }
}
