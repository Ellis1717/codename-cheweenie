#include "MartyCharacter.h"
#include "HockeySurprise.h"
#include "CozyAdventure.h"
#include "CozyModeHub.h"
#include "CatCompanions.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "InputKeyEventArgs.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/Parse.h"
#include "Misc/CommandLine.h"
#include "Misc/App.h"
#include "UnrealClient.h"

void AMartyPlayMode::TickHockeySmoke(float Delta)
{
    SmokeTime+=Delta;auto* PC=GetWorld()->GetFirstPlayerController();auto* P=PC?Cast<AMartyCharacter>(PC->GetPawn()):nullptr;
    if(!P||!Adventure||!Adventure->Progress)return;
    FString Root=FPaths::ProjectSavedDir();FParse::Value(FCommandLine::Get(),TEXT("CheweenieTestOutput="),Root);
    auto Key=[&](FKey K,bool Down){PC->InputKey(FInputKeyEventArgs(nullptr,INPUTDEVICEID_NONE,K,Down?IE_Pressed:IE_Released,FPlatformTime::Cycles64()));};
    auto Next=[&](){++SmokeStage;SmokeTime=0;};
    auto Shot=[&](const TCHAR* N){if(FApp::CanEverRender())FScreenshotRequest::RequestScreenshot(Root/N,true,false);};
    if(SmokeStage==2)PC->InputKey(FInputKeyEventArgs(nullptr,INPUTDEVICEID_NONE,EKeys::Gamepad_LeftY,1.f,Delta,1,FPlatformTime::Cycles64()));
    switch(SmokeStage)
    {
    case 0:if(SmokeTime>(FApp::CanEverRender()?15:3))
        {
            auto* Hub=GetGameInstance()->GetSubsystem<UCozyModeHub>();Adventure->OpenModes();Hub->Selection=7;
            SmokeCheck(Hub->Rows().IsValidIndex(7)&&Hub->Rows()[7].Contains(TEXT("Hockey Bag")),TEXT("Main mode menu exposes Hockey Bag Surprise"));
            Hub->Confirm();Next();
        }break;
    case 1:if(SmokeTime>1)
        {
            SmokeCheck(Hockey->Active()&&Hockey->Stage==0&&Adventure->Screen==ECozyScreen::Playing,TEXT("Mode menu starts the playable truck scene"));
            SmokeCheck(Companions->Cats[0].Actor->IsHidden(),TEXT("Home Lakitu hidden while story Lakitu is in the bag"));
            SmokeCheck(Adventure->Prompt().IsEmpty(),TEXT("Bag interaction requires approaching the truck"));
            SmokePosition=P->GetActorLocation();Key(EKeys::W,true);Next();
        }break;
    case 2:if(SmokeTime>1.8&&(!Adventure->Prompt().IsEmpty()||SmokeTime>4))
        {
            Key(EKeys::W,false);PC->InputKey(FInputKeyEventArgs(nullptr,INPUTDEVICEID_NONE,EKeys::Gamepad_LeftY,0.f,Delta,1,FPlatformTime::Cycles64()));
            SmokeCheck(P->GetActorLocation().X>SmokePosition.X+80&&P->GetCharacterMovement()->IsMovingOnGround(),TEXT("Keyboard and synthetic stick move Marty on the truck loading platform"));
            SmokeCheck(!Adventure->Prompt().IsEmpty(),TEXT("Truck bag prompt becomes available at close range"));
            Shot(TEXT("HockeyTruck.png"));Next();
        }break;
    case 3:if(SmokeTime>1){Key(EKeys::E,true);Key(EKeys::E,false);Next();}break;
    case 4:if(SmokeTime>1)
        {
            SmokeCheck(Hockey->Stage==1&&Adventure->Objective().Contains(TEXT("Learn to Skate")),TEXT("Keyboard E carries story into Leo's dressing room"));
            P->TeleportTo(FVector(-10,0,3040),FRotator::ZeroRotator);Next();
        }break;
    case 5:if(SmokeTime>1){SmokeYaw=Adventure->AudibleCueCounts.FindRef(TEXT("Lakitu"));Key(EKeys::Gamepad_FaceButton_Left,true);Key(EKeys::Gamepad_FaceButton_Left,false);Next();}break;
    case 6:if(SmokeTime>2)
        {
            SmokeCheck(Hockey->Stage==2&&Adventure->Message.Contains(TEXT("You're not my skates")),TEXT("Synthetic X/Square unzips bag and reveals Lakitu with Leo's reaction"));
            SmokeCheck(Adventure->AudibleCueCounts.FindRef(TEXT("Lakitu"))==int32(SmokeYaw)+1,TEXT("Reveal triggers Lakitu's meow cue"));
            SmokeCheck(Adventure->Progress->SnackMask==0&&Adventure->Progress->VisitIndex==0,TEXT("Story does not advance main patrol progress"));
            Shot(TEXT("HockeyDressingRoom.png"));Next();
        }break;
    case 7:if(SmokeTime>.5&&Adventure->Screen==ECozyScreen::Playing)Adventure->Pause();if(SmokeTime>1)
        {
            SmokeCheck(Hockey->Stage==2&&Adventure->Screen==ECozyScreen::Pause,TEXT("Pause retains hockey story stage"));Adventure->Back();Key(EKeys::E,true);Key(EKeys::E,false);Next();
        }break;
    case 8:if(SmokeTime>1)
        {
            SmokeCheck(Hockey->Stage==3,TEXT("Greeting awards the story's honorary mascot finale"));Key(EKeys::E,true);Key(EKeys::E,false);Next();
        }break;
    case 9:if(SmokeTime>1)
        {
            SmokeCheck(!Hockey->Active()&&P->GetActorLocation().Z<200&&!Companions->Cats[0].Actor->IsHidden(),TEXT("Final bag interaction returns Marty and Lakitu home"));
            Hockey->Start();SmokeCheck(Hockey->Stage==0,TEXT("Completed story can be replayed from its beginning"));P->ReturnHome();
            SmokeCheck(!Hockey->Active()&&P->GetActorLocation().Z<200,TEXT("Return-home recovery cancels the mode safely"));
            auto O=MakeShared<FJsonObject>();TArray<TSharedPtr<FJsonValue>> Pass,Fail;for(auto& S:SmokePassed)Pass.Add(MakeShared<FJsonValueString>(S));for(auto& S:SmokeFailed)Fail.Add(MakeShared<FJsonValueString>(S));O->SetArrayField(TEXT("passed"),Pass);O->SetArrayField(TEXT("failed"),Fail);
            FString Json;FJsonSerializer::Serialize(O,TJsonWriterFactory<>::Create(&Json));const bool Saved=FFileHelper::SaveStringToFile(Json,*(Root/TEXT("hockey-smoke.json")));FPlatformMisc::RequestExitWithStatus(false,Saved&&SmokeFailed.IsEmpty()?0:1);SmokeStage=1000;
        }break;
    }
}
