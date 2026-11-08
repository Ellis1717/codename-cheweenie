// Opt-in acceptance through the ordinary Enhanced Input mappings, never test-only bindings.
#include "MartyCharacter.h"
#include "CozyAdventure.h"
#include "DogLife.h"
#include "Neighbourhood.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputKeyEventArgs.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/App.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "Kismet/GameplayStatics.h"
#include "UnrealClient.h"

void AMartyPlayMode::TickControllerButtonsSmoke(float Delta)
{
#if !UE_BUILD_SHIPPING
    SmokeTime+=Delta;
    auto* PC=GetWorld()->GetFirstPlayerController();
    auto* P=PC?Cast<AMartyCharacter>(PC->GetPawn()):nullptr;
    if(!P||!P->Life||!Adventure||!Adventure->Progress)return;
    auto* S=Adventure.Get();auto* D=S->Progress.Get();auto* L=P->Life.Get();
    FString Root=FPaths::ProjectSavedDir();FParse::Value(FCommandLine::Get(),TEXT("CheweenieTestOutput="),Root);
    static FVector4 Axes(0,0,0,0);
    static int32 Barks=0,Carry=-1,Camera=0;
    static float Music=0;
    static FVector Start;
    static FRotator View;
    const auto A=EKeys::Gamepad_FaceButton_Bottom,B=EKeys::Gamepad_FaceButton_Right;
    const auto X=EKeys::Gamepad_FaceButton_Left,Y=EKeys::Gamepad_FaceButton_Top;
    const auto LB=EKeys::Gamepad_LeftShoulder,RB=EKeys::Gamepad_RightShoulder;
    const auto Menu=EKeys::Gamepad_Special_Right,Bag=EKeys::Gamepad_Special_Left;
    const auto Up=EKeys::Gamepad_DPad_Up,Down=EKeys::Gamepad_DPad_Down;
    auto Key=[&](FKey K,bool Press){PC->InputKey(FInputKeyEventArgs(nullptr,INPUTDEVICEID_NONE,K,Press?IE_Pressed:IE_Released,FPlatformTime::Cycles64()));};
    auto Axis=[&](FKey K,float V){PC->InputKey(FInputKeyEventArgs(nullptr,INPUTDEVICEID_NONE,K,V,Delta,1,FPlatformTime::Cycles64()));};
    Axis(EKeys::Gamepad_LeftX,Axes.X);Axis(EKeys::Gamepad_LeftY,Axes.Y);
    Axis(EKeys::Gamepad_RightX,Axes.Z);Axis(EKeys::Gamepad_RightY,Axes.W);
    TArray<TPair<float,TFunction<void()>>> Steps;
    auto Add=[&](float Delay,TFunction<void()> Run){Steps.Emplace(Delay,MoveTemp(Run));};
    auto Tap=[&](FKey K,TFunction<void()> Check,float Hold=.18f)
    {
        Add(.15f,[&,K](){Key(K,true);});
        Add(Hold,[&,K,Check=MoveTemp(Check)](){Check();Key(K,false);});
        Add(.15f,[](){});
    };
    auto Check=[&](bool OK,const TCHAR* Name){SmokeCheck(OK,Name);};
    auto Place=[&](FVector Position){L->CancelAction();P->GetCharacterMovement()->StopMovementImmediately();P->TeleportTo(Position,FRotator::ZeroRotator);PC->SetControlRotation(FRotator(-12,0,0));};
    Add(FApp::CanEverRender()?20.f:3.f,[&](){
        Check(S->Slot==TEXT("CheweenieControllerButtonsAcceptance"),TEXT("Button audit uses an isolated save"));
        Check(S->Screen==ECozyScreen::Title,TEXT("Ordinary game title and production bindings are active"));
    });
    Tap(A,[&](){Check(S->Screen==ECozyScreen::Playing,TEXT("A/Cross starts the game from the title"));});
    Add(.3f,[&](){Check(!L->SprintRequested,TEXT("Releasing title confirm leaves hurry off"));D->PetEnergy=100;});
    Tap(A,[&](){Check(L->SprintRequested&&P->GetCharacterMovement()->MaxWalkSpeed>150,TEXT("Holding A/Cross requests hurry in gameplay"));},.4f);
    Add(.2f,[&](){Check(!L->SprintRequested,TEXT("Releasing A/Cross ends hurry"));});
    Tap(B,[&](){Check(S->Screen==ECozyScreen::Tricks,TEXT("B/Circle opens Tricks during gameplay"));});
    Tap(B,[&](){Check(S->Screen==ECozyScreen::Playing,TEXT("B/Circle returns from Tricks"));});
    Tap(Bag,[&](){Check(S->Screen==ECozyScreen::Backpack,TEXT("View/touchpad opens the backpack"));});
    Tap(Bag,[&](){Check(S->Screen==ECozyScreen::Playing,TEXT("View/touchpad closes the backpack"));});
    Tap(Up,[&](){Check(S->Screen==ECozyScreen::Map,TEXT("D-pad Up opens the map"));});
    Tap(Down,[&](){Check(S->Screen==ECozyScreen::Map&&S->Selection==1,TEXT("D-pad Down navigates the map without opening crew"));});
    Tap(Up,[&](){Check(S->Selection==0,TEXT("D-pad Up navigates back up the map"));});
    Tap(B,[&](){Check(S->Screen==ECozyScreen::Playing,TEXT("B/Circle closes the map"));});
    Tap(Down,[&](){Check(S->Screen==ECozyScreen::Companions,TEXT("D-pad Down opens companions during gameplay"));});
    Tap(B,[&](){Check(S->Screen==ECozyScreen::Playing,TEXT("B/Circle closes companions"));});
    Tap(Menu,[&](){Check(S->Screen==ECozyScreen::Pause,TEXT("Menu/Options pauses without teleporting Marty"));});
    Tap(A,[&](){Check(S->Screen==ECozyScreen::Playing&&!L->SprintRequested,TEXT("A/Cross resumes without leaking hurry into gameplay"));});
    Tap(Menu,[&](){Check(S->Screen==ECozyScreen::Pause,TEXT("Menu/Options can pause again after resume"));});
    Add(.2f,[&](){S->Selection=3;});
    Tap(A,[&](){Check(S->Screen==ECozyScreen::Settings,TEXT("A/Cross selects sound settings"));Music=D->Music;});
    Tap(EKeys::Gamepad_DPad_Right,[&](){Check(FMath::IsNearlyEqual(D->Music,Music+.1f),TEXT("D-pad Right increases the selected setting"));});
    Tap(EKeys::Gamepad_DPad_Left,[&](){Check(FMath::IsNearlyEqual(D->Music,Music),TEXT("D-pad Left decreases the selected setting"));});
    Tap(B,[&](){Check(S->Screen==ECozyScreen::Pause,TEXT("B/Circle returns to the pause menu"));});
    Tap(Menu,[&](){Check(S->Screen==ECozyScreen::Playing,TEXT("Menu/Options resumes gameplay"));});
    Add(.3f,[&](){Place(FVector(0,-230,35));});
    Tap(X,[&](){Check(D->CarryItem==0,TEXT("X/Square picks up the nearby ball"));});
    Tap(EKeys::Gamepad_LeftTrigger,[&](){Check(D->CarryItem==-1,TEXT("LT/L2 drops the carried toy"));});
    Add(.3f,[&](){Place(FVector(-200,-200,35));});
    Tap(Y,[&](){Check(P->GetCharacterMovement()->IsFalling(),TEXT("Y/Triangle jumps"));},.12f);
    Add(1.2f,[&](){Check(P->GetCharacterMovement()->IsMovingOnGround(),TEXT("Jump release returns Marty to grounded movement"));Barks=L->BarkCount;});
    Tap(LB,[](){},.12f);
    Add(.3f,[&](){Check(L->BarkCount==Barks+1,TEXT("A short LB/L1 tap barks exactly once"));});
    for(int32 I=0;I<6;++I)
    {
        Add(.3f,[&](){Camera=P->CameraPreset;});
        Tap(EKeys::Gamepad_RightThumbstick,[&](){Check(P->CameraPreset==(Camera+1)%6,TEXT("R3 advances the camera and wraps through all six release views"));});
    }
    Add(.2f,[&](){View=PC->GetControlRotation();Camera=P->CameraPreset;Start=P->GetActorLocation();});
    Tap(EKeys::Gamepad_LeftThumbstick,[&](){Check(L->SprintRequested&&P->CameraPreset==Camera&&S->Screen==ECozyScreen::Playing&&FVector::Dist2D(Start,P->GetActorLocation())<.5f,TEXT("L3 toggles hurry without moving Marty or changing camera"));});
    Add(.2f,[&](){Check(L->SprintRequested,TEXT("L3 hurry remains active after release"));});
    Tap(EKeys::Gamepad_LeftThumbstick,[&](){Check(!L->SprintRequested,TEXT("Second L3 click restores walking"));});
    for(int32 I=0;I<4;++I)
    {
        Add(.3f,[&,I](){Place(FVector(-200,-200,35));Start=P->GetActorLocation();Axes=FVector4(I<2?(I==0?.6f:-.6f):0,I>=2?(I==2?.6f:-.6f):0,0,0);});
        Add(.65f,[&,I](){const auto Offset=P->GetActorLocation()-Start;Check((I<2?Offset.Y:Offset.X)*(I%2==0?1:-1)>8,TEXT("Left-stick cardinal direction moves Marty in the expected direction"));Axes=FVector4(0,0,0,0);});
        Add(.4f,[&](){Check(P->GetVelocity().Size2D()<.1f,TEXT("Releasing each left-stick direction stops Marty"));});
    }
    for(int32 I=0;I<2;++I)
    {
        Add(.2f,[&,I](){View=PC->GetControlRotation();Axes=FVector4(0,0,I==0?.6f:-.6f,I==0?.4f:-.4f);});
        Add(.5f,[&,I](){const auto Change=(PC->GetControlRotation()-View).GetNormalized();Check(Change.Yaw*(I==0?1:-1)>3&&Change.Pitch*(I==0?1:-1)>1,TEXT("Right stick orbits and pitches in both directions"));Axes=FVector4(0,0,0,0);});
        Add(.2f,[](){});
        Tap(RB,[&](){const auto R=PC->GetControlRotation();UE_LOG(LogTemp,Display,TEXT("CONTROLLER_RECENTER yaw=%f target=%f pitch=%f"),R.Yaw,P->GetActorRotation().Yaw,FRotator::NormalizeAxis(R.Pitch));Check(FMath::Abs(FRotator::NormalizeAxis(R.Yaw-P->GetActorRotation().Yaw))<1&&FMath::Abs(FRotator::NormalizeAxis(R.Pitch)+12)<1,TEXT("RB/R1 recenters the camera after orbiting"));});
    }
    for(int32 I=0;I<2;++I)
    {
        Add(.2f,[&,I](){Start=P->GetActorLocation();View=PC->GetControlRotation();const float V=I==0?.19f:-.19f;Axes=FVector4(V,-V,V,-V);});
        Add(.8f,[&](){Check(FVector::Dist2D(Start,P->GetActorLocation())<.5f&&P->GetVelocity().Size2D()<.1f,TEXT("Diagonal resting-stick noise does not creep"));Check((PC->GetControlRotation()-View).GetNormalized().IsNearlyZero(.1f),TEXT("Resting-stick noise does not rotate the camera"));Axes=FVector4(0,0,0,0);});
    }
    Add(.2f,[&](){Neighbourhood->GoOutside(P);Place(L->Patches[0]+FVector(-48,0,35));});
    Tap(EKeys::Gamepad_RightTrigger,[&](){Check(L->Activity==EDogActivity::Digging,TEXT("RT/R2 starts digging at a dig patch"));},.25f);
    Add(1.3f,[&](){Check(D->HoleDepths[0]==1,TEXT("A held/released trigger completes one dig without repeating"));P->ReturnHome();PC->SetViewTarget(P);});
    Add(.3f,[&](){Start=P->GetActorLocation();Camera=P->CameraPreset;Carry=D->CarryItem;Barks=L->BarkCount;View=PC->GetControlRotation();});
    Tap(Menu,[](){});
    Tap(X,[](){});Tap(Y,[](){});Tap(EKeys::Gamepad_RightTrigger,[](){});Tap(EKeys::Gamepad_LeftTrigger,[](){});Tap(LB,[](){});
    Tap(EKeys::Gamepad_RightThumbstick,[](){});
    Add(.2f,[&](){Axes=FVector4(.8f,.8f,.8f,.8f);});
    Add(.6f,[&](){Axes=FVector4(0,0,0,0);Check(S->Screen==ECozyScreen::Pause&&L->Activity==EDogActivity::Idle&&L->BarkCount==Barks&&D->CarryItem==Carry&&P->CameraPreset==Camera&&!L->SprintRequested&&FVector::Dist2D(Start,P->GetActorLocation())<.5f,TEXT("Pause blocks gameplay buttons, jumping, movement, barking and camera cycling"));Check((PC->GetControlRotation()-View).GetNormalized().IsNearlyZero(.1f),TEXT("Pause blocks right-stick orbit"));});
    Tap(B,[&](){Check(S->Screen==ECozyScreen::Playing,TEXT("B/Circle resumes from pause"));});
    Add(.2f,[&](){Key(A,true);});
    Add(.3f,[&](){Check(L->SprintRequested,TEXT("Hurry works again after menu testing"));PC->FlushPressedKeys();});
    Add(.3f,[&](){Check(!L->SprintRequested,TEXT("Input flush clears held hurry after focus loss"));Key(A,false);});
    Add(.2f,[&](){if(FApp::CanEverRender())FScreenshotRequest::RequestScreenshot(Root/TEXT("ControllerButtonsGameplay.png"),true,false);});
    Add(.4f,[&](){
        auto J=MakeShared<FJsonObject>();auto Strings=[](const TArray<FString>& V){TArray<TSharedPtr<FJsonValue>> R;for(const auto& Text:V)R.Add(MakeShared<FJsonValueString>(Text));return R;};
        J->SetArrayField(TEXT("passed"),Strings(SmokePassed));J->SetArrayField(TEXT("failed"),Strings(SmokeFailed));
        J->SetStringField(TEXT("scope"),TEXT("Real game map; normal Enhanced Input mappings; synthetic controller events, controlled positions and isolated save. Physical button presses and PlayStation driver mapping unverified."));
        FString Json;FJsonSerializer::Serialize(J,TJsonWriterFactory<>::Create(&Json));
        const bool Saved=FFileHelper::SaveStringToFile(Json,*(Root/TEXT("controller-buttons.json")));
        UGameplayStatics::DeleteGameInSlot(S->Slot,0);SmokeStage=10000;FPlatformMisc::RequestExitWithStatus(false,Saved&&SmokeFailed.IsEmpty()?0:1);
    });
    if(Steps.IsValidIndex(SmokeStage)&&SmokeTime>=Steps[SmokeStage].Key)
    {
        const int32 Before=SmokeStage;Steps[SmokeStage].Value();
        if(SmokeStage==Before)++SmokeStage;
        SmokeTime=0;
    }
#endif
}
