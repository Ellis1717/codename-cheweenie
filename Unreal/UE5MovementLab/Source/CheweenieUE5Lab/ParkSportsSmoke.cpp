#include "MartyCharacter.h"
#include "ParkSports.h"
#include "Neighbourhood.h"
#include "CozyAdventure.h"
#include "CozyNavigation.h"
#include "Engine/World.h"
#include "Engine/StaticMeshActor.h"
#include "Camera/CameraActor.h"
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

void AMartyPlayMode::TickParkSportsSmoke(float Delta)
{
    SmokeTime+=Delta;auto* PC=GetWorld()->GetFirstPlayerController();auto* P=PC?Cast<AMartyCharacter>(PC->GetPawn()):nullptr;
    if(!P||!Adventure||!Adventure->Progress||!Neighbourhood||!Neighbourhood->Sports)return;
    auto* S=Neighbourhood->Sports.Get();FString Root=FPaths::ProjectSavedDir();FParse::Value(FCommandLine::Get(),TEXT("CheweenieTestOutput="),Root);
    auto Key=[&](FKey K,bool Down){PC->InputKey(FInputKeyEventArgs(nullptr,INPUTDEVICEID_NONE,K,Down?IE_Pressed:IE_Released,FPlatformTime::Cycles64()));};
    auto Tap=[&](FKey K){Key(K,true);Key(K,false);};
    auto Next=[&](){++SmokeStage;SmokeTime=0;};
    auto Place=[&](FVector Pos,float Yaw){P->GetCharacterMovement()->StopMovementImmediately();P->TeleportTo(Pos+FVector(0,0,45),FRotator(0,Yaw,0));PC->SetControlRotation(FRotator(-12,Yaw,0));PC->SetViewTarget(P);if(PC->GetHUD())PC->GetHUD()->bShowHUD=true;};
    auto View=[&](FVector Center){auto* C=GetWorld()->SpawnActor<ACameraActor>(Center+FVector(0,-1700,1900),FRotator(-48,90,0));PC->SetViewTarget(C);if(PC->GetHUD())PC->GetHUD()->bShowHUD=false;};
    auto Shot=[&](const TCHAR* N){if(FApp::CanEverRender())FScreenshotRequest::RequestScreenshot(Root/N,true,false);};
    if(SmokeStage==38)PC->InputKey(FInputKeyEventArgs(nullptr,INPUTDEVICEID_NONE,EKeys::Gamepad_LeftY,1.f,Delta,1,FPlatformTime::Cycles64()));
    if(SmokeStage==32&&S->NextBase>=0)PC->SetControlRotation(FRotator(-12,(S->Base(S->NextBase)-P->GetActorLocation()).Rotation().Yaw,0));
    switch(SmokeStage)
    {
    case 0:if(SmokeTime>(FApp::CanEverRender()?25:3))
        {
            Adventure->Start(true);Neighbourhood->GoOutside(P);
            if(FParse::Param(FCommandLine::Get(),TEXT("CheweenieParkPrivateAudit")))SmokeCheck(Neighbourhood->PrivateMapLoaded,TEXT("Retained private map loads its park anchor without exporting coordinates"));
            SmokeCheck(S->SoccerBall&&S->Baseball&&S->Basketball,TEXT("Cumulative real-map neighbourhood contains all three sports"));
            SmokeCheck(Navigation->Landmarks().Last().Label.Contains(TEXT("basketball")),TEXT("Sports destinations append after existing saved map indices"));
            SmokeCheck(S->GroundHeight(S->Soccer)>S->GroundHeight(S->Soccer+FVector(1000,0,0))+5,TEXT("Soccer field has a gentle crown instead of a flat slab"));
            SmokeCheck(S->GroundHeight(S->Diamond+FVector(0,-100,0))>S->Base(1).Z+20,TEXT("Baseball mound rises smoothly above the base-running circuit"));
            SmokeCheck(S->Court.Z>Neighbourhood->Park.Z+80&&FMath::Abs(S->GroundHeight(S->Court+FVector(700,600,0))-S->Court.Z)<.1f,TEXT("Basketball terrace stays level above its sloping approach"));
            bool Grounded=true;
            for(FVector Offset:{FVector(-2400,5500,0),FVector(2300,5850,0),FVector(600,5000,0),FVector(1700,3300,0)})
            {
                const FVector At=S->GroundPoint(Neighbourhood->Park+Offset);FHitResult Hit;
                Grounded&=GetWorld()->LineTraceSingleByChannel(Hit,At+FVector(0,0,500),At-FVector(0,0,30),ECC_Visibility)&&FMath::Abs(Hit.ImpactPoint.Z-At.Z)<2&&Hit.ImpactNormal.Z>.75f;
            }
            SmokeCheck(Grounded,TEXT("Hills mound and ramp have walkable collision matching the sampled surface"));
            Place(Neighbourhood->Park+FVector(0,1000,0),90);SmokePosition=P->GetActorLocation();Key(EKeys::W,true);Next();
        }break;
    case 1:if(SmokeTime>12)
        {
            UE_LOG(LogTemp,Display,TEXT("PARK_APPROACH distance=%.2f heightGain=%.2f mode=%d"),FVector::Dist2D(SmokePosition,P->GetActorLocation()),P->GetActorLocation().Z-SmokePosition.Z,int(P->GetCharacterMovement()->MovementMode));
            Key(EKeys::W,false);SmokeCheck(P->GetActorLocation().Y>SmokePosition.Y+900&&P->GetCharacterMovement()->IsMovingOnGround(),TEXT("Keyboard walks continuously from original park through old boundary into sports park"));
            Place(S->Soccer-FVector(70,0,0),0);Next();
        }break;
    case 2:if(SmokeTime>.5){SmokeCheck(Neighbourhood->Prompt(P).Contains(TEXT("kick")),TEXT("Approaching soccer ball displays kick controls"));Tap(EKeys::E);Next();}break;
    case 3:if(SmokeTime>.3){SmokeCheck(FMath::Abs(S->SoccerBall->GetActorLocation().Z-S->GroundHeight(S->SoccerBall->GetActorLocation())-16)<.1f,TEXT("Moving soccer ball follows the field crown without floating or burying"));Adventure->Pause();SmokePosition=S->SoccerBall->GetActorLocation();Next();}break;
    case 4:if(SmokeTime>.6){SmokeCheck(S->SoccerBall->GetActorLocation().Equals(SmokePosition,.01),TEXT("Pause freezes live soccer motion"));Adventure->Back();Next();}break;
    case 5:if(SmokeTime>2)
        {
            SmokeCheck(S->Goals[0]==1&&S->Goals[1]==0,TEXT("Keyboard kick crosses right goal mouth and awards exactly one goal"));
            Place(S->Soccer+FVector(70,0,0),180);Next();
        }break;
    case 6:if(SmokeTime>.5){Tap(EKeys::Gamepad_FaceButton_Left);Next();}break;
    case 7:if(SmokeTime>2)
        {
            SmokeCheck(S->Goals[0]==1&&S->Goals[1]==1,TEXT("Synthetic X/Square kick scores through opposite goal"));
            SmokeCheck(FMath::Abs(S->SoccerBall->GetActorLocation().Z-S->GroundHeight(S->SoccerBall->GetActorLocation())-16)<.1f,TEXT("Soccer ball rests on contoured ground after scoring"));
            S->SoccerBall->SetActorLocation(S->Soccer+FVector(0,1050,16));Place(S->Soccer+FVector(0,990,0),90);Next();
        }break;
    case 8:if(SmokeTime>.4){Tap(EKeys::E);Next();}break;
    case 9:if(SmokeTime>.8)
        {
            SmokeCheck(FVector::Dist2D(S->SoccerBall->GetActorLocation(),S->Soccer)<1&&S->Goals[0]==1&&S->Goals[1]==1,TEXT("Touchline miss resets ball without awarding a goal"));
            Place(S->Soccer+FVector(0,-500,0),0);SmokeCheck(!Neighbourhood->Interact(P),TEXT("Sports interaction cannot kick from far away"));
            Place(S->Soccer+FVector(0,0,400),0);SmokeCheck(S->Prompt(P).IsEmpty(),TEXT("Sports interaction rejects wrong-height fixtures"));
            Place(S->Soccer+FVector(0,-700,0),90);View(S->Soccer);Next();
        }break;
    case 10:if(SmokeTime>2){Shot(TEXT("ParkSoccer.png"));Next();}break;
    case 11:if(SmokeTime>.7){Place(S->Base(0)+FVector(0,-70,0),90);Next();}break;
    case 12:if(SmokeTime>.5){Tap(EKeys::E);Next();}break;
    case 13:if(SmokeTime>.5)
        {
            SmokeCheck(S->NextBase==1&&S->Baseball->GetActorLocation().Z>60,TEXT("Keyboard batting launches baseball and starts ordered base run"));
            Place(S->Base(2),90);Next();
        }break;
    case 14:if(SmokeTime>.5)
        {
            SmokeCheck(S->NextBase==1&&S->Runs==0,TEXT("Skipping first base cannot advance or score"));Place(S->Base(1),45);Next();
        }break;
    case 15:if(SmokeTime>.4)
        {
            SmokeCheck(S->NextBase==2,TEXT("Touching first advances to second"));
            SmokePosition=P->GetActorLocation();Key(EKeys::W,true);Next();
        }break;
    case 16:if(SmokeTime>2)
        {
            Key(EKeys::W,false);SmokeCheck(FVector::Dist2D(P->GetActorLocation(),SmokePosition)>100&&P->GetCharacterMovement()->IsMovingOnGround(),TEXT("Marty runs on colliding baseball infield with keyboard"));Place(S->Base(2),90);Next();
        }break;
    case 17:if(SmokeTime>.4){SmokeCheck(S->NextBase==3,TEXT("Second base advances to third"));Place(S->Base(3),90);Next();}break;
    case 18:if(SmokeTime>.4){SmokeCheck(S->NextBase==0,TEXT("Third base requires returning home"));Place(S->Base(0),90);Next();}break;
    case 19:if(SmokeTime>.4)
        {
            SmokeCheck(S->Runs==1&&S->NextBase<0,TEXT("Ordered complete diamond awards one run and resets tee"));View(S->Diamond);Next();
        }break;
    case 20:if(SmokeTime>2){Shot(TEXT("ParkBaseball.png"));Next();}break;
    case 21:if(SmokeTime>.7){Place(S->Court+FVector(0,-520,0),90);Next();}break;
    case 22:if(SmokeTime>.5){Tap(EKeys::Gamepad_FaceButton_Left);Next();}break;
    case 23:if(SmokeTime>1.6)
        {
            SmokeCheck(S->Baskets==1,TEXT("Synthetic controller shot aimed at rim scores one basket"));
            Place(S->Court+FVector(0,-520,0),0);Next();
        }break;
    case 24:if(SmokeTime>.5){Tap(EKeys::E);Next();}break;
    case 25:if(SmokeTime>1.6)
        {
            SmokeCheck(S->Baskets==1,TEXT("Misaimed basketball shot misses without adding score"));
            Place(S->ResetPoint(2)+FVector(70,0,0),90);Next();
        }break;
    case 26:if(SmokeTime>.5)
        {
            Tap(EKeys::Gamepad_FaceButton_Left);SmokeCheck(S->Baskets==1&&FVector::Dist2D(S->Basketball->GetActorLocation(),S->Court+FVector(0,-450,0))<1,TEXT("Controller reset bollard retrieves ball and retains score"));
            SmokeCheck(Navigation->Location().Contains(TEXT("basketball")),TEXT("Navigation identifies basketball court"));View(S->Court);Next();
        }break;
    case 27:if(SmokeTime>2){Shot(TEXT("ParkBasketball.png"));Next();}break;
    case 28:if(SmokeTime>.7){Place(S->Base(0),90);Next();}break;
    case 29:if(SmokeTime>.5){Tap(EKeys::Gamepad_FaceButton_Left);Place(S->Diamond+FVector(1100,0,0),90);Next();}break;
    case 30:if(SmokeTime>27)
        {
            SmokeCheck(S->NextBase<0&&S->Runs==1,TEXT("Baseball timeout resets tee without a free run"));
            Place(S->Base(0),90);Next();
        }break;
    case 31:if(SmokeTime>.5){Tap(EKeys::Gamepad_FaceButton_Left);Key(EKeys::Gamepad_FaceButton_Bottom,true);Key(EKeys::W,true);Next();}break;
    case 32:if(S->Runs>=2||SmokeTime>24)
        {
            Key(EKeys::W,false);Key(EKeys::Gamepad_FaceButton_Bottom,false);
            SmokeCheck(S->Runs==2&&S->NextBase<0,TEXT("Real hurry movement completes all four bases within 25 seconds without base teleports"));
            if(Neighbourhood->PrivateMapLoaded){SmokeStage=34;SmokeTime=0;}
            else{Place(Neighbourhood->Park+FVector(0,8000,0),90);Key(EKeys::W,true);Next();}
        }break;
    case 33:if(SmokeTime>6)
        {
            Key(EKeys::W,false);
            SmokeCheck(Neighbourhood->Outside&&P->GetCharacterMovement()->IsMovingOnGround()&&P->GetActorLocation().Y>Neighbourhood->Park.Y+8300&&P->GetActorLocation().Y<Neighbourhood->Park.Y+8600,TEXT("Expanded public park stays grounded to its enclosing hedge without falling through a terrain gap"));Next();
        }break;
    case 34:if(SmokeTime>.3)
        {
            Place(S->GroundPoint(Neighbourhood->Park+FVector(-2400,5000,0)),90);SmokePosition=P->GetActorLocation();Key(EKeys::W,true);Next();
        }break;
    case 35:if(SmokeTime>6)
        {
            UE_LOG(LogTemp,Display,TEXT("PARK_UPHILL distance=%.2f heightGain=%.2f mode=%d"),FVector::Dist2D(SmokePosition,P->GetActorLocation()),P->GetActorLocation().Z-SmokePosition.Z,int(P->GetCharacterMovement()->MovementMode));
            Key(EKeys::W,false);SmokeCheck(P->GetActorLocation().Z>SmokePosition.Z+65&&P->GetCharacterMovement()->IsMovingOnGround(),TEXT("Keyboard walks uphill with actual collision and gains elevation"));
            SmokePosition=P->GetActorLocation();PC->SetControlRotation(FRotator(-12,270,0));Key(EKeys::W,true);Next();
        }break;
    case 36:if(SmokeTime>6)
        {
            Key(EKeys::W,false);SmokeCheck(P->GetActorLocation().Z<SmokePosition.Z-65&&P->GetCharacterMovement()->IsMovingOnGround(),TEXT("Keyboard descends the hill without falling through terrain"));
            Place(S->GroundPoint(Neighbourhood->Park+FVector(0,4800,0)),90);Next();
        }break;
    case 37:if(SmokeTime>.5){SmokePosition=P->GetActorLocation();Next();}break;
    case 38:if(SmokeTime>7)
        {
            PC->InputKey(FInputKeyEventArgs(nullptr,INPUTDEVICEID_NONE,EKeys::Gamepad_LeftY,0.f,Delta,1,FPlatformTime::Cycles64()));
            SmokeCheck(P->GetActorLocation().Z>SmokePosition.Z+50&&P->GetCharacterMovement()->IsMovingOnGround(),TEXT("Synthetic left stick climbs the continuous court approach"));
            auto* C=GetWorld()->SpawnActor<ACameraActor>();const FVector Eye=Neighbourhood->Park+FVector(-4500,4300,1000),Target=Neighbourhood->Park+FVector(0,5400,90);
            C->SetActorLocationAndRotation(Eye,(Target-Eye).Rotation());PC->SetViewTarget(C);if(PC->GetHUD())PC->GetHUD()->bShowHUD=false;Next();
        }break;
    case 39:if(SmokeTime>2){Shot(TEXT("ParkTerrain.png"));Next();}break;
    case 40:if(SmokeTime>.7){Place(S->GroundPoint(Neighbourhood->Park+FVector(-2300,5700,0)),-70);Next();}break;
    case 41:if(SmokeTime>2){Shot(TEXT("ParkHillFollow.png"));Next();}break;
    case 42:if(SmokeTime>.7)
        {
            SmokeCheck(Adventure->Progress->SnackMask==0&&Adventure->Progress->VisitIndex==0,TEXT("Sports preserve indoor patrol progress"));
            P->ReturnHome();SmokeCheck(!Neighbourhood->Outside&&P->GetActorLocation().Z<200,TEXT("Return-home recovery works from sports park"));
            SmokeCheck(Adventure->MissionValue(TEXT("soccer"))==1&&Adventure->MissionValue(TEXT("baseball"))==1&&Adventure->MissionValue(TEXT("basketball"))==1,TEXT("Actual scored soccer, baseball and basketball events persist mission objectives"));
            SmokeCheck(Adventure->MissionPercent(TEXT("sports"))==100&&Adventure->Progress->FinishedOpenMissions.Contains(TEXT("sports")),TEXT("Park Playdate completes once from real sports gameplay"));
            auto O=MakeShared<FJsonObject>();TArray<TSharedPtr<FJsonValue>> Pass,Fail;for(auto& C:SmokePassed)Pass.Add(MakeShared<FJsonValueString>(C));for(auto& C:SmokeFailed)Fail.Add(MakeShared<FJsonValueString>(C));
            O->SetStringField(TEXT("scope"),TEXT("Real game map; synthetic keyboard/gamepad; fixture teleports for individual cases; real movement for park approach, hills, court ramp and a complete timed baseball circuit; camera steering toward each base; elevated HUD-free field captures and actual hill follow camera; physical hardware unverified"));
            O->SetBoolField(TEXT("rendered"),FApp::CanEverRender());O->SetBoolField(TEXT("privateMap"),Neighbourhood->PrivateMapLoaded);
            O->SetArrayField(TEXT("passed"),Pass);O->SetArrayField(TEXT("failed"),Fail);FString Json;FJsonSerializer::Serialize(O,TJsonWriterFactory<>::Create(&Json));
            bool Saved=FFileHelper::SaveStringToFile(Json,*(Root/TEXT("park-sports-smoke.json")));FPlatformMisc::RequestExitWithStatus(false,Saved&&SmokeFailed.IsEmpty()?0:1);SmokeStage=1000;
        }break;
    }
}
