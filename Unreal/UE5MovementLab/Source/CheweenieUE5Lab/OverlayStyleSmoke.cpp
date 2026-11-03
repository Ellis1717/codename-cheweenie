#include "CozyModeHub.h"
#include "CozyAdventure.h"
#include "CozyCoop.h"
#include "MartyCharacter.h"
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

void UCozyModeHub::TickStyleAcceptance(float Delta)
{
    TestTime+=Delta;TestTotal+=Delta;
    auto* PC=UGameplayStatics::GetPlayerController(this,0);if(!PC)return;auto* S=Session();
    FString Root=FPaths::ProjectSavedDir();FParse::Value(FCommandLine::Get(),TEXT("CheweenieTestOutput="),Root);
    auto Check=[&](bool OK,const TCHAR* Label){(OK?TestPassed:TestFailed).Add(Label);};
    auto Next=[&](){++TestStage;TestTime=0;};
    auto Key=[&](FKey K,bool Down){PC->InputKey(FInputKeyEventArgs(nullptr,INPUTDEVICEID_NONE,K,Down?IE_Pressed:IE_Released,FPlatformTime::Cycles64()));};
    auto Shot=[&](const TCHAR* Kind){if(FApp::CanEverRender())FScreenshotRequest::RequestScreenshot(Root/FString::Printf(TEXT("Style%d%s.png"),HUDStyle,Kind),true,false);};
    auto Menu=[&](){S->OpenModes();Page=ECozyHubPage::Graphics;Selection=4;S->Selection=4;};
    auto Finish=[&]()
    {
        auto Report=MakeShared<FJsonObject>();auto Strings=[](const TArray<FString>& Values){TArray<TSharedPtr<FJsonValue>> R;for(const auto& V:Values)R.Add(MakeShared<FJsonValueString>(V));return R;};
        Report->SetArrayField(TEXT("passed"),Strings(TestPassed));Report->SetArrayField(TEXT("failed"),Strings(TestFailed));Report->SetStringField(TEXT("scope"),TEXT("Real solo/room maps; synthetic keyboard, gamepad and HUD mouse hitbox input; isolated preferences/save and rendering when requested. Physical hardware unverified."));
        FString Json;FJsonSerializer::Serialize(Report,TJsonWriterFactory<>::Create(&Json));
        const bool Saved=FFileHelper::SaveStringToFile(Json,*(Root/TEXT("overlay-styles.json")));TestStage=999;FPlatformMisc::RequestExitWithStatus(false,Saved&&TestFailed.IsEmpty()?0:1);
    };
    if(TestTotal>150){Check(false,TEXT("Style acceptance completed within time budget"));Finish();return;}
    if(TestTime<.5)return;
    switch(TestStage)
    {
    case 0:
        if(!S||!S->Progress||TestTime<(FApp::CanEverRender()?25:3))break;
        Check(S->Slot==TEXT("CheweenieOverlayAcceptance"),TEXT("Style acceptance uses an isolated checkpoint"));
        if(FParse::Param(FCommandLine::Get(),TEXT("CheweenieOverlayStyleResume"))){Check(HUDStyle==3,TEXT("Chosen style survives a second process"));Check(!OverlayVisible(ECozyOverlay::Objective),TEXT("Visibility choice survives with the style"));ResetOverlays();Check(HUDStyle==0&&HiddenOverlays==0,TEXT("Restore defaults returns to Classic Teal and visible overlays"));Finish();break;}
        ResetOverlays();Check(HUDStyle==0&&OverlayStyleName()==TEXT("Classic Teal"),TEXT("Classic Teal remains the default"));S->Start(false);Next();break;
    case 1:if(TestTime<3)break;Shot(TEXT("Game"));Next();break;
    case 2:Menu();Check(CanAdjust(4)&&Rows()[4].Contains(TEXT("Classic Teal")),TEXT("Graphics exposes a named adjustable style row"));Shot(TEXT("Menu"));Next();break;
    case 3:Key(EKeys::Right,true);Next();break;
    case 4:Key(EKeys::Right,false);Check(HUDStyle==1,TEXT("Keyboard right selects Cozy Paper live"));S->Start(false);Shot(TEXT("Game"));Next();break;
    case 5:Menu();Shot(TEXT("Menu"));Next();break;
    case 6:Key(EKeys::Gamepad_DPad_Right,true);Next();break;
    case 7:Key(EKeys::Gamepad_DPad_Right,false);Check(HUDStyle==2,TEXT("Gamepad D-pad selects Midnight Pop live"));S->Start(false);Shot(TEXT("Game"));Next();break;
    case 8:Menu();Shot(TEXT("Menu"));Next();break;
    case 9:
        CastChecked<AMartyHUD>(PC->GetHUD())->NotifyHitBoxClick(TEXT("CozyPlus_4"));Check(HUDStyle==3,TEXT("Mouse plus hitbox selects Clean Slate"));S->Start(false);Shot(TEXT("Game"));Next();break;
    case 10:Menu();Shot(TEXT("Menu"));Next();break;
    case 11:
        HUDStyle=0;LoadOverlays();Check(HUDStyle==3,TEXT("Saved style reloads exactly"));ToggleOverlay(ECozyOverlay::Objective);AdjustOverlayStyle(1);Check(HUDStyle==0&&!OverlayVisible(ECozyOverlay::Objective),TEXT("Forward wrap preserves visibility choices"));AdjustOverlayStyle(-1);Check(HUDStyle==3,TEXT("Backward wrap returns to Clean Slate"));
        S->Progress->RatchetUnlocked=true;S->Save();Next();Request(TEXT("ratchet"));break;
    case 12:
        if(S||!Cast<ACozyCoopController>(PC))break;
        Check(HUDStyle==3&&!OverlayVisible(ECozyOverlay::Objective),TEXT("Style and visibility survive actual room travel"));
        CastChecked<ACozyCoopController>(PC)->SetHubMenu(true);Page=ECozyHubPage::Graphics;Selection=4;Confirm();Check(HUDStyle==0,TEXT("Room menu cycles the same local style"));AdjustOverlayStyle(-1);Finish();break;
    }
}
