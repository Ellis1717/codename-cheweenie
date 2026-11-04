#include "MartyCharacter.h"
#include "ControllerComfort.h"
#include "CozyAdventure.h"
#include "DogLife.h"
#include "ControllerGuide.h"
#include "CozyModeHub.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/GameInstance.h"
#include "InputKeyEventArgs.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/App.h"
#include "Misc/CoreDelegates.h"
#include "UnrealClient.h"

void AMartyPlayMode::TickControllerComfortSmoke(float Delta)
{
#if !UE_BUILD_SHIPPING
    SmokeTime+=Delta;auto* PC=GetWorld()->GetFirstPlayerController();auto* P=PC?Cast<AMartyCharacter>(PC->GetPawn()):nullptr;
    if(!P||!Adventure||!Adventure->Progress||!P->Life)return;
    auto* S=Adventure.Get();auto* D=S->Progress.Get();auto* L=P->Life.Get();auto* Pref=GetGameInstance()->GetSubsystem<UControllerComfort>();
    auto* Boom=P->FindComponentByClass<USpringArmComponent>();
    const bool Resume=FParse::Param(FCommandLine::Get(),TEXT("CheweenieControllerComfortResume"));
    FString Root=FPaths::ProjectSavedDir();FParse::Value(FCommandLine::Get(),TEXT("CheweenieTestOutput="),Root);
    static FVector4 Axes(0,0,0,0);static FVector Start;static FRotator View;static float Music=0;
    const auto A=EKeys::Gamepad_FaceButton_Bottom,B=EKeys::Gamepad_FaceButton_Right,L3=EKeys::Gamepad_LeftThumbstick,Menu=EKeys::Gamepad_Special_Right;
    auto Key=[&](FKey K,bool Down){PC->InputKey(FInputKeyEventArgs(nullptr,INPUTDEVICEID_NONE,K,Down?IE_Pressed:IE_Released,FPlatformTime::Cycles64()));};
    auto Axis=[&](FKey K,float V){PC->InputKey(FInputKeyEventArgs(nullptr,INPUTDEVICEID_NONE,K,V,Delta,1,FPlatformTime::Cycles64()));};
    Axis(EKeys::Gamepad_LeftX,Axes.X);Axis(EKeys::Gamepad_LeftY,Axes.Y);Axis(EKeys::Gamepad_RightX,Axes.Z);Axis(EKeys::Gamepad_RightY,Axes.W);
    TArray<TPair<float,TFunction<void()>>> Steps;
    auto Add=[&](float Wait,TFunction<void()> Run){Steps.Emplace(Wait,MoveTemp(Run));};
    auto Tap=[&](FKey K,TFunction<void()> Check,float Hold=.18f){Add(.15f,[&,K](){Key(K,true);});Add(Hold,[&,K,Check=MoveTemp(Check)](){Check();Key(K,false);});Add(.15f,[](){});};
    auto Check=[&](bool OK,const TCHAR* Name){SmokeCheck(OK,Name);};
    auto OpenSetup=[&](){if(S->Screen==ECozyScreen::Playing)S->Pause();S->Selection=2;S->Confirm();S->Selection=10;S->Confirm();};
    auto CloseSetup=[&](){S->Back();S->Back();S->Back();};
    auto Place=[&](){P->ReturnHome();P->GetCharacterMovement()->StopMovementImmediately();P->TeleportTo(FVector(-200,-200,35),FRotator::ZeroRotator);PC->SetControlRotation(FRotator(-12,0,0));};
    auto Finish=[&](){auto J=MakeShared<FJsonObject>();auto Strings=[](const TArray<FString>& V){TArray<TSharedPtr<FJsonValue>> R;for(const auto& T:V)R.Add(MakeShared<FJsonValueString>(T));return R;};J->SetArrayField(TEXT("passed"),Strings(SmokePassed));J->SetArrayField(TEXT("failed"),Strings(SmokeFailed));J->SetStringField(TEXT("scope"),TEXT("Real map with synthetic ordinary Enhanced Input, local profile/position/menu fixtures, simulated deactivation and disconnect events. Separate process verifies persisted preferences. Physical comfort and new physical gameplay checks unverified."));FString Json;FJsonSerializer::Serialize(J,TJsonWriterFactory<>::Create(&Json));const bool Saved=FFileHelper::SaveStringToFile(Json,*(Root/(Resume?TEXT("controller-comfort-resume.json"):TEXT("controller-comfort.json"))));SmokeStage=10000;FPlatformMisc::RequestExitWithStatus(false,Saved&&SmokeFailed.IsEmpty()?0:1);};
    Add(FApp::CanEverRender()?20.f:3.f,[&](){
        Check(S->Slot==TEXT("CheweenieControllerComfortAcceptance"),TEXT("Comfort audit has an isolated adventure save"));
        if(!Resume){Pref->ResetDefaults();Check(UControllerGuide::Classify(TEXT("Razer Wolverine Ultimate"))==ECozyInputFamily::Xbox,TEXT("Wolverine Ultimate hardware name selects Xbox labels"));Check(UControllerGuide::Classify(TEXT("Razer Wolverine V2 Pro"))==ECozyInputFamily::Generic,TEXT("Other Razer models are not guessed as Xbox"));Check(Pref->L3Mode==2&&Pref->PreciseLook&&FMath::IsNearlyEqual(Pref->LookSpeed,1.f),TEXT("Comfort defaults preserve full turn speed and use precise response"));}
        else{Check(FMath::IsNearlyEqual(Pref->LeftDeadZone,.25f)&&FMath::IsNearlyEqual(Pref->RightDeadZone,.225f),TEXT("Separate stick dead zones reload in another process"));Check(FMath::IsNearlyEqual(Pref->LookSpeed,1.3f)&&!Pref->PreciseLook&&Pref->L3Mode==1,TEXT("Camera speed, linear response and L3 hold reload"));Check(FMath::IsNearlyEqual(Pref->CameraDistances[0],190.f),TEXT("Per-view zoom reloads"));Check(!L->SprintRequested,TEXT("Hurry latch never persists across restart"));}
    });
    Tap(A,[&](){Check(S->Screen==ECozyScreen::Playing,TEXT("Production confirm enters gameplay"));D->PetEnergy=100;});
    if(Resume)
    {
        Add(.4f,[&](){Start=P->GetActorLocation();View=PC->GetControlRotation();Axes=FVector4(.24f,0,.22f,0);});
        Add(.6f,[&](){Check(FVector::Dist2D(Start,P->GetActorLocation())<.5f&&(PC->GetControlRotation()-View).GetNormalized().IsNearlyZero(.1f),TEXT("Reloaded dead zones govern actual movement and look mappings"));Axes=FVector4(0,0,0,0);});
        Tap(L3,[&](){Check(L->SprintRequested,TEXT("Reloaded L3 hold starts hurry"));});
        Add(.3f,[&](){Check(!L->SprintRequested,TEXT("Reloaded L3 hold stops on release"));Pref->ResetDefaults();});
        Add(.3f,[&](){Finish();});
    }
    else
    {
        Tap(L3,[&](){Check(L->SprintRequested,TEXT("L3 click starts hurry without holding A"));});
        Add(.3f,[&](){Check(L->SprintRequested,TEXT("Hurry toggle survives L3 release"));});
        Add(.2f,[&](){Key(A,true);});
        Tap(L3,[&](){Check(L->SprintRequested,TEXT("Holding A still hurries when the L3 latch is switched off"));});
        Add(.2f,[&](){Key(A,false);});
        Add(.2f,[&](){Check(!L->SprintRequested,TEXT("Releasing the remaining hurry input restores walking"));});
        Tap(L3,[](){});
        Tap(Menu,[&](){Check(S->Screen==ECozyScreen::Pause&&!L->SprintRequested,TEXT("Opening a menu clears latched hurry"));});
        Tap(L3,[&](){Check(!L->SprintRequested,TEXT("L3 in a menu cannot queue a hurry toggle"));});
        Tap(B,[&](){Check(S->Screen==ECozyScreen::Playing&&!L->SprintRequested,TEXT("Returning to play does not revive hurry"));});
        Tap(L3,[](){});
        Add(.2f,[&](){FCoreDelegates::ApplicationWillDeactivateDelegate.Broadcast();});
        Add(.2f,[&](){Check(!L->SprintRequested,TEXT("Application deactivation cancels latched hurry"));});
        Tap(L3,[](){});
        Add(.2f,[&](){IPlatformInputDeviceMapper::Get().GetOnInputDeviceConnectionChange().Broadcast(EInputDeviceConnectionState::Disconnected,FPlatformUserId::CreateFromInternalId(0),INPUTDEVICEID_NONE);});
        Add(.2f,[&](){Check(!L->SprintRequested,TEXT("Device-disconnect notification cancels latched hurry"));});
        Tap(L3,[](){});
        Add(.2f,[&](){Place();Check(!L->SprintRequested,TEXT("Return home clears hurry before travel"));});
        Add(.3f,[&](){Axes=FVector4(0,.9f,0,0);});
        Add(.2f,[&](){Key(Menu,true);});
        Add(.3f,[&](){Key(Menu,false);Check(S->Screen==ECozyScreen::Pause&&S->Selection==0,TEXT("A stick held while opening a menu does not change selection"));Axes=FVector4(0,0,0,0);});
        Add(.2f,[&](){Start=P->GetActorLocation();View=PC->GetControlRotation();Axes=FVector4(0,-.9f,.8f,.8f);});
        Add(.2f,[&](){Check(S->Selection==1,TEXT("Left-stick down navigates one menu row"));});
        Add(.65f,[&](){Check(S->Selection>1&&S->Selection<=6,TEXT("Holding the stick scrolls with a controlled repeat delay"));Check(FVector::Dist2D(Start,P->GetActorLocation())<.5f&&(PC->GetControlRotation()-View).GetNormalized().IsNearlyZero(.1f),TEXT("Menu sticks navigate without moving Marty or orbiting"));Axes=FVector4(0,0,0,0);S->Selection=3;});
        Tap(A,[&](){Check(S->Screen==ECozyScreen::Settings,TEXT("Controller confirmation opens the selected settings"));Music=D->Music;});
        Add(.2f,[&](){Axes=FVector4(.9f,0,0,0);});
        Add(.2f,[&](){Check(FMath::IsNearlyEqual(D->Music,Music+.1f),TEXT("Horizontal menu stick adjusts a setting once"));});
        Add(.6f,[&](){Check(FMath::IsNearlyEqual(D->Music,Music+.1f),TEXT("Holding horizontal input does not repeatedly toggle or adjust"));Axes=FVector4(0,0,0,0);});
        Tap(B,[](){});
        Add(.2f,[&](){Axes=FVector4(.9f,0,0,0);});
        Add(.2f,[&](){Start=P->GetActorLocation();Key(B,true);});
        Add(.4f,[&](){Key(B,false);UE_LOG(LogTemp,Display,TEXT("COMFORT_RESUME screen=%d distance=%f leftBlocked=%d"),int32(S->Screen),FVector::Dist2D(Start,P->GetActorLocation()),P->ControllerAxisBlocked(false));Check(S->Screen==ECozyScreen::Playing&&FVector::Dist2D(Start,P->GetActorLocation())<.5f,TEXT("A navigation stick held on resume cannot cause accidental walking"));Key(EKeys::W,true);});
        Add(.4f,[&](){Check(FVector::Dist2D(Start,P->GetActorLocation())>10,TEXT("Keyboard movement works while a gamepad stick waits for neutral"));Key(EKeys::W,false);Axes=FVector4(0,0,0,0);});
        Add(.4f,[&](){Place();Start=P->GetActorLocation();Axes=FVector4(.8f,0,0,0);});
        Add(.4f,[&](){Check(FVector::Dist2D(Start,P->GetActorLocation())>10,TEXT("Releasing to neutral rearms controller walking"));Axes=FVector4(0,0,0,0);});
        Add(.3f,[&](){Place();S->OpenModes();});
        Add(.2f,[&](){Axes=FVector4(0,-.9f,0,0);});
        Add(.2f,[&](){S->Selection=3;Key(A,true);});
        Add(.6f,[&](){Key(A,false);auto* Hub=GetGameInstance()->GetSubsystem<UCozyModeHub>();Check(Hub->Page==ECozyHubPage::Graphics&&Hub->Selection==0,TEXT("A stick held across hub pages cannot scroll the new page"));Axes=FVector4(0,0,0,0);});
        Tap(B,[&](){Check(GetGameInstance()->GetSubsystem<UCozyModeHub>()->Page==ECozyHubPage::Modes,TEXT("Hub Back returns to its parent page"));});
        Tap(B,[&](){Check(S->Screen==ECozyScreen::Playing,TEXT("Hub closes to gameplay after neutral menu navigation"));});
        Tap(EKeys::Gamepad_DPad_Right,[](){});
        Add(.5f,[&](){Check(FMath::IsNearlyEqual(Boom->TargetArmLength,190.f,1.f),TEXT("D-pad Right smoothly zooms the gameplay camera in"));});
        Tap(EKeys::Gamepad_DPad_Left,[](){});
        Add(.5f,[&](){Check(FMath::IsNearlyEqual(Boom->TargetArmLength,220.f,1.f),TEXT("D-pad Left zooms out without changing preset"));});
        Tap(EKeys::Gamepad_DPad_Right,[](){});
        Add(.5f,[](){});
        for(int I=0;I<4;++I)Tap(EKeys::Gamepad_RightThumbstick,[](){});
        Add(.3f,[&](){Check(P->CameraPreset==0&&FMath::IsNearlyEqual(Boom->TargetArmLength,190.f,1.f),TEXT("Camera cycling remembers each view's zoom"));OpenSetup();});
        Add(.2f,[&](){Check(S->Screen==ECozyScreen::ControllerSetup&&S->Menu().Num()==9,TEXT("Controller comfort settings are reachable from Controls"));S->Selection=3;});
        Tap(EKeys::Gamepad_DPad_Right,[&](){Check(FMath::IsNearlyEqual(Pref->LookSpeed,1.1f),TEXT("D-pad adjusts saved camera turn speed"));S->Selection=0;});
        Tap(A,[&](){Check(!Pref->PreciseLook&&FMath::IsNearlyEqual(Pref->LookSpeed,1.5f),TEXT("Fast preset selects a quicker linear camera"));S->Selection=0;});
        Tap(A,[&](){Check(Pref->PreciseLook&&FMath::IsNearlyEqual(Pref->LookSpeed,1.f),TEXT("Comfort preset restores fine response with unchanged full turn speed"));S->Selection=5;});
        Tap(EKeys::Gamepad_DPad_Right,[&](){Check(Pref->L3Mode==0,TEXT("L3 can be disabled in settings"));CloseSetup();});
        Tap(L3,[&](){Check(!L->SprintRequested,TEXT("Disabled L3 does not request hurry"));OpenSetup();S->Selection=5;});
        Tap(EKeys::Gamepad_DPad_Right,[&](){Check(Pref->L3Mode==1,TEXT("L3 hold mode is selectable"));CloseSetup();});
        Tap(L3,[&](){Check(L->SprintRequested,TEXT("L3 hold mode hurries while pressed"));});
        Add(.2f,[&](){Check(!L->SprintRequested,TEXT("L3 hold mode stops on release"));OpenSetup();S->Selection=7;});
        Tap(A,[&](){Check(Pref->L3Mode==2&&Pref->PreciseLook&&FMath::IsNearlyEqual(Pref->CameraDistances[0],220.f),TEXT("Restore defaults resets controller tuning and zoom"));S->Selection=1;});
        Tap(EKeys::Gamepad_DPad_Left,[&](){Check(FMath::IsNearlyEqual(Pref->LeftDeadZone,.175f),TEXT("Left dead zone adjusts independently"));S->Selection=2;});
        Tap(EKeys::Gamepad_DPad_Right,[](){});
        Tap(EKeys::Gamepad_DPad_Right,[&](){Check(FMath::IsNearlyEqual(Pref->RightDeadZone,.25f)&&FMath::IsNearlyEqual(Pref->LeftDeadZone,.175f),TEXT("Right dead zone adjusts without changing the left"));CloseSetup();});
        Add(.3f,[&](){Place();Start=P->GetActorLocation();View=PC->GetControlRotation();Axes=FVector4(.2f,0,.24f,0);});
        Add(.8f,[&](){Check(FVector::Dist2D(Start,P->GetActorLocation())>.5f,TEXT("Live left-zone tuning affects actual analog walking"));Check((PC->GetControlRotation()-View).GetNormalized().IsNearlyZero(.1f),TEXT("Live right-zone tuning filters look independently"));Axes=FVector4(0,0,0,0);OpenSetup();});
        Add(.3f,[&](){Pref->ResetDefaults();if(FApp::CanEverRender())FScreenshotRequest::RequestScreenshot(Root/TEXT("ControllerComfortSettings.png"),true,false);});
        Add(.3f,[&](){Pref->LeftDeadZone=.25f;Pref->RightDeadZone=.225f;Pref->LookSpeed=1.3f;Pref->PreciseLook=false;Pref->L3Mode=1;Pref->CameraDistances[0]=190;Pref->Save();Finish();});
    }
    if(Steps.IsValidIndex(SmokeStage)&&SmokeTime>=Steps[SmokeStage].Key){const int32 Before=SmokeStage;Steps[SmokeStage].Value();if(SmokeStage==Before)++SmokeStage;SmokeTime=0;}
#endif
}
