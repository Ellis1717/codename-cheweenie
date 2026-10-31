#include "MartyCharacter.h"
#include "CozyAdventure.h"
#include "CozyCoop.h"
#include "HalloweenCostume.h"
#include "MartyGait.h"
#include "DogLife.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PoseableMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
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
#include "UnrealClient.h"

static FString RatchetRoot(){FString Root=FPaths::ProjectSavedDir();FParse::Value(FCommandLine::Get(),TEXT("CheweenieTestOutput="),Root);return Root;}
static bool RatchetReport(const TCHAR* File,const TArray<FString>& Passed,const TArray<FString>& Failed)
{
    auto J=MakeShared<FJsonObject>();TArray<TSharedPtr<FJsonValue>> Pass,Fail;
    for(auto V:Passed)Pass.Add(MakeShared<FJsonValueString>(V));for(auto V:Failed)Fail.Add(MakeShared<FJsonValueString>(V));
    J->SetArrayField(TEXT("passed"),Pass);J->SetArrayField(TEXT("failed"),Fail);J->SetStringField(TEXT("scope"),TEXT("Actual map/travel with synthetic keyboard/gamepad and isolated completion/save fixtures. Static first-pass Ratchet model; physical controllers unverified."));
    FString Json;FJsonSerializer::Serialize(J,TJsonWriterFactory<>::Create(&Json));return FFileHelper::SaveStringToFile(Json,*(RatchetRoot()/File));
}
void AMartyPlayMode::TickRatchetSmoke(float Delta)
{
    SmokeTime+=Delta;auto* PC=GetWorld()->GetFirstPlayerController();auto* P=PC?Cast<AMartyCharacter>(PC->GetPawn()):nullptr;if(!P||!Adventure||!Adventure->Progress)return;
    auto* S=Adventure.Get();auto* D=S->Progress.Get();
    auto Key=[PC](FKey K,bool Down){PC->InputKey(FInputKeyEventArgs(nullptr,INPUTDEVICEID_NONE,K,Down?IE_Pressed:IE_Released,FPlatformTime::Cycles64()));};
    auto Next=[&](){++SmokeStage;SmokeTime=0;};
    if(FParse::Param(FCommandLine::Get(),TEXT("CheweenieRatchetResume")))
    {
        if(SmokeStage==0&&SmokeTime>3)
        {
            const bool Returned=FPaths::FileExists(RatchetRoot()/TEXT("ratchet-memory.json"));
            SmokeCheck(D->RatchetUnlocked,TEXT("Completed legacy checkpoint migrates to permanent Ratchet unlock"));
            SmokeCheck(S->Menu().IsValidIndex(6)&&S->Menu()[6].Contains(TEXT("welcome home")),TEXT("Title advertises the unlocked memory walk"));
            if(Returned){SmokeCheck(P->UsesAdventureHome(),TEXT("Controller View returns from Ratchet to the real solo patrol"));const bool Saved=RatchetReport(TEXT("ratchet-return.json"),SmokePassed,SmokeFailed);FPlatformMisc::RequestExitWithStatus(false,Saved&&SmokeFailed.IsEmpty()?0:1);Next();}
            else{RatchetReport(TEXT("ratchet-resume.json"),SmokePassed,SmokeFailed);S->Selection=6;Key(EKeys::Enter,true);Next();}
        }return;
    }
    switch(SmokeStage)
    {
    case 0:if(SmokeTime<3)break;SmokeCheck(!D->RatchetUnlocked,TEXT("Ratchet starts locked on a fresh checkpoint"));S->Selection=6;Key(EKeys::Gamepad_FaceButton_Bottom,true);Next();break;
    case 1:
        Key(EKeys::Gamepad_FaceButton_Bottom,false);if(SmokeTime<.4f)break;
        SmokeCheck(!D->RatchetUnlocked&&S->Screen==ECozyScreen::Title&&S->Message.Contains(TEXT("Finish")),TEXT("Locked controller selection explains the Cozy Patrol requirement without travelling"));
        S->Start(false);D->SnackMask=(1<<S->Mission->Snacks.Num())-1;D->VisitIndex=S->Mission->Visits.Num()-1;
        P->TeleportTo(S->Mission->Visits.Last().Position+FVector(0,0,28),FRotator::ZeroRotator);S->Interact();Next();break;
    case 2:
        if(SmokeTime<.5f)break;
        SmokeCheck(D->Complete&&D->RatchetUnlocked,TEXT("Real final patrol interaction permanently unlocks Ratchet"));
        SmokeCheck(S->Message.Contains(TEXT("Welcome home, Ratchet")),TEXT("Unlock presents the gentle welcome-home message"));
        {const auto* Saved=Cast<UCozyAdventureSave>(UGameplayStatics::LoadGameFromSlot(S->Slot,0));SmokeCheck(Saved&&Saved->RatchetUnlocked,TEXT("Earned unlock is written to the isolated checkpoint"));}
        S->Start(true);SmokeCheck(D->RatchetUnlocked&&!D->Complete,TEXT("Starting a new patrol preserves the earned character"));
        // An older completed save has no unlock field: exercise migration in another process.
        D->RatchetUnlocked=false;D->Complete=true;D->SnackMask=(1<<S->Mission->Snacks.Num())-1;D->VisitIndex=S->Mission->Visits.Num();S->Save();
        {const bool Saved=RatchetReport(TEXT("ratchet-unlock.json"),SmokePassed,SmokeFailed);FPlatformMisc::RequestExitWithStatus(false,Saved&&SmokeFailed.IsEmpty()?0:1);Next();}break;
    }
}
void ACozyCoopController::TickRatchetAcceptance(float Delta)
{
    StageTime+=Delta;auto* P=Cast<ACozyCoopPet>(GetPawn());if(!P)return;
    auto Check=[&](bool OK,const TCHAR* Label){(OK?Passed:Failed).Add(Label);UE_LOG(LogTemp,Display,TEXT("RATCHET_CHECK %s %s"),OK?TEXT("PASS"):TEXT("FAIL"),Label);};
    auto Key=[&](FKey K,bool Down){InputKey(FInputKeyEventArgs(nullptr,INPUTDEVICEID_NONE,K,Down?IE_Pressed:IE_Released,FPlatformTime::Cycles64()));};
    auto Axis=[&](FKey K,float V){InputKey(FInputKeyEventArgs(nullptr,INPUTDEVICEID_NONE,K,V,Delta,1,FPlatformTime::Cycles64()));};
    auto Next=[&](){++Stage;StageTime=0;};
    if(Stage>=3)Axis(EKeys::Gamepad_LeftY,0);
    switch(Stage)
    {
    case 0:
        if(StageTime<3)break;
        Check(GetNetMode()==NM_Standalone&&P->PetId==3,TEXT("Unlocked menu travels to the real map as playable Ratchet"));
        Check(GetWorld()->GetGameState<ACozyCoopState>()->RatchetUnlocked,TEXT("Memory room recognises the saved unlock"));
        {const auto* Mesh=Cast<UStaticMeshComponent>(P->GetDefaultSubobjectByName(TEXT("RetainedMarty")));Check(Mesh&&Mesh->GetStaticMesh()&&Mesh->GetStaticMesh()->GetName()==TEXT("Ratchet_Tribute"),TEXT("Ratchet uses his distinct original fuller-coat mesh"));if(Mesh)UE_LOG(LogTemp,Display,TEXT("RATCHET_VIS visible=%d actor=%s component=%s local_bounds=%s world_bounds=%s scale=%s"),Mesh->IsVisible(),*P->GetActorLocation().ToString(),*Mesh->GetComponentLocation().ToString(),*Mesh->GetStaticMesh()->GetBounds().Origin.ToString(),*Mesh->Bounds.Origin.ToString(),*Mesh->GetComponentScale().ToString());}
        Check(!P->Skin->IsVisible()&&!P->Gait->IsComponentTickEnabled()&&!P->Life->IsComponentTickEnabled(),TEXT("Ratchet does not inherit visible dog anatomy or dog needs/tricks"));
        OwnStart=P->GetActorLocation();SetControlRotation(FRotator(-12,0,0));Key(EKeys::W,true);Next();break;
    case 1:
        if(StageTime<.7f)break;Key(EKeys::W,false);Check(FVector::Dist2D(OwnStart,P->GetActorLocation())>25,TEXT("Keyboard walks Ratchet"));OwnStart=P->GetActorLocation();Next();break;
    case 2:
        Axis(EKeys::Gamepad_LeftY,.7f);if(StageTime<.7f)break;Axis(EKeys::Gamepad_LeftY,0);Check(FVector::Dist2D(OwnStart,P->GetActorLocation())>20,TEXT("Synthetic controller stick walks Ratchet"));Key(EKeys::Gamepad_FaceButton_Top,true);Next();break;
    case 3:
        Key(EKeys::Gamepad_FaceButton_Top,false);if(StageTime<.2f)break;Check(P->GetCharacterMovement()->IsFalling(),TEXT("Controller jump works for Ratchet"));Key(EKeys::Gamepad_DPad_Up,true);Next();break;
    case 4:
        Key(EKeys::Gamepad_DPad_Up,false);if(StageTime<1)break;Check(P->Costume->VisiblePieces()==0,TEXT("Ratchet keeps his own coat without an unrelated pet costume"));Key(EKeys::One,true);Next();break;
    case 5:
        Key(EKeys::One,false);if(StageTime<.5f)break;Check(P->PetId==0,TEXT("Memory walk can switch back to Marty"));Key(EKeys::Gamepad_DPad_Left,true);Next();break;
    case 6:
        Key(EKeys::Gamepad_DPad_Left,false);if(StageTime<.5f)break;Check(P->PetId==3,TEXT("Controller pet cycling reaches unlocked Ratchet"));P->ReturnHome();Next();break;
    case 7:
        if(StageTime<1)break;
        OwnStart=P->GetActorLocation();
        {const FVector Target=P->GetActorLocation()+FVector(0,0,-4);auto* C=GetWorld()->SpawnActor<ACameraActor>(Target+FVector(105,-105,48),(FVector(-105,105,-48)).Rotation());C->GetCameraComponent()->FieldOfView=42;SetViewTarget(C);if(GetHUD())GetHUD()->bShowHUD=false;}Next();break;
    case 8:
        if(StageTime<5)break;Check(FVector::Dist2D(OwnStart,P->GetActorLocation())<2,TEXT("Synthetic stick release keeps Ratchet still for the portrait"));if(FApp::CanEverRender()){const auto* Mesh=Cast<UStaticMeshComponent>(P->GetDefaultSubobjectByName(TEXT("RetainedMarty")));Check(Mesh&&Mesh->WasRecentlyRendered(.5f),TEXT("Ratchet mesh is actually rendered in the portrait camera"));}if(FApp::CanEverRender())FScreenshotRequest::RequestScreenshot(RatchetRoot()/TEXT("RatchetMemory.png"),true,false);Next();break;
    case 9:
        if(StageTime<1)break;
        if(!RatchetReport(TEXT("ratchet-memory.json"),Passed,Failed)||!Failed.IsEmpty()){FPlatformMisc::RequestExitWithStatus(false,1);Next();break;}
        Key(EKeys::Gamepad_Special_Left,true);Next();break;
    }
}
