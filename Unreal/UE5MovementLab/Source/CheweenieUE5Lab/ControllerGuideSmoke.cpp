#include "ControllerGuide.h"
#include "CozyModeHub.h"
#include "HAL/FileManager.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "CozyAdventure.h"
#include "MartyCharacter.h"
#include "Engine/GameInstance.h"
#include "GameFramework/PlayerController.h"
#include "Framework/Application/SlateApplication.h"
#include "GenericPlatform/InputDeviceRegistry.h"
#include "Input/Events.h"
#include "InputKeyEventArgs.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/App.h"
#include "UnrealClient.h"
#include "EngineUtils.h"
#include "Camera/CameraActor.h"
#include "GameFramework/HUD.h"
#include "Components/TextRenderComponent.h"

void AMartyPlayMode::TickControllerGuideSmoke(float Delta)
{
#if !UE_BUILD_SHIPPING
    SmokeTime+=Delta;
    auto* PC=GetWorld()->GetFirstPlayerController();
    if(!PC||!Adventure||!Adventure->Progress)return;
    auto* Guide=GetGameInstance()->GetSubsystem<UControllerGuide>();
    static FString Original;
    static FInputDeviceId Device=IPlatformInputDeviceMapper::Get().AllocateNewInputDeviceId();
    auto Next=[&](){++SmokeStage;SmokeTime=0;};
    auto Input=[&](const TCHAR* Hardware,FKey Key)
    {
        FInputDeviceDescriptor D;D.HardwareDeviceHandle=Device;D.InputDeviceName=FName(Hardware);D.HardwareDeviceIdentifier=FName(Hardware);
        FInputDeviceRegistry::SetSimulatedDescriptor(Device,D);
        FSlateApplication::Get().ProcessKeyDownEvent(FKeyEvent(Key,FModifierKeysState(),Device,false,0,0,0));
        FSlateApplication::Get().ProcessKeyUpEvent(FKeyEvent(Key,FModifierKeysState(),Device,false,0,0,0));
    };
    FString Root; if(!FParse::Value(FCommandLine::Get(),TEXT("CheweenieTestOutput="),Root))Root=FPaths::ProjectSavedDir(); IFileManager::Get().MakeDirectory(*Root,true);
    auto Shot=[&](const TCHAR* Name){if(FApp::CanEverRender())FScreenshotRequest::RequestScreenshot(Root/Name,true,false);};
    auto Help=[&]()
    {
        auto* Hub=GetGameInstance()->GetSubsystem<UCozyModeHub>();const auto Page=Hub->Page;
        Hub->Page=ECozyHubPage::Help;const FString Text=Guide->Format(FString::Join(Hub->Rows(),TEXT(" | ")));
        Hub->Page=Page;return Text;
    };
    auto HasSign=[&](const TCHAR* Text)
    {
        for(TActorIterator<AActor> It(GetWorld());It;++It)
        {
            TInlineComponentArray<UTextRenderComponent*> Labels;It->GetComponents(Labels);
            for(const auto* Label:Labels)if(Label->Text.ToString()==Text)return true;
        }
        return false;
    };
    if(SmokeTime<(SmokeStage==0?(FApp::CanEverRender()?25.f:3.f):1.f))return;
    switch(SmokeStage)
    {
    case 0:
        Original=Guide->PreferenceLabel();
        for(int I=0;I<5&&!Guide->PreferenceLabel().Contains(TEXT("Auto"));++I)Guide->CycleOverride();
        SmokeCheck(Guide->Classify(TEXT("XInputInterface"))==ECozyInputFamily::Xbox,TEXT("XInput is identified as Xbox"));
        SmokeCheck(Guide->Classify(TEXT("DualSense Sony"))==ECozyInputFamily::PlayStation,TEXT("DualSense is identified as PlayStation"));
        SmokeCheck(Guide->Classify(TEXT("DualShock4"))==ECozyInputFamily::PlayStation,TEXT("DualShock is identified as PlayStation"));
        SmokeCheck(Guide->Classify(TEXT("Unknown HID"))==ECozyInputFamily::Generic,TEXT("Unknown hardware is not guessed as Xbox"));
        Adventure->OpenControlsFromModes();Input(TEXT("XInputInterface"),EKeys::Gamepad_LeftThumbstick);
        SmokeCheck(Guide->Family()==ECozyInputFamily::Xbox,TEXT("Synthetic Slate Xbox event switches display"));
        SmokeCheck(Guide->Controls()[1].Contains(TEXT("X: interact")),TEXT("Xbox legend matches gameplay face mapping"));
        SmokeCheck(Guide->Format(TEXT("1 / 9 | Arrows choose | Enter select | Backspace back"))==TEXT("1 / 9 | LS/D-pad choose | A select | B back"),TEXT("Screenshot footer switches to Xbox without changing row counts"));
        SmokeCheck(Guide->Format(TEXT("C: trick menu"))==TEXT("B: trick menu"),TEXT("Xbox trick hint removes keyboard-only C"));
        SmokeCheck(!Help().Contains(TEXT("WASD"))&&!Help().Contains(TEXT("arrows"))&&!Help().Contains(TEXT("Left Ctrl"))&&!Help().Contains(TEXT("brackets"))&&!Help().Contains(TEXT("Wheel"))&&Help().Contains(TEXT("hold LB + A: lie")),TEXT("Xbox Help page removes keyboard-only instructions"));
        Shot(TEXT("ControllerGuideXbox.png"));Next();break;
    case 1:
        Input(TEXT("DualSense"),EKeys::Gamepad_LeftThumbstick);
        SmokeCheck(Guide->Family()==ECozyInputFamily::PlayStation,TEXT("Synthetic hot swap switches to PlayStation"));
        SmokeCheck(Guide->Controls()[1].Contains(TEXT("Triangle: jump")),TEXT("PlayStation legend matches gameplay face mapping"));
        SmokeCheck(Guide->Format(TEXT("Arrows choose | Enter select | Backspace back"))==TEXT("LS/D-pad choose | Cross select | Circle back"),TEXT("Screenshot footer switches to PlayStation"));
        SmokeCheck(Guide->Format(TEXT("WASD / LS | mouse / RS | B / LB-L1 | R / RB-R1"))==TEXT("LS | RS | L1 | R1"),TEXT("LAN hints use PlayStation controls without keyboard keys"));
        Input(TEXT("Keyboard"),FKey(FName(TEXT("PrintScreen"))));
        SmokeCheck(Guide->Family()==ECozyInputFamily::PlayStation,TEXT("Print Screen preserves controller prompts"));
        Input(TEXT("Keyboard"),EKeys::LeftCommand);
        SmokeCheck(Guide->Family()==ECozyInputFamily::PlayStation,TEXT("Windows shortcut key preserves controller prompts"));
        Guide->Observe(EKeys::Gamepad_LeftX,INPUTDEVICEID_NONE,.1f);
        SmokeCheck(Guide->Family()==ECozyInputFamily::PlayStation,TEXT("Resting stick noise leaves device display stable"));
        SmokeCheck(Help().Contains(TEXT("hold L1 + Cross: lie"))&&Help().Contains(TEXT("Circle: roll"))&&!Help().Contains(TEXT("Left Ctrl"))&&!Help().Contains(TEXT("Mouse")),TEXT("PlayStation Help page uses matching buttons"));
        Shot(TEXT("ControllerGuidePlayStation.png"));Next();break;
    case 2:
        Input(TEXT("Keyboard"),EKeys::LeftShift);
        SmokeCheck(Guide->Family()==ECozyInputFamily::Keyboard,TEXT("Synthetic keyboard input restores keyboard guide"));
        SmokeCheck(Guide->Format(TEXT("Arrows choose | Enter select | Backspace back"))==TEXT("Arrows choose | Enter select | Backspace back"),TEXT("Screenshot footer restores keyboard wording"));
        SmokeCheck(Help().Contains(TEXT("hold Left Ctrl + 1: lie"))&&Help().Contains(TEXT("2: roll"))&&!Help().Contains(TEXT("Enter: lie")),TEXT("Keyboard Help page uses actual direct-trick number keys"));
        Shot(TEXT("ControllerGuideKeyboard.png"));Next();break;
    case 3:
        Input(TEXT("Unknown HID"),EKeys::Gamepad_LeftThumbstick);
        SmokeCheck(Guide->Family()==ECozyInputFamily::Generic,TEXT("Unknown controller gets positional face labels"));
        SmokeCheck(Guide->Format(TEXT("Arrows choose | Enter select | Backspace back"))==TEXT("LS/D-pad choose | Bottom select | Right back"),TEXT("Screenshot footer uses neutral unknown-controller names"));
        Adventure->Selection=9;
        PC->InputKey(FInputKeyEventArgs(nullptr,INPUTDEVICEID_NONE,EKeys::Enter,IE_Pressed,FPlatformTime::Cycles64()));
        Next();break;
    case 4:
        PC->InputKey(FInputKeyEventArgs(nullptr,INPUTDEVICEID_NONE,EKeys::Enter,IE_Released,FPlatformTime::Cycles64()));
        SmokeCheck(Guide->Family()==ECozyInputFamily::Xbox,TEXT("Keyboard selects manual Xbox display in real controls menu"));
        Input(TEXT("DualSense"),EKeys::Gamepad_LeftThumbstick);
        SmokeCheck(Guide->Family()==ECozyInputFamily::Xbox,TEXT("Manual override survives conflicting detected hardware"));
        Guide->CycleOverride();
        SmokeCheck(Guide->Format(TEXT("X-Square / Y-Triangle / L1/LB"))==TEXT("Square / Triangle / L1"),TEXT("PlayStation gameplay hints use matching button names"));
        for(int I=0;I<5&&!Guide->PreferenceLabel().Contains(TEXT("Auto"));++I)Guide->CycleOverride();
        Adventure->Screen=ECozyScreen::Pause;Adventure->Selection=0;
        Input(TEXT("XInputInterface"),EKeys::Gamepad_LeftThumbstick);
        Next();break;
    case 5:
        SmokeCheck(HasSign(TEXT("STRENGTH\nX then shake a paw")),TEXT("Gym strength sign switches to Xbox while paused"));
        SmokeCheck(HasSign(TEXT("AGILITY\nX then roll over")),TEXT("Gym agility sign switches to Xbox while paused"));
        Shot(TEXT("ControllerGuidePauseXbox.png"));Next();break;
    case 6:
        Input(TEXT("DualSense"),EKeys::Gamepad_LeftThumbstick);
        Next();break;
    case 7:
        SmokeCheck(HasSign(TEXT("STRENGTH\nSquare then shake a paw")),TEXT("Gym sign follows PlayStation swap while paused"));
        SmokeCheck(HasSign(TEXT("SPEED\nSquare then run to the gold finish")),TEXT("Gym speed sign uses PlayStation interaction"));
        Shot(TEXT("ControllerGuidePausePlayStation.png"));Next();break;
    case 8:
        Input(TEXT("Keyboard"),EKeys::LeftShift);
        Next();break;
    case 9:
        SmokeCheck(HasSign(TEXT("STRENGTH\nE then shake a paw")),TEXT("Gym sign restores keyboard after controller while paused"));
        SmokeCheck(HasSign(TEXT("GARAGE + GYM\nE")),TEXT("Door sign restores keyboard interaction"));
        Shot(TEXT("ControllerGuidePauseKeyboard.png"));Next();break;
    case 10:
        Input(TEXT("DualSense"),EKeys::Gamepad_LeftThumbstick);
        Adventure->Screen=ECozyScreen::Playing;
        if(auto* HUD=PC->GetHUD())HUD->bShowHUD=false;
        {
            const FVector Eye(8150,0,240),Focus(8600,0,80);
            auto* Camera=GetWorld()->SpawnActor<ACameraActor>(Eye,(Focus-Eye).Rotation());
            if(Camera)PC->SetViewTarget(Camera);
        }
        Next();break;
    case 11:
        Shot(TEXT("ControllerGuideGymPlayStation.png"));Next();break;
    case 12:
        Input(TEXT("XInputInterface"),EKeys::Gamepad_LeftThumbstick);
        Adventure->Screen=ECozyScreen::Modes;Adventure->Selection=0;
        GetGameInstance()->GetSubsystem<UCozyModeHub>()->Page=ECozyHubPage::Help;
        if(auto* HUD=PC->GetHUD())HUD->bShowHUD=true;
        Next();break;
    case 13:
        Shot(TEXT("ControllerGuideHelpXbox.png"));Next();break;
    case 14:
        for(int I=0;I<5&&Guide->PreferenceLabel()!=Original;++I)Guide->CycleOverride();
        FInputDeviceRegistry::ClearSimulatedDescriptor(Device);
        {
            auto Report=MakeShared<FJsonObject>();auto Strings=[](const TArray<FString>& V){TArray<TSharedPtr<FJsonValue>> R;for(const auto& S:V)R.Add(MakeShared<FJsonValueString>(S));return R;};
            Report->SetArrayField(TEXT("passed"),Strings(SmokePassed));Report->SetArrayField(TEXT("failed"),Strings(SmokeFailed));
            Report->SetStringField(TEXT("scope"),TEXT("Real game map, simulated device descriptors and Slate events, synthetic keyboard menu selection. Physical controller identification and handling unverified."));
            FString Json;FJsonSerializer::Serialize(Report,TJsonWriterFactory<>::Create(&Json));
            const bool Saved=FFileHelper::SaveStringToFile(Json,*(Root/TEXT("controller-guide-smoke.json")));
            SmokeStage=999;FPlatformMisc::RequestExitWithStatus(false,Saved&&SmokeFailed.IsEmpty()?0:1);
        }break;
    }
#endif
}
