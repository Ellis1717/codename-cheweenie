#include "MartyCharacter.h"
#include "DogLife.h"
#include "MartyGait.h"
#include "CozyAdventure.h"
#include "Components/PoseableMeshComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/DamageEvents.h"
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

void AMartyPlayMode::TickTricksSmoke(float Delta)
{
    SmokeTime+=Delta;auto* PC=GetWorld()->GetFirstPlayerController();auto* P=PC?Cast<AMartyCharacter>(PC->GetPawn()):nullptr;
    if(!P||!P->Life||!Adventure||!Adventure->Progress)return;auto* L=P->Life.Get();auto* D=Adventure->Progress.Get();
    FString Root=FPaths::ProjectSavedDir();FParse::Value(FCommandLine::Get(),TEXT("CheweenieTestOutput="),Root);
    auto Key=[PC](FKey K,bool Down){PC->InputKey(FInputKeyEventArgs(nullptr,INPUTDEVICEID_NONE,K,Down?IE_Pressed:IE_Released,FPlatformTime::Cycles64()));};
    auto Next=[this](){++SmokeStage;SmokeTime=0;};
    auto Axis=[PC,Delta](FKey K,float Value){PC->InputKey(FInputKeyEventArgs(nullptr,INPUTDEVICEID_NONE,K,Value,Delta,1,FPlatformTime::Cycles64()));};
    if(SmokeStage==24){Axis(EKeys::Gamepad_RightX,.8f);Axis(EKeys::Gamepad_RightY,.4f);Axis(EKeys::Gamepad_LeftY,.4f);}
    if(SmokeStage>=25){Axis(EKeys::Gamepad_RightX,0);Axis(EKeys::Gamepad_RightY,0);Axis(EKeys::Gamepad_LeftY,0);}
    auto Shot=[&](const TCHAR* Name){if(FApp::CanEverRender())FScreenshotRequest::RequestScreenshot(Root/Name,true,false);};
    auto Finish=[&](bool Resume){auto O=MakeShared<FJsonObject>();TArray<TSharedPtr<FJsonValue>> Pass,Fail;for(auto& S:SmokePassed)Pass.Add(MakeShared<FJsonValueString>(S));for(auto& S:SmokeFailed)Fail.Add(MakeShared<FJsonValueString>(S));O->SetStringField(TEXT("scope"),TEXT("Synthetic keyboard/controller menus, actual skin poses and CharacterMovement acceleration. Health/item/selection fixtures; physical devices unverified."));O->SetArrayField(TEXT("passed"),Pass);O->SetArrayField(TEXT("failed"),Fail);FString Json;FJsonSerializer::Serialize(O,TJsonWriterFactory<>::Create(&Json));bool Saved=FFileHelper::SaveStringToFile(Json,*(Root/(Resume?TEXT("tricks-resume.json"):TEXT("tricks-smoke.json"))));if(Resume)UGameplayStatics::DeleteGameInSlot(TEXT("CheweenieTricksAcceptance"),0);FPlatformMisc::RequestExitWithStatus(false,Saved&&SmokeFailed.IsEmpty()?0:1);SmokeStage=1000;};
    if(FParse::Param(FCommandLine::Get(),TEXT("CheweenieTricksResume")))
    {
        if(SmokeTime>3&&SmokeStage==0){SmokeCheck(D->BagItems.Num()==2&&D->BagItems[0]!=D->BagItems[1]&&D->CarryItem==-1,TEXT("Both backpack items reload without duplicating a mouth item"));SmokeCheck(FMath::IsNearlyEqual(D->PetHealth,62.f,.01f)&&FMath::IsNearlyEqual(D->PetEnergy,42.f,.01f),TEXT("Health and energy reload in a second process"));SmokeCheck(D->PetPace==2,TEXT("Selected run pace persists"));SmokeCheck(L->Activity==EDogActivity::Idle&&Adventure->Screen==ECozyScreen::Title,TEXT("Transient tricks do not resume on title"));SmokeCheck(D->BodyControls&&L->BodyRoll==0&&L->BodyPitch==0,TEXT("Control mode persists while spring offsets reset safely"));Finish(true);}return;
    }
    auto Trick=[&](int32 Selection){Adventure->OpenTricks();Adventure->Selection=Selection;Key(EKeys::Enter,true);};
    switch(SmokeStage)
    {
    case 0:if(SmokeTime>(FApp::CanEverRender()?40:3)){Adventure->Start(true);D->PetHealth=60;D->PetEnergy=80;D->CarryItem=0;P->TakeDamage(15,FDamageEvent(),nullptr,nullptr);SmokePitch=P->Skin->GetBoneRotationByName(TEXT("root"),EBoneSpaces::ComponentSpace).Roll;Next();}break;
    case 1:if(SmokeTime>.6){SmokeCheck(L->Health()>44&&L->Health()<46,TEXT("Engine damage updates health without replacing energy"));Key(EKeys::Tab,true);Next();}break;
    case 2:if(SmokeTime>.2){Key(EKeys::Tab,false);SmokeCheck(Adventure->Screen==ECozyScreen::Backpack,TEXT("Tab opens the backpack menu"));Key(EKeys::Enter,true);Next();}break;
    case 3:if(SmokeTime>.2){Key(EKeys::Enter,false);SmokeCheck(D->CarryItem==-1&&D->BagItems.Contains(0),TEXT("Backpack menu stows the carried ball"));Adventure->OpenBackpack();Adventure->Selection=1;SmokeMoving=false;Next();}break;
    case 4:if(SmokeTime>.1&&!SmokeMoving){Key(EKeys::Enter,true);SmokeMoving=true;}if(SmokeTime>.3){Key(EKeys::Enter,false);SmokeCheck(D->CarryItem==0&&D->BagItems.IsEmpty(),TEXT("Retrieving moves one bag item back to the mouth"));Key(EKeys::Gamepad_FaceButton_Right,true);SmokeMoving=false;Next();}break;
    case 5:if(SmokeTime>.2){Key(EKeys::Gamepad_FaceButton_Right,false);SmokeCheck(Adventure->Screen==ECozyScreen::Tricks,TEXT("Controller B/Circle opens the tricks menu"));Key(EKeys::Enter,true);Next();}break;
    case 6:if(SmokeTime>.3){Key(EKeys::Enter,false);SmokeCheck(L->Activity==EDogActivity::Lying,TEXT("Lie down enters the resting pose"));SmokeCheck(P->Skin->GetBoneLocationByName(TEXT("spine"),EBoneSpaces::ComponentSpace).Z<10,TEXT("Lie down lowers the torso into a genuine resting pose"));SmokeYaw=D->PetEnergy;Shot(TEXT("MartyLying.png"));Next();}break;
    case 7:if(SmokeTime>.8){SmokeCheck(D->PetEnergy>SmokeYaw+.8f,TEXT("Lying down restores energy"));Key(EKeys::E,true);Next();}break;
    case 8:if(SmokeTime>.2){Key(EKeys::E,false);SmokeCheck(L->Activity==EDogActivity::Idle,TEXT("Interact gets Marty back up"));Trick(1);Next();}break;
    case 9:if(SmokeTime>.2){Key(EKeys::Enter,false);Next();}break;
    case 10:if(SmokeTime>.6){SmokeCheck(L->Activity==EDogActivity::Rolling&&L->Busy(),TEXT("Roll over is a timed motion with movement locked"));SmokeCheck(FMath::Abs(FRotator::NormalizeAxis(P->Skin->GetBoneRotationByName(TEXT("root"),EBoneSpaces::ComponentSpace).Roll-SmokePitch))>20,TEXT("Rolling visibly rotates the real skinned rig"));Shot(TEXT("MartyRoll.png"));Key(EKeys::Escape,true);Next();}break;
    case 11:if(SmokeTime>.2){Key(EKeys::Escape,false);SmokeYaw=L->ActionProgress();Next();}break;
    case 12:if(SmokeTime>.5){SmokeCheck(Adventure->Screen==ECozyScreen::Pause&&FMath::IsNearlyEqual(L->ActionProgress(),SmokeYaw,.001f),TEXT("Pause freezes a roll in progress"));Key(EKeys::Escape,true);Next();}break;
    case 13:if(SmokeTime>.2){Key(EKeys::Escape,false);Next();}break;
    case 14:if(SmokeTime>2){SmokeCheck(L->Activity==EDogActivity::Idle&&FMath::Abs(FRotator::NormalizeAxis(P->Skin->GetBoneRotationByName(TEXT("root"),EBoneSpaces::ComponentSpace).Roll-SmokePitch))<.1f,TEXT("Completed roll restores the upright rig"));Trick(2);Next();}break;
    case 15:if(SmokeTime>.2){Key(EKeys::Enter,false);Next();}break;
    case 16:if(SmokeTime>.6){SmokeCheck(L->Activity==EDogActivity::Shaking&&P->Skin->GetBoneLocationByName(TEXT("front_l_paw"),EBoneSpaces::ComponentSpace).Z>10,TEXT("Shake raises the actual front paw"));SmokeCheck(P->Gait->PlantedPaws==3,TEXT("Shake retains three supporting paws"));Shot(TEXT("MartyShake.png"));Next();}break;
    case 17:if(SmokeTime>1.3){SmokeCheck(L->Activity==EDogActivity::Idle,TEXT("Shake finishes without leaving controls locked"));Trick(4);Next();}break;
    case 18:if(SmokeTime>.2){Key(EKeys::Enter,false);SmokeCheck(D->PetPace==1&&L->WalkSpeed()==155,TEXT("Tricks menu selects jog pace"));PC->SetControlRotation(FRotator(-12,0,0));Key(EKeys::W,true);Next();}break;
    case 19:if(SmokeTime>.08){SmokeCheck(P->GetVelocity().Size2D()<155,TEXT("Jog uses acceleration rather than instant full velocity"));Next();}break;
    case 20:if(SmokeTime>.7){SmokeCheck(P->GetVelocity().Size2D()>100&&P->GetVelocity().Size2D()<=156,TEXT("Keyboard traversal reaches the jog speed"));L->CyclePace();Next();}break;
    case 21:if(SmokeTime>.8){SmokeCheck(P->GetVelocity().Size2D()>180&&P->GetVelocity().Size2D()<=231,TEXT("Run pace reaches a distinct higher physical speed"));Key(EKeys::W,false);Next();}break;
    case 22:if(SmokeTime>.8){SmokeCheck(P->GetVelocity().Size2D()<1,TEXT("Friction and braking stop Marty after input release"));Trick(5);Next();}break;
    case 23:if(SmokeTime>.2){Key(EKeys::Enter,false);SmokeCheck(D->BodyControls,TEXT("Tricks menu enables the alternative control mode"));SmokeYaw=P->GetActorRotation().Yaw;SmokePosition=P->GetActorLocation();Next();}break;
    case 24:if(SmokeTime>.6){SmokeCheck(L->BodyRoll>2&&L->BodyRoll<=12&&FMath::Abs(L->BodyPitch)<=8,TEXT("Right joystick produces bounded spring-driven body sway"));SmokeCheck(FMath::Abs(FRotator::NormalizeAxis(P->GetActorRotation().Yaw-SmokeYaw))>5,TEXT("Right joystick turns the body with damped angular motion"));SmokeCheck(FVector::Dist2D(P->GetActorLocation(),SmokePosition)>10,TEXT("Both joysticks can drive movement and body sway together"));Shot(TEXT("MartyBodySway.png"));Next();}break;
    case 25:if(SmokeTime>1.2){SmokeCheck(FMath::Abs(L->BodyRoll)<.5f&&FMath::Abs(L->BodyYawSpeed)<2,TEXT("Releasing the joystick lets the spring and turning settle"));Trick(5);Next();}break;
    case 26:if(SmokeTime>.2){Key(EKeys::Enter,false);SmokeCheck(!D->BodyControls&&P->GetCharacterMovement()->bOrientRotationToMovement,TEXT("Standard controls can be restored from the same menu"));L->ToggleBodyControls();L->AdjustHealth(-1000);SmokeCheck(L->Health()==25&&P->GetActorLocation().X<0,TEXT("Zero health recovers safely at home"));L->Stow();D->CarryItem=1;L->Stow();D->PetHealth=62;D->PetEnergy=42;Adventure->Save();SmokeCheck(D->BagItems.Num()==2&&D->CarryItem==-1,TEXT("Backpack can safely hold both available toys"));Adventure->OpenBackpack();Next();}break;
    case 27:if(SmokeTime>.3){Shot(TEXT("MartyBackpack.png"));Next();}break;
    case 28:if(SmokeTime>.4)Finish(false);break;
    }
}
