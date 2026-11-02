#include "CozyModeHub.h"
#include "CozyAdventure.h"
#include "CozyCoop.h"
#include "MartyCharacter.h"
#include "HomeProgression.h"
#include "CatCompanions.h"
#include "Components/TextRenderComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "InputKeyEventArgs.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/Parse.h"
#include "Misc/CommandLine.h"
#include "Misc/App.h"
#include "UnrealClient.h"

// Opt-in real-map checks; never read/write the player's overlay preferences or checkpoint.
void UCozyModeHub::TickOverlayAcceptance(float Delta)
{
    if(!FParse::Param(FCommandLine::Get(),TEXT("CheweenieOverlaySmoke"))||TestStage==999)return;
    TestTime+=Delta;TestTotal+=Delta;
    auto* PC=UGameplayStatics::GetPlayerController(this,0);if(!PC)return;
    auto* S=Session();
    auto* Mode=GetWorld()->GetAuthGameMode<AMartyPlayMode>();
    auto ReactionVisible=[&](){if(!Mode||!Mode->Companions)return false;for(const auto& Cat:Mode->Companions->Cats)if(auto* A=Cat.Actor.Get()){TArray<UTextRenderComponent*> Labels;A->GetComponents(Labels);for(const auto* Label:Labels)if(Label->IsVisible())return true;}return false;};
    static FVector HiddenPosition;
    FString Root=FPaths::ProjectSavedDir();FParse::Value(FCommandLine::Get(),TEXT("CheweenieTestOutput="),Root);
    auto Check=[&](bool OK,const TCHAR* Label){(OK?TestPassed:TestFailed).Add(Label);};
    auto Next=[&](){++TestStage;TestTime=0;};
    auto Key=[&](FKey K,bool Down){PC->InputKey(FInputKeyEventArgs(nullptr,INPUTDEVICEID_NONE,K,Down?IE_Pressed:IE_Released,FPlatformTime::Cycles64()));};
    auto Shot=[&](const TCHAR* Name){if(FApp::CanEverRender())FScreenshotRequest::RequestScreenshot(Root/Name,true,false);};
    auto Finish=[&]()
    {
        auto Report=MakeShared<FJsonObject>();auto Strings=[](const TArray<FString>& Values){TArray<TSharedPtr<FJsonValue>> R;for(const auto& V:Values)R.Add(MakeShared<FJsonValueString>(V));return R;};
        Report->SetStringField(TEXT("scope"),TEXT("Real solo/room maps, synthetic keyboard/gamepad input, isolated preferences. Physical hardware unverified."));
        Report->SetArrayField(TEXT("passed"),Strings(TestPassed));Report->SetArrayField(TEXT("failed"),Strings(TestFailed));
        FString Json;FJsonSerializer::Serialize(Report,TJsonWriterFactory<>::Create(&Json));
        const bool Saved=FFileHelper::SaveStringToFile(Json,*(Root/TEXT("overlay-smoke.json")));TestStage=999;
        FPlatformMisc::RequestExitWithStatus(false,Saved&&TestFailed.IsEmpty()?0:1);
    };
    if(TestTotal>150){Check(false,TEXT("Overlay checks complete within time budget"));Finish();return;}
    if(TestTime<.4)return;
    switch(TestStage)
    {
    case 0:
        if(!S||!S->Progress||TestTime<(FApp::CanEverRender()?25:3))break;
        Check(S->Slot==TEXT("CheweenieOverlayAcceptance"),TEXT("Checkpoint is isolated"));
        if(FParse::Param(FCommandLine::Get(),TEXT("CheweenieOverlayResume")))
        {
            Check(!OverlayVisible(ECozyOverlay::All),TEXT("Master OFF survives a new process"));
            ToggleOverlay(ECozyOverlay::All);
            Check(!OverlayVisible(ECozyOverlay::Objective)&&OverlayVisible(ECozyOverlay::Health),TEXT("Individual choices survive a new process independently"));
            ResetOverlays();Finish();break;
        }
        ResetOverlays();
        for(int32 I=0;I<int32(ECozyOverlay::Count);++I)Check(OverlayVisible(ECozyOverlay(I)),TEXT("Default overlay is visible"));
        for(int32 I=1;I<int32(ECozyOverlay::Count);++I)
        {
            const auto Id=ECozyOverlay(I);ToggleOverlay(Id);
            bool Independent=!OverlayVisible(Id);for(int32 J=1;J<int32(ECozyOverlay::Count);++J)if(J!=I)Independent&=OverlayVisible(ECozyOverlay(J));
            Check(Independent,TEXT("Individual overlay switch leaves other overlays visible"));ToggleOverlay(Id);
        }
        S->Start(true);Shot(TEXT("OverlaysDefault.png"));Next();break;
    case 1:
        ToggleOverlay(ECozyOverlay::All);
        for(int32 I=0;I<int32(ECozyOverlay::Count);++I)Check(!OverlayVisible(ECozyOverlay(I)),TEXT("Master OFF hides every gameplay overlay"));
        Check(S->AllowsMovement()&&S->Progress->DisabledFeatures==0,TEXT("Hidden overlays leave movement and gameplay features enabled"));
        HiddenPosition=PC->GetPawn()->GetActorLocation();Key(EKeys::W,true);TestStage=100;TestTime=0;break;
    case 100:
        Key(EKeys::W,false);Check(FVector::Dist2D(HiddenPosition,PC->GetPawn()->GetActorLocation())>10,TEXT("Keyboard movement works with every overlay hidden"));
        Shot(TEXT("OverlaysHidden.png"));TestStage=101;TestTime=0;break;
    case 101:
        Key(EKeys::Escape,true);TestStage=2;TestTime=0;break;
    case 2:
        Key(EKeys::Escape,false);Check(S->Screen==ECozyScreen::Pause,TEXT("Keyboard pause remains available with master OFF"));
        S->Selection=2;S->Confirm();S->Selection=8;S->Confirm();
        Check(S->Screen==ECozyScreen::Modes&&Page==ECozyHubPage::Overlays,TEXT("Controls offers a direct screen-overlay entry"));
        S->Back();S->Back();S->Back();Check(S->Screen==ECozyScreen::Controls,TEXT("Backing out of overlay settings returns to Controls"));
        S->OpenModes();Page=ECozyHubPage::Graphics;Selection=3;S->Selection=3;S->Confirm();
        Check(Page==ECozyHubPage::Overlays,TEXT("Screen overlays is reachable through the shared settings menu"));
        Key(EKeys::Down,true);Next();break;
    case 3:
        Key(EKeys::Down,false);Check(Selection==1,TEXT("Keyboard arrows navigate overlay settings"));
        Key(EKeys::Gamepad_DPad_Down,true);Next();break;
    case 4:
        Key(EKeys::Gamepad_DPad_Down,false);Check(Selection==2,TEXT("Synthetic gamepad D-pad navigates overlay settings"));
        Selection=0;S->Selection=0;Key(EKeys::Gamepad_FaceButton_Bottom,true);Next();break;
    case 5:
        Key(EKeys::Gamepad_FaceButton_Bottom,false);Check(OverlayVisible(ECozyOverlay::All),TEXT("Synthetic A-Cross restores master visibility"));
        Selection=1;S->Selection=1;Key(EKeys::Right,true);Next();break;
    case 6:
    {
        Key(EKeys::Right,false);Check(!OverlayVisible(ECozyOverlay::Objective),TEXT("Keyboard adjustment toggles selected overlay"));
        const uint32 Expected=HiddenOverlays;HiddenOverlays=0;LoadOverlays();Check(HiddenOverlays==Expected,TEXT("Saved preferences reload exactly"));
        Shot(TEXT("OverlaySettings.png"));Next();break;
    }
    case 7:
        S->Start(false);S->Progress->PraiseCooldownRemaining=0;S->Progress->TrickPraiseMask=0;S->Progress->PraiseStars=0;
        PC->GetPawn()->SetActorLocation(Mode->Companions->Position(0)+FVector(40,0,0));
        ToggleOverlay(ECozyOverlay::Reactions);Mode->HomeProgression->OnSkill(0);
        Check(S->Progress->PraiseStars>0,TEXT("Hiding cat reactions preserves trick rewards"));
        Check(!ReactionVisible(),TEXT("Cat reaction switch hides actual text components"));
        ToggleOverlay(ECozyOverlay::Reactions);ToggleOverlay(ECozyOverlay::All);TestStage=102;TestTime=0;break;
    case 102:
        Check(!ReactionVisible(),TEXT("Master OFF keeps active cat reaction labels hidden"));
        ToggleOverlay(ECozyOverlay::All);TestStage=103;TestTime=0;break;
    case 103:
        Check(ReactionVisible(),TEXT("Restoring overlays reveals an unexpired cat reaction"));
        // The subsystem must retain presentation settings across a real room travel.
        S->Progress->RatchetUnlocked=true;S->Save();TestStage=8;TestTime=0;Request(TEXT("ratchet"));break;
    case 8:
        if(S||!Cast<ACozyCoopController>(PC))break;
        Check(!OverlayVisible(ECozyOverlay::Objective)&&OverlayVisible(ECozyOverlay::Health),TEXT("Preferences survive solo-to-room map travel"));
        CastChecked<ACozyCoopController>(PC)->SetHubMenu(true);Page=ECozyHubPage::Graphics;Selection=3;Confirm();
        Check(Page==ECozyHubPage::Overlays,TEXT("Room menu exposes the same local overlay settings"));
        Selection=int32(ECozyOverlay::PetNames);Confirm();Check(!OverlayVisible(ECozyOverlay::PetNames),TEXT("Room pet-name labels can be hidden independently"));
        Selection=int32(ECozyOverlay::Count);Confirm();Check(HiddenOverlays==0,TEXT("Restore defaults resets all overlay switches"));
        ToggleOverlay(ECozyOverlay::Objective);ToggleOverlay(ECozyOverlay::All);Finish();break;
    }
}
