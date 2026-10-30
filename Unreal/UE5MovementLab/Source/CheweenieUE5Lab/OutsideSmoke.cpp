#include "MartyCharacter.h"
#include "Neighbourhood.h"
#include "CozyAdventure.h"
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
void AMartyPlayMode::TickOutsideSmoke(float Delta)
{
    SmokeTime+=Delta;auto* PC=GetWorld()->GetFirstPlayerController();auto* P=PC?Cast<AMartyCharacter>(PC->GetPawn()):nullptr;
    if(!P||!Adventure||!Adventure->Progress||!Neighbourhood)return;auto* N=Neighbourhood.Get();
    FString Root=FPaths::ProjectSavedDir();FParse::Value(FCommandLine::Get(),TEXT("CheweenieTestOutput="),Root);
    auto Key=[PC](FKey K,bool Down){PC->InputKey(FInputKeyEventArgs(nullptr,INPUTDEVICEID_NONE,K,Down?IE_Pressed:IE_Released,FPlatformTime::Cycles64()));};
    auto Next=[this](){++SmokeStage;SmokeTime=0;};
    auto Shot=[&](const TCHAR* Name){if(FApp::CanEverRender())FScreenshotRequest::RequestScreenshot(Root/Name,true,false);};
    if(FParse::Param(FCommandLine::Get(),TEXT("CheweenieOutsideMapAudit")))
    {
        if(SmokeStage==0&&SmokeTime>3)
        {
            SmokeCheck(N->PrivateMapLoaded&&N->RoadSegments>20,TEXT("Retained private streets validate and instantiate without publishing coordinates"));
            SmokeCheck(!N->Park.IsZero()&&!N->School.IsZero()&&N->Houses==9,TEXT("Private park, school and schematic homes are present"));
            Adventure->Start(true);N->GoOutside(P);SmokePosition=P->GetActorLocation();Key(EKeys::W,true);Next();
        }
        else if(SmokeStage==1&&SmokeTime>1.5)
        {
            Key(EKeys::W,false);SmokeCheck(N->Outside&&P->GetCharacterMovement()->IsMovingOnGround()&&FVector::Dist2D(P->GetActorLocation(),SmokePosition)>60,TEXT("Marty walks on the private-map ground"));
            P->ReturnHome();SmokeCheck(!N->Outside,TEXT("Private-map recovery returns to headquarters"));Next();
        }
        else if(SmokeStage==2&&SmokeTime>.2)
        {
            auto O=MakeShared<FJsonObject>();TArray<TSharedPtr<FJsonValue>> Pass,Fail;
            for(auto& S:SmokePassed)Pass.Add(MakeShared<FJsonValueString>(S));for(auto& S:SmokeFailed)Fail.Add(MakeShared<FJsonValueString>(S));
            O->SetArrayField(TEXT("passed"),Pass);O->SetArrayField(TEXT("failed"),Fail);FString Json;FJsonSerializer::Serialize(O,TJsonWriterFactory<>::Create(&Json));
            bool Saved=FFileHelper::SaveStringToFile(Json,*(Root/TEXT("outside-map-audit.json")));UGameplayStatics::DeleteGameInSlot(TEXT("CheweenieOutsideAcceptance"),0);
            FPlatformMisc::RequestExitWithStatus(false,Saved&&SmokeFailed.IsEmpty()?0:1);SmokeStage=100;
        }
        return;
    }
    switch(SmokeStage)
    {
    case 0:
        if(SmokeTime>(FApp::CanEverRender()?40:3))
        {
            SmokeCheck(!N->PrivateMapLoaded&&N->Houses==9&&N->Dogs.Num()==8&&N->RoadSegments>8,TEXT("Schematic exterior builds nine houses, eight dog slots and connected routes"));
            Adventure->Start(true);Adventure->Progress->SnackMask=1;Adventure->Save();PC->SetControlRotation(FRotator(-12,0,0));Key(EKeys::S,true);Next();
        }break;
    case 1:
        if(P->GetActorLocation().X< -335||SmokeTime>4){Key(EKeys::S,false);Next();}break;
    case 2:
        if(SmokeTime>.25){SmokeCheck(N->Prompt(P).Contains(TEXT("open the door")),TEXT("Actual indoor walking reaches the exit door prompt"));Key(EKeys::E,true);Next();}break;
    case 3:
        if(SmokeTime>.7)
        {
            Key(EKeys::E,false);SmokeCheck(N->Outside&&FVector::Dist2D(P->GetActorLocation(),N->Entrance)<120,TEXT("Keyboard interaction opens door and transfers Marty outside onto the front walk"));
            SmokeCheck(P->GetCharacterMovement()->IsMovingOnGround(),TEXT("Outside arrival lands on collision ground"));
            SmokeCheck(Adventure->Progress->SnackMask==1,TEXT("Door transition preserves the indoor mission"));
            Shot(TEXT("OutsideArrival.png"));SmokePosition=P->GetActorLocation();Key(EKeys::W,true);Next();
        }break;
    case 4:
        if(FVector::Dist2D(P->GetActorLocation(),SmokePosition)>100||SmokeTime>4)
        {
            Key(EKeys::W,false);SmokeCheck(FVector::Dist2D(P->GetActorLocation(),SmokePosition)>80&&P->GetCharacterMovement()->IsMovingOnGround(),TEXT("Real movement walks on the larger exterior ground"));
            P->GetCharacterMovement()->StopMovementImmediately();P->TeleportTo(N->Dogs[0].Position+FVector(0,0,45),FRotator::ZeroRotator);Next();
        }break;
    case 5:
        if(SmokeTime>.7){Key(EKeys::Gamepad_FaceButton_Left,true);Next();}break;
    case 6:
        if(SmokeTime>.2)
        {
            Key(EKeys::Gamepad_FaceButton_Left,false);SmokeCheck(N->MetDogs==1&&(Adventure->Progress->NeighbourDogMask&1),TEXT("Controller interaction greets a dog and saves its slot"));
            Adventure->Interact();SmokeCheck(N->MetDogs==1,TEXT("Repeated dog greeting does not duplicate friendship"));
            P->TeleportTo(N->Entrance+FVector(160,0,25),FRotator::ZeroRotator);Next();
        }break;
    case 7:
        if(SmokeTime>.5){Adventure->Interact();Next();}break;
    case 8:
        if(SmokeTime>.6)
        {
            SmokeCheck(FVector::Dist2D(P->GetActorLocation(),N->Park)<80&&P->GetCharacterMovement()->IsMovingOnGround(),TEXT("Working tour board reaches the park destination"));Shot(TEXT("OutsidePark.png"));
            Next();
        }break;
    case 9:
        if(SmokeTime>.2&&!SmokeMoving){P->TeleportTo(N->Park+FVector(300,0,40),FRotator::ZeroRotator);SmokeMoving=true;}
        if(SmokeTime>.7){Adventure->Interact();SmokeMoving=false;Next();}break;
    case 10:
        if(SmokeTime>.6)
        {
            SmokeCheck(FVector::Dist2D(P->GetActorLocation(),N->School)<80&&P->GetCharacterMovement()->IsMovingOnGround(),TEXT("Tour continues to the school exterior"));
            PC->SetControlRotation(FRotator(-8,90,0));Next();
        }break;
    case 11:
        if(SmokeTime>1){Shot(TEXT("OutsideSchool.png"));Next();}break;
    case 12:
        if(SmokeTime>.2&&!SmokeMoving){Key(EKeys::H,true);SmokeMoving=true;}
        if(SmokeTime>.8)
        {
            Key(EKeys::H,false);SmokeMoving=false;SmokeCheck(!N->Outside&&FVector::Dist2D(P->GetActorLocation(),FVector(-200,-200,0))<10,TEXT("H safely returns from the neighbourhood to headquarters"));
            P->TeleportTo(FVector(-345,-200,35),FRotator::ZeroRotator);Next();
        }break;
    case 13:
        if(SmokeTime>.4){Adventure->Interact();Next();}break;
    case 14:
        if(SmokeTime>.5){Key(EKeys::Gamepad_FaceButton_Left,true);Next();}break;
    case 15:
        if(SmokeTime>.4)
        {
            Key(EKeys::Gamepad_FaceButton_Left,false);SmokeCheck(!N->Outside,TEXT("Controller opens the exterior home door to return inside"));
            auto Report=MakeShared<FJsonObject>();auto Strings=[](const TArray<FString>& In){TArray<TSharedPtr<FJsonValue>> Out;for(auto& S:In)Out.Add(MakeShared<FJsonValueString>(S));return Out;};
            Report->SetStringField(TEXT("scope"),TEXT("Actual door and walking input; teleported fixtures for dog/landmark selection; schematic map only; physical devices unverified"));
            Report->SetArrayField(TEXT("passed"),Strings(SmokePassed));Report->SetArrayField(TEXT("failed"),Strings(SmokeFailed));FString Json;FJsonSerializer::Serialize(Report,TJsonWriterFactory<>::Create(&Json));
            bool Saved=FFileHelper::SaveStringToFile(Json,*(Root/TEXT("outside-smoke.json")));UGameplayStatics::DeleteGameInSlot(TEXT("CheweenieOutsideAcceptance"),0);
            FPlatformMisc::RequestExitWithStatus(false,Saved&&SmokeFailed.IsEmpty()?0:1);SmokeStage=100;
        }break;
    }
}
