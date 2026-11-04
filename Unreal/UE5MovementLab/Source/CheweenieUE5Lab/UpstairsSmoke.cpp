#include "MartyCharacter.h"
#include "HouseUpstairs.h"
#include "CozyAdventure.h"
#include "CozyNavigation.h"
#include "DogLife.h"
#include "HomeProgression.h"
#include "Neighbourhood.h"
#include "EngineUtils.h"
#include "Engine/StaticMeshActor.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/HUD.h"
#include "InputKeyEventArgs.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/Parse.h"
#include "Misc/CommandLine.h"
#include "Misc/App.h"
#include "UnrealClient.h"

void AMartyPlayMode::TickUpstairsSmoke(float Delta)
{
    SmokeTime+=Delta;auto* PC=GetWorld()->GetFirstPlayerController();auto* P=PC?Cast<AMartyCharacter>(PC->GetPawn()):nullptr;
    if(!P||!Adventure||!Adventure->Progress||!P->Life||!HomeProgression)return;
    auto* D=Adventure->Progress.Get();auto* L=P->Life.Get();
    FString Root=FPaths::ProjectSavedDir();FParse::Value(FCommandLine::Get(),TEXT("CheweenieTestOutput="),Root);
    auto Key=[&](FKey K,bool Down){PC->InputKey(FInputKeyEventArgs(nullptr,INPUTDEVICEID_NONE,K,Down?IE_Pressed:IE_Released,FPlatformTime::Cycles64()));};
    auto Axis=[&](FKey K,float V){PC->InputKey(FInputKeyEventArgs(nullptr,INPUTDEVICEID_NONE,K,V,Delta,1,FPlatformTime::Cycles64()));};
    auto Next=[&](){++SmokeStage;SmokeTime=0;};
    auto Place=[&](FVector V,float Yaw=0){P->GetCharacterMovement()->StopMovementImmediately();P->TeleportTo(V,FRotator(0,Yaw,0));PC->SetControlRotation(FRotator(-12,Yaw,0));};
    auto View=[&](FVector Eye,FVector Focus){auto* C=GetWorld()->SpawnActor<ACameraActor>(Eye,(Focus-Eye).Rotation());C->GetCameraComponent()->FieldOfView=75;PC->SetViewTarget(C);if(PC->GetHUD())PC->GetHUD()->bShowHUD=false;};
    auto Shot=[&](const TCHAR* Name){if(FApp::CanEverRender())FScreenshotRequest::RequestScreenshot(Root/Name,true,false);};
    // Traversal is driven by the real bindings; only the starting positions are fixtures.
    if(SmokeStage==2)Axis(EKeys::Gamepad_LeftY,1);
    if(SmokeStage==4)Axis(EKeys::Gamepad_LeftY,1);
    switch(SmokeStage)
    {
    case 0:
        if(SmokeTime>(FApp::CanEverRender()?12:3))
        {
            Adventure->Start(true);D->PetEnergy=100;
            int32 Steps=0,Beds=0;for(TActorIterator<AStaticMeshActor> It(GetWorld());It;++It){if(It->ActorHasTag(TEXT("HouseStair")))++Steps;if(It->ActorHasTag(TEXT("HouseBedFrame")))++Beds;}
            SmokeCheck(Steps==38&&Beds==2,TEXT("Real map contains a 38-step staircase and two furnished bedrooms"));
            const auto Stops=Navigation->Landmarks();D->NavTarget=Stops.Num()-4;
            SmokeCheck(Navigation->Goal().Label.Contains(TEXT("Walk upstairs")),TEXT("Bedroom guidance leads downstairs Marty to the stairs"));
            Place(CheweenieHouse::StairBottom());Key(EKeys::W,true);Next();
        }break;
    case 1:
        if(SmokeTime>8.8)
        {
            Key(EKeys::W,false);SmokeCheck(P->GetActorLocation().X>405&&P->GetActorLocation().Z>400&&P->GetCharacterMovement()->IsMovingOnGround(),TEXT("Keyboard walks the entire staircase up without jumping or teleporting"));
            PC->SetControlRotation(FRotator(-12,90,0));Next();
        }break;
    case 2:
        if(SmokeTime>1.9)
        {
            Axis(EKeys::Gamepad_LeftY,0);SmokeCheck(P->GetActorLocation().Y> -330&&CheweenieHouse::Upstairs(P->GetActorLocation()),TEXT("Gamepad stick turns off the stairs onto the upstairs landing"));
            Place(FVector(0,-220,410));Key(EKeys::W,true);PC->SetControlRotation(FRotator(-12,180,0));Next();
        }break;
    case 3:
        if(SmokeTime>2.2){Key(EKeys::W,false);SmokeCheck(P->GetActorLocation().X< -170&&P->GetActorLocation().Z>400,TEXT("Keyboard crosses the landing to the main bedroom approach"));Place(FVector(-160,30,410));PC->SetControlRotation(FRotator(-12,90,0));Next();}break;
    case 4:
        if(SmokeTime>1.2){Axis(EKeys::Gamepad_LeftY,0);SmokeCheck(Navigation->Location().Contains(TEXT("main bedroom")),TEXT("Main bedroom has a walkable floor and correct location label"));Place(FVector(160,30,410));Next();}break;
    case 5:
        if(SmokeTime>.5)
        {
            SmokeCheck(P->GetCharacterMovement()->IsMovingOnGround()&&Navigation->Location().Contains(TEXT("guest bedroom")),TEXT("Guest bedroom has a solid floor and correct location label"));
            Place(FVector(0,250,410),90);Key(EKeys::W,true);Next();
        }break;
    case 6:
        if(SmokeTime>1.3){Key(EKeys::W,false);SmokeCheck(P->GetActorLocation().Y>325&&Navigation->Location().Contains(TEXT("bathroom")),TEXT("Bathroom doorway is physically walkable"));Place(FVector(-350,-345,410));Next();}break;
    case 7:
        if(SmokeTime>.3)
        {
            SmokeCheck(!HomeProgression->Interact()&&!HomeProgression->Prompt().Contains(TEXT("rest in")),TEXT("Upstairs Marty cannot use the downstairs dog bed through the floor"));
            Place(FVector(-180,320,410));SmokeCheck(!L->Interact()&&!L->Prompt().Contains(TEXT("eat kitchen")),TEXT("Upstairs Marty cannot eat from the downstairs bowl"));
            Place(FVector(70,-310,410));SmokeCheck(!L->Interact()&&D->CarryItem==-1,TEXT("Upstairs Marty cannot pick up Rainbow Guy through the floor"));
            Place(FVector(-425,-200,410));SmokeCheck(!Neighbourhood->Interact(P),TEXT("Upstairs Marty cannot activate the downstairs outside door"));
            Place(FVector(410,350,410));SmokeCheck(!HomeProgression->Interact(),TEXT("Upstairs Marty cannot activate the garage portal through the floor"));
            const int32 Snacks=D->SnackMask;Place(FVector(-200,-200,410));SmokePosition=P->GetActorLocation();SmokeYaw=Snacks;Next();
        }break;
    case 8:
        if(SmokeTime>.4)
        {
            SmokeCheck(D->SnackMask==int32(SmokeYaw),TEXT("Upstairs movement does not collect downstairs snacks"));
            D->NavTarget=1;SmokeCheck(Navigation->Goal().Label.Contains(TEXT("Walk downstairs")),TEXT("Ground-floor guidance routes upstairs Marty back to the staircase"));
            Place(FVector(0,200,410));Key(EKeys::W,true);Next();
        }break;
    case 9:
        if(SmokeTime>1.3){Key(EKeys::W,false);SmokeCheck(P->GetActorLocation().X<70,TEXT("Bedroom partition blocks Marty outside its open doorway"));Place(FVector(435,-290,410),-90);Key(EKeys::W,true);Next();}break;
    case 10:
        if(SmokeTime>1.3){Key(EKeys::W,false);PC->SetControlRotation(FRotator(-12,180,0));Key(EKeys::W,true);Next();}break;
    case 11:
        if(SmokeTime>8.8){Key(EKeys::W,false);SmokeCheck(P->GetActorLocation().X< -350&&P->GetActorLocation().Z<70&&P->GetCharacterMovement()->IsMovingOnGround(),TEXT("Keyboard walks back down all stairs onto the original ground floor"));Place(FVector(0,0,410));Key(EKeys::H,true);Next();}break;
    case 12:
        if(SmokeTime>.3)
        {
            Key(EKeys::H,false);SmokeCheck(P->GetActorLocation().Z<100&&P->GetActorLocation().X<0,TEXT("Return-home binding safely recovers Marty from upstairs"));
            Place(FVector(0,-220,410),90);PC->SetViewTarget(P);if(PC->GetHUD())PC->GetHUD()->bShowHUD=false;
            SmokeStage=19;SmokeTime=0;
        }break;
    case 13:
        if(SmokeTime>2){Shot(TEXT("UpstairsOverview.png"));Next();}break;
    case 14:
        if(SmokeTime>.5){for(TActorIterator<AStaticMeshActor> It(GetWorld());It;++It)if(It->ActorHasTag(TEXT("HouseUpperCeiling")))It->SetActorHiddenInGame(false);View(FVector(-140,-75,570),FVector(-300,150,430));Next();}break;
    case 15:
        if(SmokeTime>1.5){Shot(TEXT("UpstairsBedroom.png"));Next();}break;
    case 16:
        if(SmokeTime>.5){View(FVector(120,-260,175),FVector(-70,-420,185));Next();}break;
    case 17:
        if(SmokeTime>1.5){Shot(TEXT("UpstairsStairs.png"));Next();}break;
    case 18:
        if(SmokeTime>.5)
        {
            auto O=MakeShared<FJsonObject>();TArray<TSharedPtr<FJsonValue>> Pass,Fail;for(auto& S:SmokePassed)Pass.Add(MakeShared<FJsonValueString>(S));for(auto& S:SmokeFailed)Fail.Add(MakeShared<FJsonValueString>(S));
            O->SetStringField(TEXT("scope"),TEXT("Real-map collision and synthetic keyboard/gamepad traversal. Teleported starting fixtures. Physical devices unverified. Overview ceiling hidden for review only."));O->SetArrayField(TEXT("passed"),Pass);O->SetArrayField(TEXT("failed"),Fail);
            FString Json;FJsonSerializer::Serialize(O,TJsonWriterFactory<>::Create(&Json));const bool Saved=FFileHelper::SaveStringToFile(Json,*(Root/TEXT("upstairs-smoke.json")));
            FPlatformMisc::RequestExitWithStatus(false,Saved&&SmokeFailed.IsEmpty()?0:1);SmokeStage=1000;
        }break;
    case 19:
        if(SmokeTime>2){Shot(TEXT("UpstairsGameplay.png"));Next();}break;
    case 20:
        if(SmokeTime>.5)
        {
            // The overview alone uses a cutaway roof. Other captures retain normal lighting.
            for(TActorIterator<AStaticMeshActor> It(GetWorld());It;++It)if(It->ActorHasTag(TEXT("HouseUpperCeiling")))It->SetActorHiddenInGame(true);
            View(FVector(0,-470,1250),FVector(0,50,380));SmokeStage=13;SmokeTime=0;
        }break;
    }
}
