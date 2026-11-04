#include "ControllerGuide.h"
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
    auto Shot=[&](const TCHAR* Name){if(FApp::CanEverRender())FScreenshotRequest::RequestScreenshot(FPaths::ProjectSavedDir()/Name,true,false);};
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
        Shot(TEXT("ControllerGuideXbox.png"));Next();break;
    case 1:
        Input(TEXT("DualSense"),EKeys::Gamepad_LeftThumbstick);
        SmokeCheck(Guide->Family()==ECozyInputFamily::PlayStation,TEXT("Synthetic hot swap switches to PlayStation"));
        SmokeCheck(Guide->Controls()[1].Contains(TEXT("Triangle: jump")),TEXT("PlayStation legend matches gameplay face mapping"));
        Guide->Observe(EKeys::Gamepad_LeftX,INPUTDEVICEID_NONE,.1f);
        SmokeCheck(Guide->Family()==ECozyInputFamily::PlayStation,TEXT("Resting stick noise leaves device display stable"));
        Shot(TEXT("ControllerGuidePlayStation.png"));Next();break;
    case 2:
        Input(TEXT("Keyboard"),EKeys::LeftShift);
        SmokeCheck(Guide->Family()==ECozyInputFamily::Keyboard,TEXT("Synthetic keyboard input restores keyboard guide"));
        Shot(TEXT("ControllerGuideKeyboard.png"));Next();break;
    case 3:
        Input(TEXT("Unknown HID"),EKeys::Gamepad_LeftThumbstick);
        SmokeCheck(Guide->Family()==ECozyInputFamily::Generic,TEXT("Unknown controller gets positional face labels"));
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
        for(int I=0;I<5&&Guide->PreferenceLabel()!=Original;++I)Guide->CycleOverride();
        FInputDeviceRegistry::ClearSimulatedDescriptor(Device);
        {
            auto Report=MakeShared<FJsonObject>();auto Strings=[](const TArray<FString>& V){TArray<TSharedPtr<FJsonValue>> R;for(const auto& S:V)R.Add(MakeShared<FJsonValueString>(S));return R;};
            Report->SetArrayField(TEXT("passed"),Strings(SmokePassed));Report->SetArrayField(TEXT("failed"),Strings(SmokeFailed));
            Report->SetStringField(TEXT("scope"),TEXT("Real game map, simulated device descriptors and Slate events, synthetic keyboard menu selection. Physical controller identification and handling unverified."));
            FString Json;FJsonSerializer::Serialize(Report,TJsonWriterFactory<>::Create(&Json));
            const bool Saved=FFileHelper::SaveStringToFile(Json,*(FPaths::ProjectSavedDir()/TEXT("controller-guide-smoke.json")));
            SmokeStage=999;FPlatformMisc::RequestExitWithStatus(false,Saved&&SmokeFailed.IsEmpty()?0:1);
        }break;
    }
#endif
}
