#include "MartyCharacter.h"
#include "HalloweenSpecial.h"
#include "CatCompanions.h"
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
    const bool HalloweenPage=Session->Screen==ECozyScreen::Halloween;
    const bool Seasonal=HalloweenPage||Session->Progress->HalloweenActive;
    const bool SeasonalArt=HalloweenPage||(Session->Screen==ECozyScreen::Title&&Seasonal);
    if(Session->Screen != ECozyScreen::Playing)
    {
        const bool Title=Session->Screen==ECozyScreen::Title;
        if(Title||HalloweenPage)
        {
            if(!Cover) Cover=LoadObject<UTexture2D>(nullptr,TEXT("/Game/Adventure/Art/Cover.Cover"));
            if(SeasonalArt&&!FestivalCover)FestivalCover=LoadObject<UTexture2D>(nullptr,TEXT("/Game/Adventure/HalloweenSpecial/HalloweenCover.HalloweenCover"));
            auto* Art=SeasonalArt&&FestivalCover?FestivalCover.Get():Cover.Get();
            if(Art)DrawTexture(Art,0,0,Width,Height,0,0,1,1);
        }
        DrawRect(FLinearColor(.015f,.03f,.045f,(Title||HalloweenPage)?.12f:.64f),0,0,Width,Height);
        const bool Map=Session->Screen==ECozyScreen::Map;
        const float PanelWidth=SeasonalArt?FMath::Min(560.f,Width*.44f):Title?500.f:Map?460.f:680.f;
        const float X=SeasonalArt?24.f:Title?32.f:Map?Width-484:FMath::Max(12.f,(Width-680)*.5f), Y=FMath::Max(12.f,(Height-480)*.5f);
        DrawRect(Seasonal?FLinearColor(.055f,.018f,.085f,.94f):FLinearColor(.035f,.09f,.12f,.96f),X,Y,PanelWidth,480);
        DrawText(Session->Heading(),FLinearColor(1,.83f,.48f),X+24,Y+24,nullptr,1.65f);
        DrawText(SeasonalArt?FString(TEXT("BORK OR TREAT / HALLOWEEN SPECIAL")):Session->Screen==ECozyScreen::Companions&&Mode->Companions?Mode->Companions->Status():FString(TEXT("Marty, Lakitu & Rio  |  The Great Cozy Patrol")),FLinearColor::White,X+24,Y+62);
        if(HalloweenPage)DrawText(FString::Printf(TEXT("PAW-PASS %d / 8 | %s"),Mode->Festival?Mode->Festival->StampCount():0,Session->Progress->HalloweenBadge?TEXT("PUMPKIN PAL EARNED"):TEXT("8 PORCHES / 1 COZY FINALE")),FLinearColor(1,.72f,.35f),X+24,Y+90,nullptr,.9f);
        if(Session->Screen == ECozyScreen::Controls)
        {
            const TArray<FString> Lines={TEXT("Move: WASD / LS | Look: mouse / RS | Hurry: Shift / A-Cross"),TEXT("Interact: E / X-Square | Jump: Space / Y-Triangle | Bark: B / LB"),TEXT("Dig: F / RT | Drop: Q / LT | Camera: V / R3 | Recenter: R / RB"),TEXT("Map: M / D-pad Up | Tricks: C / B-Circle | Bag: Tab / View"),TEXT("Crew: K / D-pad Down | Pause: Esc / Options | Home: H")};
            for(int32 I=0;I<Lines.Num();++I) DrawText(Lines[I],FLinearColor::White,X+24,Y+95+I*18);
        }
        const auto Items=Session->Menu();
        if(Session->Screen==ECozyScreen::Backpack)
        {
            const auto* PC=GetWorld()->GetFirstPlayerController();const auto* P=PC?Cast<AMartyCharacter>(PC->GetPawn()):nullptr;
            if(P&&P->Life){DrawText(P->Life->BackpackText(),FLinearColor::White,X+24,Y+105);DrawText(P->Life->Status(),FLinearColor(.7f,1,.7f),X+24,Y+140);}
            DrawText(TEXT("Carry in mouth, stow safely, retrieve, drop or bury."),FLinearColor::White,X+24,Y+180);
        }
        const float First=Session->Screen==ECozyScreen::Controls ? Y+190 : Session->Screen==ECozyScreen::Backpack?Y+225:Y+115;
        const int32 Visible=Session->Screen==ECozyScreen::Controls?5:Session->Screen==ECozyScreen::Backpack?3:7;
        const int32 Begin=FMath::Clamp(Session->Selection-Visible+1,0,FMath::Max(0,Items.Num()-Visible));
        for(int32 I=Begin;I<FMath::Min(Begin+Visible,Items.Num());++I)
        {
            const float Row=First+(I-Begin)*44;
            DrawRect(I==Session->Selection ? (Seasonal?FLinearColor(.40f,.16f,.055f,1):FLinearColor(.20f,.38f,.42f,1)) : (Seasonal?FLinearColor(.11f,.045f,.15f,1):FLinearColor(.065f,.14f,.18f,1)),X+18,Row,PanelWidth-36,38);
            DrawText((I==Session->Selection ? FString(TEXT(">  ")) : FString(TEXT("   ")))+Items[I],FLinearColor::White,X+28,Row+10,nullptr,1.15f);
            AddHitBox(FVector2D(X+18,Row),FVector2D(PanelWidth-36,38),FName(*FString::Printf(TEXT("CozyRow_%d"),I)),true);
            if(Session->Screen==ECozyScreen::Settings && I<2)
            {
                AddHitBox(FVector2D(X+550,Row),FVector2D(50,38),FName(*FString::Printf(TEXT("CozyMinus_%d"),I)),true,10);
                AddHitBox(FVector2D(X+605,Row),FVector2D(50,38),FName(*FString::Printf(TEXT("CozyPlus_%d"),I)),true,10);
                DrawText(TEXT("-       +"),FLinearColor::White,X+565,Row+10);
            }
        }
        DrawText(FString::Printf(TEXT("%d / %d | D-pad choose | A select | B back"),Session->Selection+1,Items.Num()),FLinearColor(.75f,.85f,.86f),X+24,Y+435);
        if(Map)DrawNavigation(true);
        if(Title||HalloweenPage) DrawText(TEXT("Illustration. Gameplay uses stylized 3D pets."),FLinearColor(.85f,.9f,.9f),Width-410,Height-28);
    }
    else
    {
        DrawRect(FLinearColor(.025f,.055f,.07f,.88f),16,16,760,170);
        const bool Outside=Mode->Neighbourhood&&Mode->Neighbourhood->Outside;
        DrawText(Seasonal?Session->Objective():Outside?TEXT("Neighbourhood walk: meet the dogs, visit the park and school"):Session->Objective(),FLinearColor(1,.86f,.5f),28,28,nullptr,1.25f);
        if(Session->Feature(ECozyFeature::Hints)){DrawText(TEXT("Move: WASD / left stick | Interact: E / X-Square | Pause: Esc / Options"),FLinearColor::White,28,57);
        DrawText(Session->Progress->BodyControls?TEXT("Body controls: LS steers | RS swings/turns | Mouse: look | V / R3: camera"):TEXT("Look: mouse / right stick | Hurry: Shift / A-Cross | Camera: V / R3"),FLinearColor::White,28,79);}
        const auto* OwnerPC=GetWorld()->GetFirstPlayerController();const auto* Pet=OwnerPC?Cast<AMartyCharacter>(OwnerPC->GetPawn()):nullptr;
        if(Pet&&Pet->Life)DrawText(Pet->Life->Status(),Pet->Life->Energy()<25?FLinearColor(1,.6f,.3f):FLinearColor(.7f,1,.7f),28,105,nullptr,1.1f);
        if(Pet&&Pet->Life)
        {
            if(Session->Feature(ECozyFeature::Health)){DrawText(FString::Printf(TEXT("Health %d / 100"),FMath::RoundToInt(Pet->Life->Health())),FLinearColor::White,28,130);
            DrawRect(FLinearColor(.2f,.08f,.08f),28,151,180,8);DrawRect(FLinearColor(.9f,.32f,.35f),28,151,Pet->Life->Health()*1.8f,8);
            }
            if(Session->Feature(ECozyFeature::Energy)){DrawText(TEXT("Energy"),FLinearColor::White,250,130);DrawRect(FLinearColor(.08f,.18f,.1f),250,151,180,8);DrawRect(FLinearColor(.3f,.85f,.45f),250,151,Pet->Life->Energy()*1.8f,8);}
            if(Session->Feature(ECozyFeature::Backpack)){DrawRect(FLinearColor(.025f,.055f,.07f,.88f),Width-295,16,279,100);
            DrawText(FString::Printf(TEXT("BACKPACK %d / 4"),Session->Progress->BagItems.Num()),FLinearColor(.9f,.82f,.6f),Width-282,30);
            DrawText(TEXT("Tab / View-Touchpad: open backpack"),FLinearColor::White,Width-282,80);}
        }
        const FString Prompt=Session->Prompt();
        if(!Prompt.IsEmpty()) DrawText(Prompt,FLinearColor(1,.9f,.5f),Width*.5f-140,Height-110,nullptr,1.6f);
        if(Session->Feature(ECozyFeature::WorldLabels)&&!Session->Progress->Complete&&!Outside&&!Seasonal)
        {
            for(int32 I=0;I<Session->Mission->Snacks.Num();++I) if(!(Session->Progress->SnackMask & (1<<I)))
            {
                const auto& Stop=Session->Mission->Snacks[I]; const FVector P=Project(Stop.Position+FVector(0,0,32));
                if(P.Z>0 && P.X>0 && P.X<Width && P.Y>175 && P.Y<Height-60) DrawText(TEXT("SNACK"),FLinearColor(1,.85f,.35f),P.X-22,P.Y);
            }
            if(Session->Progress->SnackMask==(1<<Session->Mission->Snacks.Num())-1)
            {
                const auto& Stop=Session->Mission->Visits[Session->Progress->VisitIndex]; const FVector P=Project(Session->StopPosition(Stop)+FVector(0,0,70));
                DrawText(Stop.Label,FLinearColor(.6f,1,1),FMath::Clamp(float(P.X),20.f,Width-180),FMath::Clamp(float(P.Y),120.f,Height-100),nullptr,1.3f);
            }
        }
    }
    if(Session->Screen==ECozyScreen::Playing&&Mode->Neighbourhood)
    {
        DrawNavigation(false);
        const auto* PC=GetWorld()->GetFirstPlayerController();const auto* Pawn=PC?Cast<AMartyCharacter>(PC->GetPawn()):nullptr;
        if(Pawn){const FString LifePrompt=Pawn->Life?Pawn->Life->Prompt():FString();const FString CatPrompt=Mode->Companions?Mode->Companions->Prompt():FString();const FString AreaPrompt=Mode->Neighbourhood->Prompt(Pawn);DrawText(!Session->Prompt().IsEmpty()?FString():!LifePrompt.IsEmpty()?LifePrompt:AreaPrompt.StartsWith(TEXT("E /"))?AreaPrompt:!CatPrompt.IsEmpty()?CatPrompt:AreaPrompt,FLinearColor(.8f,1,.8f),28,Height-90,nullptr,1.1f);}
        if(Session->Feature(ECozyFeature::Hints)){DrawText(TEXT("Space / Y-Triangle: jump | F / RT: dig | Q / LT: drop | B / LB: bark"),FLinearColor(.8f,.9f,1),28,Height-145);
        DrawText(TEXT("Map: M / D-pad Up | Crew: K / D-pad Down | C: tricks | Tab: backpack"),FLinearColor(.8f,.9f,1),28,Height-170);}
    }
    if(Seasonal&&Session->Screen==ECozyScreen::Playing&&Mode->Festival){
        DrawRect(FLinearColor(.055f,.018f,.085f,.94f),18,Height-245,510,52);
        DrawText(FString::Printf(TEXT("BORK OR TREAT / PAW-PASS %d of 8%s"),Mode->Festival->StampCount(),Session->Progress->HalloweenBadge?TEXT(" / PUMPKIN PAL"):TEXT("")),FLinearColor(1,.75f,.35f),28,Height-235);
        for(int32 I=0;I<8;++I)DrawRect((Session->Progress->HalloweenStampMask&(1<<I))?FLinearColor(1,.58f,.15f):FLinearColor(.20f,.10f,.27f),28+I*58,Height-214,48,9);
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
