#include "MartyCharacter.h"
#include "CozyAdventure.h"
#include "Neighbourhood.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/StaticMesh.h"
#include "Engine/Texture2D.h"
#include "EngineUtils.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "InputKeyEventArgs.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/Parse.h"
#include "Misc/CommandLine.h"
#include "UnrealClient.h"
#include "Kismet/GameplayStatics.h"

void AMartyPlayMode::TickWorldSmoke(float Delta)
{
    SmokeTime+=Delta;auto* PC=GetWorld()->GetFirstPlayerController();auto* P=PC?Cast<AMartyCharacter>(PC->GetPawn()):nullptr;if(!P||!Adventure||!Adventure->Progress||!Neighbourhood)return;
    FString Root=FPaths::ProjectSavedDir();FParse::Value(FCommandLine::Get(),TEXT("CheweenieTestOutput="),Root);
    auto Next=[&](){++SmokeStage;SmokeTime=0;};
    auto Shot=[&](const TCHAR* Name){if(FApp::CanEverRender())FScreenshotRequest::RequestScreenshot(Root/Name,true,false);};
    auto View=[&](FVector Position,FVector Target){auto* Camera=GetWorld()->SpawnActor<ACameraActor>(Position,(Target-Position).Rotation());Camera->GetCameraComponent()->FieldOfView=75;PC->SetViewTarget(Camera);};
    auto Key=[&](FKey K,bool Down){PC->InputKey(FInputKeyEventArgs(nullptr,INPUTDEVICEID_NONE,K,Down?IE_Pressed:IE_Released,FPlatformTime::Cycles64()));};
    static TArray<double> Frames;static double Last=0;static int32 Previous=-1;
    if(FApp::CanEverRender()&&(SmokeStage==2||SmokeStage==4))
    {const double Now=FPlatformTime::Seconds();if(Previous==SmokeStage&&Last>0&&SmokeTime>1)Frames.Add((Now-Last)*1000);Last=Now;}Previous=SmokeStage;
    switch(SmokeStage)
    {
    case 0:
        if(SmokeTime<(FApp::CanEverRender()?45:2))break;
        SmokeCheck(P->GetCharacterMovement()->IsMovingOnGround(),TEXT("Marty remains grounded on the retained room floor"));Adventure->Start(true);
        {int32 Room=0,Outdoor=0;bool SofaAligned=false,VisualOnly=true;for(TActorIterator<AStaticMeshActor> It(GetWorld());It;++It){if(It->ActorHasTag(TEXT("WorldRealismRoom"))){++Room;auto* C=It->GetStaticMeshComponent();VisualOnly&=C->GetCollisionEnabled()==ECollisionEnabled::NoCollision;if(C->GetStaticMesh()&&C->GetStaticMesh()->GetName()==TEXT("RoomSofa"))SofaAligned=FMath::Abs(C->Bounds.Origin.Y+15)<2&&FMath::Abs(C->Bounds.Origin.X-340)<2;}if(It->ActorHasTag(TEXT("WorldRealismOutdoor")))++Outdoor;}
        SmokeCheck(Room==7,TEXT("Seven detailed room mesh batches are present"));SmokeCheck(VisualOnly,TEXT("Detailed room surfaces retain existing collision via visual-only overlays"));SmokeCheck(SofaAligned,TEXT("FBX handedness correction aligns sofa with tested gameplay coordinates"));SmokeCheck(Outdoor>=7,TEXT("Original leafy park trees and bench replace primitive silhouettes"));}
        {auto* Base=LoadObject<UTexture2D>(nullptr,TEXT("/Game/Adventure/Scenery/Textures/Oak_Base.Oak_Base"));auto* Normal=LoadObject<UTexture2D>(nullptr,TEXT("/Game/Adventure/Scenery/Textures/SofaLinen_Normal.SofaLinen_Normal"));auto* Rough=LoadObject<UTexture2D>(nullptr,TEXT("/Game/Adventure/Scenery/Textures/Brick_Rough.Brick_Rough"));SmokeCheck(Base&&Base->GetSizeX()==2048&&Base->SRGB,TEXT("Oak uses a 2K colour-managed base map"));SmokeCheck(Normal&&Normal->GetSizeX()==1024&&!Normal->SRGB,TEXT("Fabric normal map uses linear normal data"));SmokeCheck(Rough&&!Rough->SRGB,TEXT("Brick roughness map uses linear data"));}
        SmokePosition=P->GetActorLocation();PC->SetControlRotation(FRotator(-12,0,0));Key(EKeys::W,true);Next();break;
    case 1:
        if(SmokeTime<.7f)break;Key(EKeys::W,false);SmokeCheck(FVector::Dist2D(P->GetActorLocation(),SmokePosition)>35,TEXT("Keyboard movement traverses the detailed room"));
        if(PC->GetHUD())PC->GetHUD()->bShowHUD=false;View(FVector(-365,-380,150),FVector(230,140,115));Next();break;
    case 2:
        if(SmokeTime<5)break;Shot(TEXT("WorldRoom.png"));Next();break;
    case 3:
        if(SmokeTime<1)break;PC->SetViewTarget(P);if(PC->GetHUD())PC->GetHUD()->bShowHUD=true;P->ReturnHome();Next();break;
    case 4:
        if(SmokeTime<5)break;Shot(TEXT("WorldDogView.png"));Next();break;
    case 5:
        if(SmokeTime<1)break;Neighbourhood->GoOutside(P);if(PC->GetHUD())PC->GetHUD()->bShowHUD=false;SmokeCheck(Neighbourhood->Outside,TEXT("Headquarters door travel still reaches the neighbourhood"));
        View(Neighbourhood->Entrance+FVector(-1300,-1400,350),Neighbourhood->Entrance+FVector(-250,200,260));Next();break;
    case 6:
        if(SmokeTime<5)break;Shot(TEXT("WorldHome.png"));Next();break;
    case 7:
        if(SmokeTime<1)break;
        {int32 Grass=0;for(TActorIterator<AActor> It(GetWorld());It;++It)if(It->ActorHasTag(TEXT("WorldRealismOutdoor")))if(auto* C=It->FindComponentByClass<UInstancedStaticMeshComponent>())if(C->GetStaticMesh()&&C->GetStaticMesh()->GetName()==TEXT("GrassClump")){Grass+=C->GetInstanceCount();SmokeCheck(C->GetCollisionEnabled()==ECollisionEnabled::NoCollision&&!It->IsActorTickEnabled(),TEXT("Grass batch has no collision or actor tick"));}SmokeCheck(Grass==640,TEXT("Park grass is drawn in one 640-instance batch"));}
        View(Neighbourhood->Park+FVector(-1250,-1150,150),Neighbourhood->Park+FVector(200,450,210));Next();break;
    case 8:
        if(SmokeTime<8)break;Shot(TEXT("WorldPark.png"));Next();break;
    case 9:
        if(SmokeTime<1)break;PC->SetViewTarget(P);if(PC->GetHUD())PC->GetHUD()->bShowHUD=true;P->ReturnHome();SmokeCheck(!Neighbourhood->Outside,TEXT("Return-home recovery restores the detailed indoor room"));Key(EKeys::Gamepad_RightThumbstick,true);Next();break;
    case 10:
        Key(EKeys::Gamepad_RightThumbstick,false);if(SmokeTime<.2f)break;SmokeCheck(P->CameraPreset==1,TEXT("Synthetic controller camera switching remains available"));Next();break;
    case 11:
        if(SmokeTime<1)break;
        {auto J=MakeShared<FJsonObject>();TArray<TSharedPtr<FJsonValue>> Pass,Fail;for(auto S:SmokePassed)Pass.Add(MakeShared<FJsonValueString>(S));for(auto S:SmokeFailed)Fail.Add(MakeShared<FJsonValueString>(S));J->SetArrayField(TEXT("passed"),Pass);J->SetArrayField(TEXT("failed"),Fail);J->SetStringField(TEXT("input"),TEXT("Synthetic keyboard/gamepad; physical hardware unverified"));J->SetStringField(TEXT("frame_scope"),TEXT("Offscreen DX12 frame intervals on this machine; not GPU timings, a baseline comparison or a playable-frame-rate benchmark"));if(!Frames.IsEmpty()){Frames.Sort();J->SetNumberField(TEXT("frame_samples"),Frames.Num());J->SetNumberField(TEXT("median_frame_ms"),Frames[Frames.Num()/2]);J->SetNumberField(TEXT("p95_frame_ms"),Frames[FMath::FloorToInt((Frames.Num()-1)*.95)]);}FString Json;FJsonSerializer::Serialize(J,TJsonWriterFactory<>::Create(&Json));bool Saved=FFileHelper::SaveStringToFile(Json,*(Root/TEXT("world-smoke.json")));UGameplayStatics::DeleteGameInSlot(TEXT("CheweenieWorldAcceptance"),0);FPlatformMisc::RequestExitWithStatus(false,Saved&&SmokeFailed.IsEmpty()?0:1);Next();}break;
    }
}
