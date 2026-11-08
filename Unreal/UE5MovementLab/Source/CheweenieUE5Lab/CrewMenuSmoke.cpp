#include "MartyCharacter.h"
#include "CozyAdventure.h"
#include "CozyModeHub.h"
#include "Engine/GameInstance.h"
#include "GameFramework/PlayerController.h"
#include "InputKeyEventArgs.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/App.h"
#include "UnrealClient.h"

void AMartyPlayMode::TickCrewMenuSmoke(float Delta)
{
#if !UE_BUILD_SHIPPING
    SmokeTime+=Delta;
    auto* PC=GetWorld()->GetFirstPlayerController();if(!PC||!Adventure||!Adventure->Progress)return;
    auto* H=GetGameInstance()->GetSubsystem<UCozyModeHub>();auto* A=Adventure.Get();
    auto Key=[&](FKey K,bool Down){PC->InputKey(FInputKeyEventArgs(nullptr,INPUTDEVICEID_NONE,K,Down?IE_Pressed:IE_Released,FPlatformTime::Cycles64()));};
    auto Shot=[&](const TCHAR* Name){if(FApp::CanEverRender())FScreenshotRequest::RequestScreenshot(FPaths::ProjectSavedDir()/Name,true,false);};
    auto Next=[&](){++SmokeStage;SmokeTime=0;};
    if(SmokeTime<(SmokeStage==0?(FApp::CanEverRender()?25.f:3.f):1.f))return;
    switch(SmokeStage)
    {
    case 0:
        SmokeCheck(A->Slot==TEXT("CheweenieCrewMenuAcceptance"),TEXT("Menu acceptance uses isolated progress"));
        SmokeCheck(A->IllustratedMenus,TEXT("Startup uses illustrated menus"));
        A->OpenModes();SmokeCheck(H->Rows().Last().Contains(TEXT("Character bios")),TEXT("Main menu exposes Character bios without replacing older choices"));
        A->Selection=8;Key(EKeys::Enter,true);Next();break;
    case 1:
        Key(EKeys::Enter,false);SmokeCheck(H->Page==ECozyHubPage::Crew,TEXT("Keyboard opens the Character bios section"));
        SmokeCheck(H->Rows().Num()==4,TEXT("Crew section has one entry for each pet and Back"));
        SmokeCheck(H->MenuBackdrop()!=nullptr,TEXT("New sofa transition art loads in the crew section"));
        Shot(TEXT("CrewSection.png"));Key(EKeys::Gamepad_DPad_Down,true);Next();break;
    case 2:
        Key(EKeys::Gamepad_DPad_Down,false);SmokeCheck(A->Selection==1,TEXT("Synthetic D-pad selects Lakitu"));
        Key(EKeys::Gamepad_FaceButton_Bottom,true);Next();break;
    case 3:
        Key(EKeys::Gamepad_FaceButton_Bottom,false);SmokeCheck(H->Page==ECozyHubPage::Biography&&H->CrewIndex==1,TEXT("Synthetic A-Cross opens Lakitu's own bio page"));
        SmokeCheck(H->CrewPortrait()!=nullptr,TEXT("Lakitu portrait asset loads"));Shot(TEXT("CrewLakitu.png"));Next();break;
    case 4:
        Key(EKeys::Right,true);Next();break;
    case 5:
        Key(EKeys::Right,false);SmokeCheck(H->CrewIndex==2,TEXT("Keyboard right switches from Lakitu to Rio"));
        SmokeCheck(H->CrewPortrait()!=nullptr,TEXT("Rio portrait asset loads"));Shot(TEXT("CrewRio.png"));Next();break;
    case 6:
        H->Adjust(1);SmokeCheck(H->CrewIndex==0,TEXT("Next character wraps Rio to Marty"));
        SmokeCheck(H->CrewPortrait()!=nullptr,TEXT("Marty portrait asset loads"));Shot(TEXT("CrewMarty.png"));Next();break;
    case 7:
        if(auto* HUD=PC->GetHUD())HUD->NotifyHitBoxClick(TEXT("CozyRow_2"));
        SmokeCheck(H->Page==ECozyHubPage::Crew&&A->Selection==0,TEXT("Mouse bio Back restores the selected character"));
        A->Back();SmokeCheck(H->Page==ECozyHubPage::Modes&&A->Selection==8,TEXT("Back from crew restores the bios entry in the main menu"));
        A->Start(false);SmokeCheck(!A->IllustratedMenus,TEXT("Starting patrol switches menu backgrounds to the game scene"));
        Key(EKeys::Escape,true);Next();break;
    case 8:
        Key(EKeys::Escape,false);SmokeCheck(A->Screen==ECozyScreen::Pause&&!A->IllustratedMenus,TEXT("Keyboard Escape opens pause over the actual world"));
        Shot(TEXT("CrewPause.png"));Next();break;
    case 9:
        A->Selection=9;A->Confirm();SmokeCheck(H->Page==ECozyHubPage::Crew&&!A->IllustratedMenus,TEXT("Pause opens character bios while retaining world context"));
        A->Back();A->Back();SmokeCheck(A->Screen==ECozyScreen::Pause,TEXT("Closing pause bios returns to pause rather than title"));
        A->Back();A->OpenBackpack();SmokeCheck(A->Screen==ECozyScreen::Backpack&&!A->IllustratedMenus,TEXT("Backpack uses an in-world menu"));
        Shot(TEXT("CrewBackpack.png"));Next();break;
    case 10:
        A->Back();A->OpenCompanions();A->Selection=7;A->Confirm();SmokeCheck(H->Page==ECozyHubPage::Crew,TEXT("Companion actions provide a route to individual bios"));
        A->Back();A->Back();SmokeCheck(A->Screen==ECozyScreen::Companions,TEXT("Companion bios return to the companion menu"));
        A->Back();A->Pause();A->Selection=2;A->Confirm();SmokeCheck(A->Screen==ECozyScreen::Controls&&!A->IllustratedMenus,TEXT("Dense controls keep the actual game underneath"));
        Shot(TEXT("CrewControls.png"));Next();break;
    case 11:
        A->Back();A->Selection=3;A->Confirm();SmokeCheck(A->Screen==ECozyScreen::Settings,TEXT("Sound settings remain reachable from pause"));
        SmokeYaw=A->Progress->Music;A->Selection=0;A->Adjust(1);SmokeCheck(A->Progress->Music>SmokeYaw,TEXT("Sound adjustment still works through the new menu"));
        Shot(TEXT("CrewSound.png"));Next();break;
    case 12:
        A->Back();A->Selection=4;A->Confirm();SmokeCheck(A->Screen==ECozyScreen::Title&&A->IllustratedMenus,TEXT("Return to title restores illustrated menu context"));
        A->OpenModes();H->Page=ECozyHubPage::Graphics;A->Selection=0;
        SmokeCheck(H->MenuBackdrop()!=nullptr,TEXT("Rainy-window transition art loads for illustrated utility menus"));Shot(TEXT("CrewGraphics.png"));Next();break;
    case 13:
        H->Page=ECozyHubPage::Archives;SmokeCheck(H->MenuBackdrop()!=nullptr,TEXT("Bedtime transition art loads for the archive menu"));
        H->Page=ECozyHubPage::Host;SmokeCheck(H->MenuBackdrop()!=nullptr,TEXT("Fort transition art loads for the host menu"));
        H->Page=ECozyHubPage::Modes;A->Selection=8;Shot(TEXT("CrewMainMenu.png"));Next();break;
    case 14:
        {
            auto Report=MakeShared<FJsonObject>();auto Strings=[](const TArray<FString>& V){TArray<TSharedPtr<FJsonValue>> R;for(const auto& S:V)R.Add(MakeShared<FJsonValueString>(S));return R;};
            Report->SetArrayField(TEXT("passed"),Strings(SmokePassed));Report->SetArrayField(TEXT("failed"),Strings(SmokeFailed));
            Report->SetStringField(TEXT("scope"),TEXT("Real map; keyboard, synthetic gamepad and mouse-handler events. Physical controllers unverified. No package update."));
            FString Json;FJsonSerializer::Serialize(Report,TJsonWriterFactory<>::Create(&Json));
            const bool Saved=FFileHelper::SaveStringToFile(Json,*(FPaths::ProjectSavedDir()/TEXT("crew-menu-smoke.json")));
            SmokeStage=999;FPlatformMisc::RequestExitWithStatus(false,Saved&&SmokeFailed.IsEmpty()?0:1);
        }break;
    }
#endif
}
