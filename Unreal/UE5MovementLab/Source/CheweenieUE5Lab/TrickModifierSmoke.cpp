#include "MartyCharacter.h"
#include "DogLife.h"
#include "CozyAdventure.h"
#include "Components/PoseableMeshComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InputKeyEventArgs.h"
#include "Kismet/GameplayStatics.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/Parse.h"
#include "Misc/CommandLine.h"
#include "Misc/App.h"
#include "UnrealClient.h"

void AMartyPlayMode::TickTrickModifierSmoke(float Delta)
{
    SmokeTime+=Delta;auto* PC=GetWorld()->GetFirstPlayerController();auto* P=PC?Cast<AMartyCharacter>(PC->GetPawn()):nullptr;
    if(!P||!P->Life||!Adventure||!Adventure->Progress)return;
    auto* L=P->Life.Get();auto* S=Adventure.Get();auto* D=S->Progress.Get();
    FString Root=FPaths::ProjectSavedDir();FParse::Value(FCommandLine::Get(),TEXT("CheweenieTestOutput="),Root);
    auto Key=[&](FKey K,bool Down){PC->InputKey(FInputKeyEventArgs(nullptr,INPUTDEVICEID_NONE,K,Down?IE_Pressed:IE_Released,FPlatformTime::Cycles64()));};
    auto Next=[&](){++SmokeStage;SmokeTime=0;};
    auto Shot=[&](const TCHAR* Name){if(FApp::CanEverRender())FScreenshotRequest::RequestScreenshot(Root/Name,true,false);};
    const auto LB=EKeys::Gamepad_LeftShoulder,A=EKeys::Gamepad_FaceButton_Bottom,B=EKeys::Gamepad_FaceButton_Right,X=EKeys::Gamepad_FaceButton_Left,Y=EKeys::Gamepad_FaceButton_Top;
    PC->InputKey(FInputKeyEventArgs(nullptr,INPUTDEVICEID_NONE,EKeys::Gamepad_RightX,SmokeStage==1?.5f:0.f,Delta,1,FPlatformTime::Cycles64()));
    if(SmokeTime<.15)return;
    switch(SmokeStage)
    {
    case 0:
        if(SmokeTime<(FApp::CanEverRender()?30:3))break;
        S->Start(true);D->PetEnergy=80;SmokeCheck(S->Slot==TEXT("CheweenieTrickModifierAcceptance"),TEXT("Modifier acceptance uses an isolated save"));SmokePosition=P->GetActorLocation();SmokeYaw=PC->GetControlRotation().Yaw;Key(LB,true);Key(EKeys::W,true);Next();break;
    case 1:
        if(SmokeTime<.4)break;
        SmokeCheck(P->TrickModifierVisible()&&S->Screen==ECozyScreen::Playing,TEXT("Holding L1-LB exposes direct tricks without opening a menu"));
        SmokeCheck(L->BarkCount==0,TEXT("A modifier hold does not bark"));
        Key(EKeys::W,false);SmokeCheck(FVector::Dist2D(P->GetActorLocation(),SmokePosition)>10,TEXT("Movement remains active while the modifier guide is held"));
        SmokeCheck(FMath::Abs(FRotator::NormalizeAxis(PC->GetControlRotation().Yaw-SmokeYaw))>2,TEXT("Right-stick camera remains active while holding the modifier"));Shot(TEXT("TrickModifierGuide.png"));Key(A,true);Next();break;
    case 2:
        Key(A,false);SmokeCheck(L->Activity==EDogActivity::Lying,TEXT("L1-LB plus A-Cross lies down directly"));
        SmokeCheck(!L->SprintRequested&&S->Screen==ECozyScreen::Playing,TEXT("Lie chord suppresses sprint and menu confirm"));Shot(TEXT("TrickModifierLie.png"));Key(LB,false);Next();break;
    case 3:
        SmokeCheck(L->BarkCount==0,TEXT("Releasing a used modifier does not add a bark"));Key(LB,true);Next();break;
    case 4:Key(A,true);Next();break;
    case 5:
        Key(A,false);SmokeCheck(L->Activity==EDogActivity::Idle,TEXT("The same lie chord gets Marty back up"));Key(B,true);Next();break;
    case 6:
        Key(B,false);SmokeCheck(L->Activity==EDogActivity::Rolling&&S->Screen==ECozyScreen::Playing,TEXT("L1-LB plus B-Circle rolls instead of opening Tricks"));Next();break;
    case 7:
        if(SmokeTime<.45)break;Shot(TEXT("TrickModifierRoll.png"));Key(X,true);Next();break;
    case 8:
        Key(X,false);SmokeCheck(L->Activity==EDogActivity::Rolling&&D->CarryItem==-1,TEXT("A busy trick consumes the chord without interacting or queueing"));Next();break;
    case 9:
        if(SmokeTime<2)break;SmokeCheck(L->Activity==EDogActivity::Idle,TEXT("Direct roll finishes and restores control"));Key(X,true);Next();break;
    case 10:
        if(SmokeTime<.55)break;Key(X,false);SmokeCheck(L->Activity==EDogActivity::Shaking&&S->Screen==ECozyScreen::Playing,TEXT("L1-LB plus X-Square shakes without interaction"));
        SmokeCheck(P->Skin->GetBoneLocationByName(TEXT("front_l_paw"),EBoneSpaces::ComponentSpace).Z>10,TEXT("Direct shake raises the actual skinned paw"));Shot(TEXT("TrickModifierShake.png"));Next();break;
    case 11:
        if(SmokeTime<2)break;Key(Y,true);Next();break;
    case 12:
        Key(Y,false);SmokeCheck(L->BarkCount==1,TEXT("L1-LB plus Y-Triangle barks directly"));
        SmokeCheck(!P->GetCharacterMovement()->IsFalling(),TEXT("Bark chord does not spend a jump"));Key(LB,false);Next();break;
    case 13:
        if(SmokeTime<1.4)break;SmokeCheck(L->BarkCount==1,TEXT("Finishing the bark chord does not bark again"));Key(LB,true);Next();break;
    case 14:Key(LB,false);Next();break;
    case 15:
        SmokeCheck(L->BarkCount==2,TEXT("A quick unmodified shoulder tap still barks once"));Key(EKeys::LeftControl,true);Next();break;
    case 16:Key(EKeys::One,true);Next();break;
    case 17:
        Key(EKeys::One,false);SmokeCheck(L->Activity==EDogActivity::Lying,TEXT("Left Ctrl plus 1 offers the keyboard lie shortcut"));Next();break;
    case 18:Key(EKeys::One,true);Next();break;
    case 19:
        Key(EKeys::One,false);SmokeCheck(L->Activity==EDogActivity::Idle,TEXT("Keyboard shortcut also toggles standing"));Key(EKeys::LeftControl,false);Next();break;
    case 20:
        SmokeCheck(!P->TrickModifierVisible()&&L->BarkCount==2,TEXT("Releasing keyboard Ctrl hides hints and never barks"));
        P->TeleportTo(FVector(0,-230,28),FRotator::ZeroRotator);Key(EKeys::E,true);Next();break;
    case 21:
        Key(EKeys::E,false);SmokeCheck(D->CarryItem==0,TEXT("Normal interaction still picks up the ball after the modifier releases"));Key(EKeys::SpaceBar,true);Next();break;
    case 22:
        Key(EKeys::SpaceBar,false);SmokeCheck(P->GetCharacterMovement()->IsFalling(),TEXT("Normal jump still works after the modifier releases"));P->ReturnHome();Next();break;
    case 23:
        if(SmokeTime<.5)break;Key(LB,true);Next();break;
    case 24:if(SmokeTime<.4)break;Key(EKeys::Escape,true);Next();break;
    case 25:
        Key(EKeys::Escape,false);SmokeCheck(S->Screen==ECozyScreen::Pause&&!P->TrickModifierVisible(),TEXT("Pause dismisses and disarms the modifier layer"));Key(A,true);Next();break;
    case 26:
        Key(A,false);SmokeCheck(S->Screen==ECozyScreen::Playing&&L->Activity==EDogActivity::Idle,TEXT("A-Cross resumes a menu normally without a queued lie"));Key(LB,false);Next();break;
    case 27:
        SmokeCheck(L->BarkCount==2,TEXT("Releasing a menu-disarmed hold never barks"));S->ToggleFeature(ECozyFeature::Tricks);Key(LB,true);Next();break;
    case 28:Key(B,true);Next();break;
    case 29:
        Key(B,false);SmokeCheck(L->Activity==EDogActivity::Idle&&S->Screen==ECozyScreen::Playing,TEXT("Disabled tricks consume the roll chord without opening a menu"));
        S->ToggleFeature(ECozyFeature::Bark);Key(Y,true);Next();break;
    case 30:
        Key(Y,false);SmokeCheck(L->BarkCount==2&&!P->GetCharacterMovement()->IsFalling(),TEXT("Disabled barking also consumes the chord without jumping"));
        S->ToggleFeature(ECozyFeature::Bark);S->ToggleFeature(ECozyFeature::Tricks);Key(LB,false);Next();break;
    case 31:Key(LB,true);Key(X,true);Next();break;
    case 32:
        SmokeCheck(L->Activity==EDogActivity::Shaking,TEXT("Same-frame shoulder and face presses resolve as a single chord"));Key(LB,false);Key(X,false);Next();break;
    case 33:
        if(SmokeTime<2)break;Key(A,true);Next();break;
    case 34:
        SmokeCheck(L->SprintRequested,TEXT("Unmodified A-Cross still requests hurry"));Key(LB,true);Next();break;
    case 35:
        if(SmokeTime<.4)break;SmokeCheck(!L->SprintRequested&&L->Activity==EDogActivity::Idle,TEXT("Adding the modifier to an already held face button cancels hurry without firing a trick"));Key(A,false);Key(LB,false);Next();break;
    case 36:
        SmokeCheck(S->Screen==ECozyScreen::Playing&&L->BarkCount==2,TEXT("Full chord sequence leaves gameplay active with no stray bark"));
        {
            auto Report=MakeShared<FJsonObject>();auto Strings=[](const TArray<FString>& In){TArray<TSharedPtr<FJsonValue>> Out;for(const auto& V:In)Out.Add(MakeShared<FJsonValueString>(V));return Out;};
            Report->SetStringField(TEXT("scope"),TEXT("Actual map with synthetic keyboard/gamepad events, save/position fixtures and rendered guide/poses when requested. Physical controllers unverified."));
            Report->SetArrayField(TEXT("passed"),Strings(SmokePassed));Report->SetArrayField(TEXT("failed"),Strings(SmokeFailed));FString Json;FJsonSerializer::Serialize(Report,TJsonWriterFactory<>::Create(&Json));
            const bool Saved=FFileHelper::SaveStringToFile(Json,*(Root/TEXT("trick-modifier.json")));UGameplayStatics::DeleteGameInSlot(S->Slot,0);SmokeStage=1000;
            FPlatformMisc::RequestExitWithStatus(false,Saved&&SmokeFailed.IsEmpty()?0:1);
        }break;
    }
}
