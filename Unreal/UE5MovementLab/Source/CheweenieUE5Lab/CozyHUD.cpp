#include "ControllerGuide.h"
#include "OpenMissions.h"
#include "ControllerComfort.h"
#include "HockeySurprise.h"
#include "MartyCharacter.h"
#include "CozyHUDStyle.h"
#include "HalloweenSpecial.h"
#include "CatCompanions.h"
#include "CozyAdventure.h"
#include "CozyModeHub.h"
#include "Engine/GameInstance.h"
#include "Neighbourhood.h"
#include "DogLife.h"
#include "DigVisuals.h"
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
    if(Session->Screen==ECozyScreen::Missions)return CozyDrawMissionJournal(this,Session,Width,Height);
    if(Session->Screen != ECozyScreen::Playing)
    {
        const bool Title=Session->Screen==ECozyScreen::Title;
        const bool Controls=Session->Screen==ECozyScreen::Controls;
        const bool KeyboardControls=Controls&&GetGameInstance()->GetSubsystem<UControllerGuide>()->Family()==ECozyInputFamily::Keyboard;
        const bool Map=Session->Screen==ECozyScreen::Map||Session->Screen==ECozyScreen::ControllerSetup||(Modes&&Hub->Page==ECozyHubPage::ControllerSetup);
        const bool Artwork=(Session->IllustratedMenus||HalloweenPage)&&Session->Feature(ECozyFeature::MenuArtwork);
        const float S=FMath::Min(Width/1280.f,Height/720.f);
        if(Artwork)
        {
            if(!Cover)Cover=LoadObject<UTexture2D>(nullptr,TEXT("/Game/Adventure/Art/Cover.Cover"));
            if(SeasonalArt&&!FestivalCover)FestivalCover=LoadObject<UTexture2D>(nullptr,TEXT("/Game/Adventure/HalloweenSpecial/HalloweenCover.HalloweenCover"));
            UTexture2D* Art=SeasonalArt&&FestivalCover?FestivalCover.Get():Title||HalloweenPage||(Modes&&Hub->Page==ECozyHubPage::Modes)?Cover.Get():Hub->MenuBackdrop();
            if(!Art)Art=Cover.Get();
            if(Art)DrawTexture(Art,0,0,Width,Height,0,0,1,1);
        }
        if(Modes&&Hub->Page==ECozyHubPage::Biography)
        {
            CozyDrawBiography(this,Hub,Width,Height,TEXT("CozyRow_"));
            return true;
        }
        // The game scene remains visible beneath in-game menus. Controls and maps
        // retain a soft local scrim, while ordinary menus use only a feathered edge.
        DrawRect(FLinearColor(.012f,.018f,.025f,Artwork?.07f:.18f),0,0,Width,Height);
        const float PanelWidth=Controls?FMath::Min(1040.f*S,Width-24):Map?460*S:FMath::Min((Artwork?680:520)*S,Width*(Artwork?.46f:.40f));
        const float X=Map?Width-PanelWidth-24*S:Controls?(Width-PanelWidth)*.5f:Width*.045f;
        const float PanelHeight=(Controls?(KeyboardControls?690:620):480)*S;
        const float Y=FMath::Max(12.f,(Height-PanelHeight)*.5f);
        if(Controls||Map)
            DrawRect(FLinearColor(.02f,.025f,.03f,Controls?.66f:.72f),X-8*S,Y-8*S,PanelWidth+16*S,PanelHeight+16*S);
        else CozyMenuShade(this,0,0,FMath::Max(Width*.59f,PanelWidth+X+50*S),Height,Artwork?.60f:.77f);
        auto MenuText=[&](const FString& Text,FLinearColor Color,float TX,float TY,UFont* Font=nullptr,float Scale=1.f)
        {
            float TW=0,TH=0;GetTextSize(Text,TW,TH,Font,Scale*S);
            const float Available=PanelWidth-48*S;
            CozyMenuText(this,Text,Color,TX,TY,TW>Available?Scale*S*Available/TW:Scale*S);
        };
        MenuText(Session->Heading(),FLinearColor(1,.85f,.57f),X+24*S,Y+24*S,nullptr,1.65f);
        MenuText(SeasonalArt?FString(TEXT("BORK OR TREAT / HALLOWEEN SPECIAL")):Session->Screen==ECozyScreen::Companions&&Mode->Companions?Mode->Companions->Status():Modes&&Hub->Page==ECozyHubPage::Crew?FString(TEXT("Three little lives, one shared adventure.")):Modes?FString(TEXT("Play, explore and make yourself at home")):FString(TEXT("Marty, Lakitu & Rio | Explore at your own pace")),FLinearColor(.94f,.89f,.79f),X+24*S,Y+62*S);
        if(HalloweenPage)MenuText(FString::Printf(TEXT("PAW-PASS %d / 8 | %s"),Mode->Festival?Mode->Festival->StampCount():0,Session->Progress->HalloweenBadge?TEXT("PUMPKIN PAL EARNED"):TEXT("8 PORCHES / 1 COZY FINALE")),FLinearColor(1,.76f,.43f),X+24*S,Y+90*S,nullptr,.9f);
        if(Controls)
        {
            auto* Guide=GetWorld()->GetGameInstance()->GetSubsystem<UControllerGuide>();
            Guide->Draw(this,X+24*S,Y+95*S,S);
            const auto Lines=Guide->Controls();
            for(int32 I=0;I<Lines.Num();++I)
                CozyMenuText(this,Lines[I],FLinearColor(.98f,.95f,.88f),
                    X+(KeyboardControls?24+(I/6)*506:410)*S,
                    Y+(KeyboardControls?310+(I%6)*19:100+I*23)*S,(KeyboardControls?.95f:.85f)*S);
        }
        const auto Items=Session->Menu();
        const bool Backpack=Session->Screen==ECozyScreen::Backpack;
        if(Backpack)
        {
            const auto* PC=GetWorld()->GetFirstPlayerController();const auto* P=PC?Cast<AMartyCharacter>(PC->GetPawn()):nullptr;
            if(P&&P->Life){MenuText(P->Life->BackpackText(),FLinearColor(.97f,.93f,.83f),X+24*S,Y+105*S);MenuText(P->Life->Status(),FLinearColor(.86f,.94f,.75f),X+24*S,Y+140*S);}
            MenuText(TEXT("Carry, stow, retrieve, drop or bury a treasure."),FLinearColor(.9f,.85f,.75f),X+24*S,Y+180*S);
        }
        const float First=Y+(Controls?(KeyboardControls?438:310):Backpack?225:115)*S;
        const int32 Visible=Controls||Backpack?4:7;
        const int32 Begin=FMath::Clamp(Session->Selection-Visible+1,0,FMath::Max(0,Items.Num()-Visible));
        for(int32 I=Begin;I<FMath::Min(Begin+Visible,Items.Num());++I)
        {
            const float Row=First+(I-Begin)*44*S;
            const bool Adjustable=(Session->Screen==ECozyScreen::ControllerSetup&&I<6)||(Session->Screen==ECozyScreen::Wardrobe&&I>=3&&I<=6)||(Modes&&Hub->CanAdjust(I))||(Session->Screen==ECozyScreen::Settings&&I<2)||(Session->Screen==ECozyScreen::Features&&Session->FeatureGroup().IsValidIndex(I)&&Session->FeatureGroup()[I]==ECozyFeature::DetailedCoat);
            CozyMenuRow(this,I==Session->Selection,X+18*S,Row,PanelWidth-36*S,38*S);
            const FString Label=(I==Session->Selection?FString(TEXT(">  ")):FString(TEXT("   ")))+Items[I];
            float TW=0,TH=0;GetTextSize(Label,TW,TH,nullptr,1.15f*S);
            const float Available=PanelWidth-(Adjustable?168:64)*S;
            const float TextScale=TW>Available?1.15f*S*Available/TW:1.15f*S;
            CozyMenuText(this,Label,FLinearColor(1,I==Session->Selection?.87f:.93f,I==Session->Selection?.63f:.83f),X+28*S,Row+10*S,TextScale);
            AddHitBox(FVector2D(X+18*S,Row),FVector2D(PanelWidth-36*S,38*S),FName(*FString::Printf(TEXT("CozyRow_%d"),I)),true);
            if(Adjustable)
            {
                const float AX=X+PanelWidth-126*S;
                AddHitBox(FVector2D(AX,Row),FVector2D(50*S,38*S),FName(*FString::Printf(TEXT("CozyMinus_%d"),I)),true,10);
                AddHitBox(FVector2D(AX+55*S,Row),FVector2D(50*S,38*S),FName(*FString::Printf(TEXT("CozyPlus_%d"),I)),true,10);
                CozyMenuText(this,TEXT("-       +"),FLinearColor(1,.85f,.57f),AX+15*S,Row+10*S,S);
            }
        }
        MenuText(FString::Printf(TEXT("%d / %d | Arrows / D-pad | Enter / A-Cross | B-Circle back"),Session->Selection+1,Items.Num()),FLinearColor(.84f,.79f,.69f),X+24*S,Y+(Controls?(KeyboardControls?650:589):435)*S,nullptr,.9f);
        if(Modes)CozyMenuText(this,Hub->Hint(),FLinearColor(.91f,.85f,.72f),20*S,Height-48*S,.85f*S);
        if(Map)DrawNavigation(true);
        if(Artwork)CozyMenuText(this,TEXT("Illustration. Gameplay uses stylized 3D pets."),FLinearColor(.82f,.78f,.72f),Width-365*S,Height-25*S,.8f*S);
    }
    else
    {
        if((Show(ECozyOverlay::Objective))||(Show(ECozyOverlay::Hints)&&Session->Feature(ECozyFeature::Hints))||Show(ECozyOverlay::Status)||Show(ECozyOverlay::HomeStats)||(Show(ECozyOverlay::Health)&&Session->Feature(ECozyFeature::Health))||(Show(ECozyOverlay::Energy)&&Session->Feature(ECozyFeature::Energy)))
            CozyDrawRect(this,FLinearColor(.025f,.055f,.07f,.88f),16,16,760,170);
        const bool Outside=Mode->Neighbourhood&&Mode->Neighbourhood->Outside;
        const auto* CameraPC=GetWorld()->GetFirstPlayerController();
        const auto* CameraPet=CameraPC?Cast<AMartyCharacter>(CameraPC->GetPawn()):nullptr;
        if(Show(ECozyOverlay::Objective))CozyDrawText(this,Mode->HomeProgression&&Mode->HomeProgression->Training>=0?Mode->HomeProgression->TrainingObjective():(Seasonal||(Mode->Hockey&&Mode->Hockey->Active()))?Session->Objective():Session->Progress->ShowMissionTracker?Session->MissionTracker():FString(TEXT("Explore freely")),FLinearColor(1,.86f,.5f),28,28,nullptr,1.25f);
        if(Show(ECozyOverlay::Hints)&&Session->Feature(ECozyFeature::Hints)){CozyDrawText(this,TEXT("Move: WASD / left stick | Interact: E / X-Square | Pause: Esc / Options"),FLinearColor::White,28,57);
        CozyDrawText(this,(Session->Progress->BodyControls?FString(TEXT("RS: body sway | ")):FString())+(CameraPet?CameraPet->CameraName()+TEXT(" | "):FString())+TEXT("V / R3: view | Wheel, +/- / D-pad L-R: zoom"),FLinearColor::White,28,79);}
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
        if(Session->Progress->ShowMissionTracker&&Show(ECozyOverlay::Objective)){CozyDrawRect(this,FLinearColor(.025f,.055f,.07f,.88f),16,186,760,29);CozyDrawText(this,Session->MissionNext(Session->Progress->TrackedOpenMission),FLinearColor(.9f,.86f,.72f),28,192,nullptr,.9f);}
        if(Show(ECozyOverlay::Messages)&&!Session->MissionNotice.IsEmpty()){CozyDrawRect(this,FLinearColor(.055f,.075f,.055f,.95f),16,FMath::Max(304.f,Height-288.f),760,36);CozyDrawText(this,Session->MissionNotice,FLinearColor(1,.87f,.5f),28,FMath::Max(304.f,Height-288.f)+10, nullptr,.95f);}
        const FString Prompt=Session->Prompt();
        if(Show(ECozyOverlay::Prompts)&&!Prompt.IsEmpty()) CozyDrawText(this,Prompt,FLinearColor(1,.9f,.5f),Width*.5f-140,Height-110,nullptr,1.6f);
        if(Show(ECozyOverlay::Prompts)&&Session->Feature(ECozyFeature::Dig)&&Pet)
        if(const auto* Dig=Pet->FindComponentByClass<UDigVisuals>();Dig&&Dig->Nearest()>=0)
        {
            const int32 Patch=Dig->Nearest(),Depth=Dig->VisibleDepth(Patch);const float X=Width-290,Y=Height-245;
            CozyDrawRect(this,FLinearColor(.08f,.045f,.02f,.94f),X,Y,265,105);
            CozyDrawText(this,TEXT("DIG PATCH"),FLinearColor(1,.8f,.46f),X+12,Y+10);
            CozyDrawText(this,Dig->DepthText(Patch),FLinearColor(1,.92f,.75f),X+12,Y+32,nullptr,.9f);
            for(int32 I=0;I<3;++I)CozyDrawRect(this,I<Depth?FLinearColor(.85f,.5f,.18f):FLinearColor(.23f,.15f,.09f),X+12+I*80,Y+58,72,12);
            if(Pet->Life&&Pet->Life->Activity==EDogActivity::Digging){CozyDrawRect(this,FLinearColor(.18f,.12f,.06f),X+12,Y+81,232,7);CozyDrawRect(this,FLinearColor(1,.8f,.4f),X+12,Y+81,232*Dig->StrokeProgress(),7);}
            else CozyDrawText(this,Dig->Covered(Patch)?TEXT("Dig again to recover the toy"):Depth==3?TEXT("Ready to bury a carried toy"):TEXT("F / RT-R2: dig deeper"),FLinearColor(.83f,.76f,.65f),X+12,Y+80,nullptr,.85f);
        }
        if(Show(ECozyOverlay::WorldLabels)&&Session->Feature(ECozyFeature::WorldLabels)&&!Session->Progress->Complete&&!Outside&&!Seasonal&&!(Mode->Hockey&&Mode->Hockey->Active())&&!(Mode->HomeProgression&&Mode->HomeProgression->InAnnex()))
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
    if(Session->Screen==ECozyScreen::Playing&&Mode->Neighbourhood&&!(Mode->Hockey&&Mode->Hockey->Active()))
    {
        DrawNavigation(false);
        const auto* PC=GetWorld()->GetFirstPlayerController();const auto* Pawn=PC?Cast<AMartyCharacter>(PC->GetPawn()):nullptr;
        if(Show(ECozyOverlay::Prompts)&&Pawn){const FString HomePrompt=Mode->HomeProgression?Mode->HomeProgression->Prompt():FString();const FString LifePrompt=!HomePrompt.IsEmpty()?HomePrompt:Pawn->Life?Pawn->Life->Prompt():FString();const FString CatPrompt=Mode->Companions?Mode->Companions->Prompt():FString();const FString AreaPrompt=Mode->HomeProgression&&Mode->HomeProgression->InAnnex()?TEXT("Garage + gym: use E / X-Square at a station or the return door"):Mode->Neighbourhood->Prompt(Pawn);CozyDrawText(this,!Session->Prompt().IsEmpty()?FString():!LifePrompt.IsEmpty()?LifePrompt:(AreaPrompt.StartsWith(TEXT("E /"))||AreaPrompt.StartsWith(TEXT("Baseball:")))?AreaPrompt:!CatPrompt.IsEmpty()?CatPrompt:AreaPrompt,FLinearColor(.8f,1,.8f),28,Height-90,nullptr,1.1f);}
        if(Show(ECozyOverlay::Hints)&&Session->Feature(ECozyFeature::Hints)&&(!Show(ECozyOverlay::TrickGuide)||!Pawn||!Pawn->TrickModifierVisible())){CozyDrawText(this,TEXT("Space / Y-Triangle: jump | F / RT: dig | Q / LT: drop | B / tap LB: bark"),FLinearColor(.8f,.9f,1),28,Height-145);
        CozyDrawText(this,TEXT("Map: M / D-pad Up | Crew: K / D-pad Down | Hold L1/LB: direct tricks | C: trick menu"),FLinearColor(.8f,.9f,1),28,Height-170);}
        if(Show(ECozyOverlay::Hints)&&Session->Feature(ECozyFeature::Hints)&&(!Pawn||!Pawn->TrickModifierVisible()))CozyDrawText(this,TEXT("Crawl: Z / LB-L3 | Two legs: U / LB-R3 | Solo Marty"),FLinearColor(.8f,.9f,1),28,Height-120);
    }
    if(Show(ECozyOverlay::Festival)&&Seasonal&&Session->Screen==ECozyScreen::Playing&&Mode->Festival){
        CozyDrawRect(this,FLinearColor(.055f,.018f,.085f,.94f),18,Height-245,510,52);
        CozyDrawText(this,FString::Printf(TEXT("BORK OR TREAT / PAW-PASS %d of 8%s"),Mode->Festival->StampCount(),Session->Progress->HalloweenBadge?TEXT(" / PUMPKIN PAL"):TEXT("")),FLinearColor(1,.75f,.35f),28,Height-235);
        for(int32 I=0;I<8;++I)CozyDrawRect(this,(Session->Progress->HalloweenStampMask&(1<<I))?FLinearColor(1,.58f,.15f):FLinearColor(.20f,.10f,.27f),28+I*58,Height-214,48,9);
    }
    const auto* TrickPC=GetWorld()->GetFirstPlayerController();const auto* TrickPet=TrickPC?Cast<AMartyCharacter>(TrickPC->GetPawn()):nullptr;
    if(Show(ECozyOverlay::TrickGuide)&&TrickPet&&TrickPet->TrickModifierVisible())
    {
        const float X=Width-500,Y=Height-214;CozyDrawRect(this,FLinearColor(.025f,.055f,.07f,.94f),X,Y,484,128);
        CozyDrawText(this,TEXT("DIRECT TRICKS | Hold L1 / LB or Left Ctrl"),FLinearColor(1,.86f,.5f),X+12,Y+10);
        CozyDrawText(this,Session->Feature(ECozyFeature::Tricks)?TEXT("Cross / A: lie or stand     Circle / B: roll over"):TEXT("Dog tricks are OFF in Controls > Pet feature switches"),FLinearColor::White,X+12,Y+34);
        CozyDrawText(this,Session->Feature(ECozyFeature::Tricks)?TEXT("Square / X: shake paw     Triangle / Y: bork!"):TEXT("Triangle / Y: bork (when Barking is enabled)"),FLinearColor::White,X+12,Y+56);
        CozyDrawText(this,TEXT("Keyboard: 1 lie / stand | 2 roll | 3 shake | 4 bark"),FLinearColor(.75f,.85f,.86f),X+12,Y+78);
        CozyDrawText(this,TEXT("L3: crawl | R3: two legs | Z / U on keyboard"),FLinearColor(.75f,.85f,.86f),X+12,Y+100);
    }
    if(Show(ECozyOverlay::Messages)&&Session->Screen==ECozyScreen::Playing&&!Session->Message.IsEmpty()) { CozyDrawRect(this,FLinearColor(.025f,.055f,.07f,.9f),16,Height-65,Width-32,48); CozyDrawText(this,Session->Message,FLinearColor::White,28,Height-49,nullptr,1.1f); }
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
