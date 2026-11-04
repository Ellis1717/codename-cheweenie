#include "ControllerGuide.h"
#include "MartyCharacter.h"
#include "CozyHUDStyle.h"
#include "HalloweenSpecial.h"
#include "CatCompanions.h"
#include "CozyAdventure.h"
#include "CozyModeHub.h"
#include "Engine/GameInstance.h"
#include "Neighbourhood.h"
#include "DogLife.h"
#include "HomeProgression.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "GameFramework/PlayerController.h"

bool AMartyHUD::DrawAdventureHUD()
{
    auto* Mode=GetWorld()->GetAuthGameMode<AMartyPlayMode>();
    auto* Session=Mode ? Mode->Adventure.Get() : nullptr;
    if (!Session || !Session->Enabled || !Session->Progress) return false;
    const float Width=Canvas->SizeX, Height=Canvas->SizeY;
    auto* Hub=GetWorld()->GetGameInstance()->GetSubsystem<UCozyModeHub>();const bool Modes=Session->Screen==ECozyScreen::Modes;
    auto Show=[&](ECozyOverlay Id){return Hub->OverlayVisible(Id);};
    const bool HalloweenPage=Session->Screen==ECozyScreen::Halloween;
    const bool Seasonal=HalloweenPage||Session->Progress->HalloweenActive;
    const bool SeasonalArt=HalloweenPage||(Session->Screen==ECozyScreen::Title&&Seasonal);
    if(Session->Screen != ECozyScreen::Playing)
    {
        const bool Title=Session->Screen==ECozyScreen::Title;
        if(Title||HalloweenPage||Modes)
        {
            if(!Cover) Cover=LoadObject<UTexture2D>(nullptr,TEXT("/Game/Adventure/Art/Cover.Cover"));
            if(SeasonalArt&&!FestivalCover)FestivalCover=LoadObject<UTexture2D>(nullptr,TEXT("/Game/Adventure/HalloweenSpecial/HalloweenCover.HalloweenCover"));
            auto* Art=SeasonalArt&&FestivalCover?FestivalCover.Get():Cover.Get();
            if(Art)DrawTexture(Art,0,0,Width,Height,0,0,1,1);
        }
        CozyDrawRect(this,FLinearColor(.015f,.03f,.045f,(Title||HalloweenPage||Modes)?.12f:.64f),0,0,Width,Height);
        const bool Map=Session->Screen==ECozyScreen::Map;
        const bool Controls=Session->Screen==ECozyScreen::Controls;
        const float ControlWidth=FMath::Min(1040.f,Width-24.f);
        const float PanelWidth=SeasonalArt?FMath::Min(560.f,Width*.44f):Title?500.f:Map?460.f:Controls?ControlWidth:680.f;
        const float X=SeasonalArt?24.f:Title?32.f:Map?Width-484:Controls?(Width-ControlWidth)*.5f:FMath::Max(12.f,(Width-680)*.5f), Y=FMath::Max(12.f,(Height-(Controls?620:480))*.5f);
        CozyDrawRect(this,Seasonal?FLinearColor(.055f,.018f,.085f,.94f):FLinearColor(.035f,.09f,.12f,.96f),X,Y,PanelWidth,Controls?620:480);
        CozyDrawText(this,Session->Heading(),FLinearColor(1,.83f,.48f),X+24,Y+24,nullptr,1.65f);
        CozyDrawText(this,SeasonalArt?FString(TEXT("BORK OR TREAT / HALLOWEEN SPECIAL")):Session->Screen==ECozyScreen::Companions&&Mode->Companions?Mode->Companions->Status():Modes?FString(TEXT("One adventure | Play, settings, multiplayer & older versions")):FString(TEXT("Marty, Lakitu & Rio  |  The Great Cozy Patrol")),FLinearColor::White,X+24,Y+62);
        if(HalloweenPage)CozyDrawText(this,FString::Printf(TEXT("PAW-PASS %d / 8 | %s"),Mode->Festival?Mode->Festival->StampCount():0,Session->Progress->HalloweenBadge?TEXT("PUMPKIN PAL EARNED"):TEXT("8 PORCHES / 1 COZY FINALE")),FLinearColor(1,.72f,.35f),X+24,Y+90,nullptr,.9f);
        if(Session->Screen == ECozyScreen::Controls)
        {
            auto* Guide=GetWorld()->GetGameInstance()->GetSubsystem<UControllerGuide>();
            Guide->Draw(this,X+24,Y+95);
            const auto Lines=Guide->Controls();
            for(int32 I=0;I<Lines.Num();++I) CozyDrawText(this,Lines[I],FLinearColor::White,X+410,Y+100+I*23,nullptr,.85f);
        }
        const auto Items=Session->Menu();
        if(Session->Screen==ECozyScreen::Backpack)
        {
            const auto* PC=GetWorld()->GetFirstPlayerController();const auto* P=PC?Cast<AMartyCharacter>(PC->GetPawn()):nullptr;
            if(P&&P->Life){CozyDrawText(this,P->Life->BackpackText(),FLinearColor::White,X+24,Y+105);CozyDrawText(this,P->Life->Status(),FLinearColor(.7f,1,.7f),X+24,Y+140);}
            CozyDrawText(this,TEXT("Carry in mouth, stow safely, retrieve, drop or bury."),FLinearColor::White,X+24,Y+180);
        }
        const float First=Session->Screen==ECozyScreen::Controls ? Y+310 : Session->Screen==ECozyScreen::Backpack?Y+225:Y+115;
        const int32 Visible=Session->Screen==ECozyScreen::Controls?4:Session->Screen==ECozyScreen::Backpack?4:7;
        const int32 Begin=FMath::Clamp(Session->Selection-Visible+1,0,FMath::Max(0,Items.Num()-Visible));
        for(int32 I=Begin;I<FMath::Min(Begin+Visible,Items.Num());++I)
        {
            const float Row=First+(I-Begin)*44;
            CozyDrawRect(this,I==Session->Selection ? (Seasonal?FLinearColor(.40f,.16f,.055f,1):FLinearColor(.20f,.38f,.42f,1)) : (Seasonal?FLinearColor(.11f,.045f,.15f,1):FLinearColor(.065f,.14f,.18f,1)),X+18,Row,PanelWidth-36,38);
            CozyDrawText(this,(I==Session->Selection ? FString(TEXT(">  ")) : FString(TEXT("   ")))+Items[I],FLinearColor::White,X+28,Row+10,nullptr,1.15f);
            AddHitBox(FVector2D(X+18,Row),FVector2D(PanelWidth-36,38),FName(*FString::Printf(TEXT("CozyRow_%d"),I)),true);
            if((Modes&&Hub->CanAdjust(I))||(Session->Screen==ECozyScreen::Settings && I<2)||(Session->Screen==ECozyScreen::Features && Session->FeatureGroup().IsValidIndex(I) && Session->FeatureGroup()[I]==ECozyFeature::DetailedCoat))
            {
                AddHitBox(FVector2D(X+550,Row),FVector2D(50,38),FName(*FString::Printf(TEXT("CozyMinus_%d"),I)),true,10);
                AddHitBox(FVector2D(X+605,Row),FVector2D(50,38),FName(*FString::Printf(TEXT("CozyPlus_%d"),I)),true,10);
                CozyDrawText(this,TEXT("-       +"),FLinearColor::White,X+565,Row+10);
            }
        }
        CozyDrawText(this,Session->Screen==ECozyScreen::Features?FString(TEXT("Arrows / D-pad: choose & adjust | Enter / A-Cross: next | B-Circle: back")):FString::Printf(TEXT("%d / %d | D-pad choose | A select | B back"),Session->Selection+1,Items.Num()),FLinearColor(.75f,.85f,.86f),X+24,Y+(Controls?589:435));
        if(Modes)CozyDrawText(this,Hub->Hint(),FLinearColor(1,.88f,.65f),20,Height-48,nullptr,.95f);
        if(Map)DrawNavigation(true);
        if(Title||HalloweenPage||Modes) CozyDrawText(this,TEXT("Illustration. Gameplay uses stylized 3D pets."),FLinearColor(.85f,.9f,.9f),Width-410,Height-28);
    }
    else
    {
        if((Show(ECozyOverlay::Objective))||(Show(ECozyOverlay::Hints)&&Session->Feature(ECozyFeature::Hints))||Show(ECozyOverlay::Status)||Show(ECozyOverlay::HomeStats)||(Show(ECozyOverlay::Health)&&Session->Feature(ECozyFeature::Health))||(Show(ECozyOverlay::Energy)&&Session->Feature(ECozyFeature::Energy)))
            CozyDrawRect(this,FLinearColor(.025f,.055f,.07f,.88f),16,16,760,170);
        const bool Outside=Mode->Neighbourhood&&Mode->Neighbourhood->Outside;
        if(Show(ECozyOverlay::Objective))CozyDrawText(this,Mode->HomeProgression&&Mode->HomeProgression->Training>=0?Mode->HomeProgression->TrainingObjective():Seasonal?Session->Objective():Outside?TEXT("Neighbourhood walk: meet the dogs, visit the park and school"):Session->Objective(),FLinearColor(1,.86f,.5f),28,28,nullptr,1.25f);
        if(Show(ECozyOverlay::Hints)&&Session->Feature(ECozyFeature::Hints)){CozyDrawText(this,TEXT("Move: WASD / left stick | Interact: E / X-Square | Pause: Esc / Options"),FLinearColor::White,28,57);
        CozyDrawText(this,Session->Progress->BodyControls?TEXT("Body controls: LS steers | RS swings/turns | Mouse: look | V / R3: camera"):TEXT("Look: mouse / right stick | Hurry: Shift / A-Cross | Camera: V / R3"),FLinearColor::White,28,79);}
        const auto* OwnerPC=GetWorld()->GetFirstPlayerController();const auto* Pet=OwnerPC?Cast<AMartyCharacter>(OwnerPC->GetPawn()):nullptr;
        if(Show(ECozyOverlay::Status)&&Pet&&Pet->Life)CozyDrawText(this,Pet->Life->Status(),Pet->Life->Energy()<25?FLinearColor(1,.6f,.3f):FLinearColor(.7f,1,.7f),28,105,nullptr,1.1f);
        if(Pet&&Pet->Life)
        {
            if(Show(ECozyOverlay::Health)&&Session->Feature(ECozyFeature::Health)){CozyDrawText(this,FString::Printf(TEXT("Health %d / 100"),FMath::RoundToInt(Pet->Life->Health())),FLinearColor::White,28,130);
            CozyDrawRect(this,FLinearColor(.2f,.08f,.08f),28,151,180,8);CozyDrawRect(this,FLinearColor(.9f,.32f,.35f),28,151,Pet->Life->Health()*1.8f,8);
            }
            if(Show(ECozyOverlay::Energy)&&Session->Feature(ECozyFeature::Energy)){CozyDrawText(this,TEXT("Energy"),FLinearColor::White,250,130);CozyDrawRect(this,FLinearColor(.08f,.18f,.1f),250,151,180,8);CozyDrawRect(this,FLinearColor(.3f,.85f,.45f),250,151,Pet->Life->Energy()*1.8f,8);}
            if(Show(ECozyOverlay::Backpack)&&Session->Feature(ECozyFeature::Backpack)){CozyDrawRect(this,FLinearColor(.025f,.055f,.07f,.88f),Width-295,16,279,100);
            CozyDrawText(this,FString::Printf(TEXT("BACKPACK %d / 4"),Session->Progress->BagItems.Num()),FLinearColor(.9f,.82f,.6f),Width-282,30);
            CozyDrawText(this,TEXT("Tab / View-Touchpad: open backpack"),FLinearColor::White,Width-282,80);}
        }
        if(Show(ECozyOverlay::HomeStats)&&Mode->HomeProgression){auto* H=Mode->HomeProgression.Get();CozyDrawText(this,H->Summary(),FLinearColor(1,.85f,.55f),450,127,nullptr,.9f);if(H->Enabled()){CozyDrawText(this,FString::Printf(TEXT("STR %d | AGI %d | SPD %d"),H->Level(0),H->Level(1),H->Level(2)),FLinearColor(.7f,.95f,.8f),450,146,nullptr,.9f);CozyDrawText(this,H->BuffSummary(),FLinearColor(.75f,.85f,.9f),450,166,nullptr,.85f);}}
        const FString Prompt=Session->Prompt();
        if(Show(ECozyOverlay::Prompts)&&!Prompt.IsEmpty()) CozyDrawText(this,Prompt,FLinearColor(1,.9f,.5f),Width*.5f-140,Height-110,nullptr,1.6f);
        if(Show(ECozyOverlay::WorldLabels)&&Session->Feature(ECozyFeature::WorldLabels)&&!Session->Progress->Complete&&!Outside&&!Seasonal&&!(Mode->HomeProgression&&Mode->HomeProgression->InAnnex()))
        {
            for(int32 I=0;I<Session->Mission->Snacks.Num();++I) if(!(Session->Progress->SnackMask & (1<<I)))
            {
                const auto& Stop=Session->Mission->Snacks[I]; const FVector P=Project(Stop.Position+FVector(0,0,32));
                if(P.Z>0 && P.X>0 && P.X<Width && P.Y>175 && P.Y<Height-60) CozyDrawText(this,TEXT("SNACK"),FLinearColor(1,.85f,.35f),P.X-22,P.Y);
            }
            if(Session->Progress->SnackMask==(1<<Session->Mission->Snacks.Num())-1)
            {
                const auto& Stop=Session->Mission->Visits[Session->Progress->VisitIndex]; const FVector P=Project(Session->StopPosition(Stop)+FVector(0,0,70));
                CozyDrawText(this,Stop.Label,FLinearColor(.6f,1,1),FMath::Clamp(float(P.X),20.f,Width-180),FMath::Clamp(float(P.Y),120.f,Height-100),nullptr,1.3f);
            }
        }
    }
    if(Session->Screen==ECozyScreen::Playing&&Mode->Neighbourhood)
    {
        DrawNavigation(false);
        const auto* PC=GetWorld()->GetFirstPlayerController();const auto* Pawn=PC?Cast<AMartyCharacter>(PC->GetPawn()):nullptr;
        if(Show(ECozyOverlay::Prompts)&&Pawn){const FString HomePrompt=Mode->HomeProgression?Mode->HomeProgression->Prompt():FString();const FString LifePrompt=!HomePrompt.IsEmpty()?HomePrompt:Pawn->Life?Pawn->Life->Prompt():FString();const FString CatPrompt=Mode->Companions?Mode->Companions->Prompt():FString();const FString AreaPrompt=Mode->HomeProgression&&Mode->HomeProgression->InAnnex()?TEXT("Garage + gym: use E / X-Square at a station or the return door"):Mode->Neighbourhood->Prompt(Pawn);CozyDrawText(this,!Session->Prompt().IsEmpty()?FString():!LifePrompt.IsEmpty()?LifePrompt:AreaPrompt.StartsWith(TEXT("E /"))?AreaPrompt:!CatPrompt.IsEmpty()?CatPrompt:AreaPrompt,FLinearColor(.8f,1,.8f),28,Height-90,nullptr,1.1f);}
        if(Show(ECozyOverlay::Hints)&&Session->Feature(ECozyFeature::Hints)&&(!Show(ECozyOverlay::TrickGuide)||!Pawn||!Pawn->TrickModifierVisible())){CozyDrawText(this,TEXT("Space / Y-Triangle: jump | F / RT: dig | Q / LT: drop | B / tap LB: bark"),FLinearColor(.8f,.9f,1),28,Height-145);
        CozyDrawText(this,TEXT("Map: M / D-pad Up | Crew: K / D-pad Down | Hold L1/LB: direct tricks | C: trick menu"),FLinearColor(.8f,.9f,1),28,Height-170);}
    }
    if(Show(ECozyOverlay::Festival)&&Seasonal&&Session->Screen==ECozyScreen::Playing&&Mode->Festival){
        CozyDrawRect(this,FLinearColor(.055f,.018f,.085f,.94f),18,Height-245,510,52);
        CozyDrawText(this,FString::Printf(TEXT("BORK OR TREAT / PAW-PASS %d of 8%s"),Mode->Festival->StampCount(),Session->Progress->HalloweenBadge?TEXT(" / PUMPKIN PAL"):TEXT("")),FLinearColor(1,.75f,.35f),28,Height-235);
        for(int32 I=0;I<8;++I)CozyDrawRect(this,(Session->Progress->HalloweenStampMask&(1<<I))?FLinearColor(1,.58f,.15f):FLinearColor(.20f,.10f,.27f),28+I*58,Height-214,48,9);
    }
    const auto* TrickPC=GetWorld()->GetFirstPlayerController();const auto* TrickPet=TrickPC?Cast<AMartyCharacter>(TrickPC->GetPawn()):nullptr;
    if(Show(ECozyOverlay::TrickGuide)&&TrickPet&&TrickPet->TrickModifierVisible())
    {
        const float X=Width-500,Y=Height-190;CozyDrawRect(this,FLinearColor(.025f,.055f,.07f,.94f),X,Y,484,104);
        CozyDrawText(this,TEXT("DIRECT TRICKS | Hold L1 / LB or Left Ctrl"),FLinearColor(1,.86f,.5f),X+12,Y+10);
        CozyDrawText(this,Session->Feature(ECozyFeature::Tricks)?TEXT("Cross / A: lie or stand     Circle / B: roll over"):TEXT("Dog tricks are OFF in Controls > Pet feature switches"),FLinearColor::White,X+12,Y+34);
        CozyDrawText(this,Session->Feature(ECozyFeature::Tricks)?TEXT("Square / X: shake paw     Triangle / Y: bork!"):TEXT("Triangle / Y: bork (when Barking is enabled)"),FLinearColor::White,X+12,Y+56);
        CozyDrawText(this,TEXT("Keyboard: 1 lie / stand | 2 roll | 3 shake | 4 bark"),FLinearColor(.75f,.85f,.86f),X+12,Y+78);
    }
    if(Show(ECozyOverlay::Messages)&&!Modes&&!Session->Message.IsEmpty()) { CozyDrawRect(this,FLinearColor(.025f,.055f,.07f,.9f),16,Height-65,Width-32,48); CozyDrawText(this,Session->Message,FLinearColor::White,28,Height-49,nullptr,1.1f); }
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
