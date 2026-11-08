#include "MartyCharacter.h"
#include "CozyAdventure.h"
#include "DogLife.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputKeyEventArgs.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/App.h"
#include "UnrealClient.h"

void AMartyPlayMode::TickCameraSmoke(float Delta)
{
#if !UE_BUILD_SHIPPING
    SmokeTime+=Delta;
    auto* PC=GetWorld()->GetFirstPlayerController();
    auto* P=PC?Cast<AMartyCharacter>(PC->GetPawn()):nullptr;
    if(!P||!Adventure||!Adventure->Progress)return;
    auto* Boom=P->FindComponentByClass<USpringArmComponent>();
    auto Key=[&](FKey K,bool Down){PC->InputKey(FInputKeyEventArgs(nullptr,INPUTDEVICEID_NONE,K,Down?IE_Pressed:IE_Released,FPlatformTime::Cycles64()));};
    auto Next=[&](){++SmokeStage;SmokeTime=0;};
    auto Shot=[&](const TCHAR* Name){if(FApp::CanEverRender())FScreenshotRequest::RequestScreenshot(FPaths::ProjectSavedDir()/Name,true,false);};
    if(SmokeTime<(SmokeStage==0?(FApp::CanEverRender()?30.f:3.f):.7f))return;
    switch(SmokeStage)
    {
    case 0:
        Adventure->Start(false);Adventure->Progress->DisabledFeatures=0;
        SmokeCheck(P->CameraPreset==0,TEXT("Default is cozy follow"));
        Key(EKeys::MouseScrollUp,true);Next();break;
    case 1:
        Key(EKeys::MouseScrollUp,false);
        SmokeCheck(FMath::IsNearlyEqual(Boom->TargetArmLength,190.f,1.f),TEXT("Mouse wheel zooms in smoothly by 30 cm"));
        Key(EKeys::MouseScrollDown,true);Next();break;
    case 2:
        Key(EKeys::MouseScrollDown,false);
        SmokeCheck(FMath::IsNearlyEqual(Boom->TargetArmLength,220.f,1.f),TEXT("Mouse wheel zooms out"));
        Key(EKeys::Equals,true);Next();break;
    case 3:
        Key(EKeys::Equals,false);
        SmokeCheck(Boom->TargetArmLength<150.f,TEXT("Held plus continuously zooms in"));
        Key(EKeys::Hyphen,true);Next();break;
    case 4:
        Key(EKeys::Hyphen,false);
        SmokeCheck(Boom->TargetArmLength>150.f,TEXT("Held minus continuously zooms out"));
        Key(EKeys::Backslash,true);Next();break;
    case 5:
        Key(EKeys::Backslash,false);
        SmokeCheck(FMath::IsNearlyEqual(Boom->TargetArmLength,220.f,1.f),TEXT("Backslash restores this mode's default zoom"));
        Key(EKeys::Gamepad_DPad_Right,true);Next();break;
    case 6:
        Key(EKeys::Gamepad_DPad_Right,false);
        SmokeCheck(FMath::IsNearlyEqual(Boom->TargetArmLength,190.f,1.f),TEXT("Controller D-pad right zooms in"));
        Key(EKeys::Gamepad_DPad_Left,true);Next();break;
    case 7:
        Key(EKeys::Gamepad_DPad_Left,false);
        SmokeCheck(FMath::IsNearlyEqual(Boom->TargetArmLength,220.f,1.f),TEXT("Controller D-pad left zooms out"));
        P->ZoomCamera(10000);Next();break;
    case 8:
        SmokeCheck(FMath::IsNearlyEqual(Boom->TargetArmLength,80.f,1.f),TEXT("Close zoom clamps before entering Marty"));
        P->ZoomCamera(-10000);Next();break;
    case 9:
        SmokeCheck(FMath::IsNearlyEqual(Boom->TargetArmLength,650.f,1.f),TEXT("Far zoom clamps at 650 cm"));
        P->ResetCameraZoom();P->ZoomCamera(30);P->CycleCamera();
        SmokeCheck(P->CameraPreset==1&&Boom->TargetArmLength==330,TEXT("Wide follow preserves original preset"));
        P->CycleCamera();SmokeCheck(P->CameraPreset==2&&Boom->TargetArmLength==140,TEXT("Close follow preserves original preset"));
        P->CycleCamera();SmokeCheck(P->CameraPreset==3&&Boom->TargetArmLength==120,TEXT("Portrait preserves original preset"));
        Key(EKeys::V,true);Next();break;
    case 10:
        Key(EKeys::V,false);
        SmokeCheck(P->CameraPreset==4&&P->CameraName()==TEXT("High overview")&&FMath::IsNearlyEqual(FRotator::NormalizeAxis(PC->GetControlRotation().Pitch),-50.f,1.f),TEXT("V selects pitched high overview"));
        Shot(TEXT("CameraOverview.png"));Next();break;
    case 11:
        Key(EKeys::Gamepad_RightThumbstick,true);Next();break;
    case 12:
        Key(EKeys::Gamepad_RightThumbstick,false);
        SmokeCheck(P->CameraPreset==5&&P->CameraName()==TEXT("Dog level")&&FMath::IsNearlyEqual(FRotator::NormalizeAxis(PC->GetControlRotation().Pitch),-5.f,1.f),TEXT("R3 selects low dog-level view"));
        Shot(TEXT("CameraDogLevel.png"));Next();break;
    case 13:
        P->CycleCamera();SmokeCheck(P->CameraPreset==0&&Boom->TargetArmLength==190,TEXT("Six views wrap and remember each mode's zoom during play"));
        Adventure->Pause();P->ZoomCamera(30);P->CycleCamera();Key(EKeys::Equals,true);Next();break;
    case 14:
        Key(EKeys::Equals,false);
        SmokeCheck(P->CameraPreset==0&&FMath::IsNearlyEqual(Boom->TargetArmLength,190.f,1.f),TEXT("Paused menu blocks zoom and cycling"));
        Adventure->Back();Adventure->ToggleFeature(ECozyFeature::Cameras);P->ZoomCamera(30);P->CycleCamera();Next();break;
    case 15:
        SmokeCheck(P->CameraPreset==0&&FMath::IsNearlyEqual(Boom->TargetArmLength,190.f,1.f),TEXT("Camera feature switch blocks zoom and cycling"));
        Adventure->ToggleFeature(ECozyFeature::Cameras);P->ResetCameraZoom();Next();break;
    case 16:
        Shot(TEXT("CameraFollow.png"));Next();break;
    case 17:
        {
            auto Report=MakeShared<FJsonObject>();auto Strings=[](const TArray<FString>& V){TArray<TSharedPtr<FJsonValue>> R;for(const auto& S:V)R.Add(MakeShared<FJsonValueString>(S));return R;};
            Report->SetArrayField(TEXT("passed"),Strings(SmokePassed));Report->SetArrayField(TEXT("failed"),Strings(SmokeFailed));
            Report->SetStringField(TEXT("scope"),TEXT("Real game map with synthetic keyboard, mouse-wheel and gamepad events. Physical controllers and LAN zoom unverified."));
            FString Json;FJsonSerializer::Serialize(Report,TJsonWriterFactory<>::Create(&Json));
            const bool Saved=FFileHelper::SaveStringToFile(Json,*(FPaths::ProjectSavedDir()/TEXT("camera-smoke.json")));
            SmokeStage=999;FPlatformMisc::RequestExitWithStatus(false,Saved&&SmokeFailed.IsEmpty()?0:1);
        }break;
    }
#endif
}
