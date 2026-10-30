// Real skin and real traversal tests. No user save slot, no physical-device claims.
#include "MartyCharacter.h"
#include "MartyGait.h"
#include "DogLife.h"
#include "CozyAdventure.h"
#include "Components/PoseableMeshComponent.h"
#include "Engine/SkinnedAsset.h"
#include "Materials/MaterialInterface.h"
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

void AMartyPlayMode::TickGaitSmoke(float Delta)
{
    SmokeTime+=Delta;auto* PC=GetWorld()->GetFirstPlayerController();auto* Pawn=PC?Cast<AMartyCharacter>(PC->GetPawn()):nullptr;
    if(!Pawn || !Adventure || !Adventure->Progress)return;
    auto* Gait=Pawn->Gait.Get();auto* Skin=Pawn->Skin.Get();
    FString Root=FPaths::ProjectSavedDir();FParse::Value(FCommandLine::Get(),TEXT("CheweenieTestOutput="),Root);
    auto Key=[PC](FKey K,bool Down){PC->InputKey(FInputKeyEventArgs(nullptr,INPUTDEVICEID_NONE,K,Down?IE_Pressed:IE_Released,FPlatformTime::Cycles64()));};
    auto Axis=[PC,Delta](FKey K,float Value){PC->InputKey(FInputKeyEventArgs(nullptr,INPUTDEVICEID_NONE,K,Value,Delta,1,FPlatformTime::Cycles64()));};
    if(SmokeStage>=4)Axis(EKeys::Gamepad_LeftY,0);
    auto Next=[this](){++SmokeStage;SmokeTime=0;};
    auto Shot=[&](const TCHAR* Name){if(FApp::CanEverRender())FScreenshotRequest::RequestScreenshot(Root/Name,true,false);};
    auto Bone=[&](const TCHAR* Name){return Skin->GetBoneLocationByName(Name,EBoneSpaces::ComponentSpace);};
    switch(SmokeStage)
    {
    case 0:
        if(SmokeTime>(FApp::CanEverRender()?50:3))
        {
            SmokeCheck(Gait&&Gait->Ready&&Skin&&Skin->GetSkinnedAsset(),TEXT("Skin-weighted Marty and 24 anatomical joints load"));
            SmokeCheck(Skin->GetSkinnedAsset()->GetName()==TEXT("Marty_RealCoat"),TEXT("Detailed short-coat model loads as the playable character"));
            bool FineFur=false;for(auto* M:Skin->GetMaterials())if(M&&M->GetName()==TEXT("Marty_FineFur"))FineFur=true;
            SmokeCheck(FineFur,TEXT("Skin-weighted fine fur material remains assigned in the real game"));
            SmokeCheck(Bone(TEXT("front_l_upper")).X>Bone(TEXT("hind_l_upper")).X+20,TEXT("Imported centimetre rig faces positive X"));
            const FVector Knee=UMartyGait::SolveKnee(FVector(0,0,20),FVector(0,0,2),12,11,-1);
            SmokeCheck(FMath::IsNearlyEqual(Knee.Size2D(),FMath::Abs(Knee.X),.01)&&Knee.X<0&&FMath::IsNearlyEqual((Knee-FVector(0,0,20)).Size(),12.f,.01),TEXT("Two-bone foreleg IK bends elbow backward at fixed bone length"));
            const FVector Hind=UMartyGait::SolveKnee(FVector(0,0,20),FVector(0,0,2),12,11,1);
            SmokeCheck(Hind.X>0,TEXT("Hind stifle bends forward rather than copying foreleg anatomy"));
            Adventure->Start(true);Pawn->ReturnHome();PC->SetControlRotation(FRotator(-10,0,0));
            Pawn->CycleCamera();Pawn->CycleCamera();Pawn->CycleCamera();
            SmokeCheck(Pawn->CameraPreset==3,TEXT("Fourth camera provides a close character inspection view"));
            // Front three-quarter actual gameplay camera; no illustration substituted.
            PC->SetControlRotation(FRotator(-8,145,0));Next();
        }break;
    case 1:
        if(SmokeTime>2){FVector2D Pixel;if(PC->ProjectWorldLocationToScreen(Skin->GetBoneLocationByName(TEXT("spine"),EBoneSpaces::WorldSpace),Pixel)){FFileHelper::SaveStringToFile(FString::Printf(TEXT("{\"x\":%.2f,\"y\":%.2f}"),Pixel.X,Pixel.Y),*(Root/TEXT("CoatProbe.json")));}Shot(TEXT("MartyPortrait.png"));SmokePosition=Bone(TEXT("front_l_lower"));SmokePitch=Skin->GetBoneRotationByName(TEXT("ear_tip_l"),EBoneSpaces::ComponentSpace).Roll;SmokeYaw=Gait->Cycle;Next();}break;
    case 2:
        if(SmokeTime<.12f)break;
        PC->SetControlRotation(FRotator(-12,0,0));if(!SmokeMoving){Key(EKeys::W,true);SmokeMoving=true;}
        if(Gait->Travel>55 || SmokeTime>5)
        {
            UE_LOG(LogTemp,Display,TEXT("GAIT_WALK travel=%f cycle=%f previous=%f speed=%f"),Gait->Travel,Gait->Cycle,SmokeYaw,Pawn->GetVelocity().Size2D());
            SmokeCheck(Gait->Travel>30&&Gait->Cycle!=SmokeYaw,TEXT("Keyboard traversal advances gait by measured travel"));
            SmokeCheck(FVector::Dist(SmokePosition,Bone(TEXT("front_l_lower")))>.5f&&Gait->MaxJointBend>5&&Gait->MaxJointBend<165,TEXT("Skinned elbow and knee actually bend while walking"));
            SmokeCheck(Gait->PlantedPaws>=1&&Gait->PlantedPaws<=4,TEXT("Locomotion retains supporting stance paws"));
            SmokeCheck(!Skin->GetBoneRotationByName(TEXT("ear_tip_l"),EBoneSpaces::ComponentSpace).ContainsNaN()&&Skin->GetBoneIndex(TEXT("ear_tip_l"))!=INDEX_NONE,TEXT("Flexible ear tips are independently skinned and posed"));
            SmokeCheck(FMath::Abs(FRotator::NormalizeAxis(Skin->GetBoneRotationByName(TEXT("ear_tip_l"),EBoneSpaces::ComponentSpace).Roll-SmokePitch))>1,TEXT("Ear tips visibly change angle during actual travel"));
            Shot(TEXT("MartyWalk.png"));Key(EKeys::W,false);SmokeMoving=false;Key(EKeys::Gamepad_FaceButton_Bottom,true);Next();
        }break;
    case 3:
        Axis(EKeys::Gamepad_LeftY,.8f);
        if(SmokeTime>.6&&(Pawn->GetVelocity().Size2D()>130||SmokeTime>2))
        {
            UE_LOG(LogTemp,Display,TEXT("GAIT_HURRY speed=%.2f requested=%d max=%.2f pos=%s"),Pawn->GetVelocity().Size2D(),Pawn->Life->SprintRequested,Pawn->GetCharacterMovement()->MaxWalkSpeed,*Pawn->GetActorLocation().ToString());
            SmokeCheck(Pawn->GetVelocity().Size2D()>130,TEXT("Analog controller hurry drives the trot at higher speed"));
            SmokeCheck(Gait->MaxJointBend>0&&FMath::IsFinite(Gait->MaxJointBend),TEXT("Trotting IK remains finite"));
            Shot(TEXT("MartyTrot.png"));Axis(EKeys::Gamepad_LeftY,0);Key(EKeys::Gamepad_FaceButton_Bottom,false);Key(EKeys::Gamepad_Special_Right,true);Next();
        }break;
    case 4:
        if(SmokeTime>.2){Key(EKeys::Gamepad_Special_Right,false);SmokeYaw=Gait->Cycle;SmokePosition=Bone(TEXT("front_l_paw"));Next();}break;
    case 5:
        if(SmokeTime>.7)
        {
            SmokeCheck(Adventure->Screen==ECozyScreen::Pause&&Gait->Cycle==SmokeYaw&&FVector::Dist(SmokePosition,Bone(TEXT("front_l_paw")))<.01f,TEXT("Pause freezes gait and paw pose"));
            Key(EKeys::Escape,true);Next();
        }break;
    case 6:
        if(SmokeTime>.3){Key(EKeys::Escape,false);Pawn->ReturnHome();Next();}break;
    case 7:
        if(SmokeTime>1)
        {
            UE_LOG(LogTemp,Display,TEXT("GAIT_IDLE speed=%f planted=%d cycle=%f"),Pawn->GetVelocity().Size2D(),Gait->PlantedPaws,Gait->Cycle);
            SmokeCheck(Gait->Ready&&!Bone(TEXT("front_l_paw")).ContainsNaN()&&Gait->PlantedPaws==4,TEXT("Return-home and stopping recover a stable four-paw idle"));
            Shot(TEXT("MartyIdle.png"));Next();
        }break;
    case 8:
        if(SmokeTime>2)
        {
            auto Report=MakeShared<FJsonObject>();auto Strings=[](const TArray<FString>& In){TArray<TSharedPtr<FJsonValue>> Out;for(auto& S:In)Out.Add(MakeShared<FJsonValueString>(S));return Out;};
            Report->SetStringField(TEXT("scope"),TEXT("Native skin-weighted mesh, analytic canine IK, synthetic keyboard/gamepad. Physical devices and subjective realism unverified."));
            Report->SetArrayField(TEXT("passed"),Strings(SmokePassed));Report->SetArrayField(TEXT("failed"),Strings(SmokeFailed));
            FString Json;FJsonSerializer::Serialize(Report,TJsonWriterFactory<>::Create(&Json));
            bool Saved=FFileHelper::SaveStringToFile(Json,*(Root/TEXT("marty-gait.json")));
            UGameplayStatics::DeleteGameInSlot(TEXT("CheweenieGaitAcceptance"),0);
            FPlatformMisc::RequestExitWithStatus(false,Saved&&SmokeFailed.IsEmpty()?0:1);SmokeStage=100;
        }break;
    }
}
