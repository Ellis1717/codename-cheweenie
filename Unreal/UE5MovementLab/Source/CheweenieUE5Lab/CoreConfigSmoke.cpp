#include "MartyCharacter.h"
#include "CozyAdventure.h"
#include "CozyModeHub.h"
#include "ParkSports.h"
#include "Neighbourhood.h"
#include "HockeySurprise.h"
#include "ControllerComfort.h"
#include "DogJackets.h"
#include "HalloweenCostume.h"
#include "GameFramework/PlayerController.h"
#include "Engine/GameInstance.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"
#include "InputKeyEventArgs.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "Misc/App.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "UnrealClient.h"

void AMartyPlayMode::TickCoreConfigSmoke(float Delta)
{
    SmokeTime+=Delta;auto* PC=GetWorld()->GetFirstPlayerController();auto* P=PC?Cast<AMartyCharacter>(PC->GetPawn()):nullptr;
    if(!P||!Adventure||!Adventure->Progress||!Neighbourhood||!Neighbourhood->Sports||!Hockey)return;
    auto* S=Adventure.Get();auto* D=S->Progress.Get();auto* Pref=GetGameInstance()->GetSubsystem<UControllerComfort>();
    const bool Resume=FParse::Param(FCommandLine::Get(),TEXT("CheweenieCoreConfigResume"));
    FString Root=FPaths::ProjectSavedDir();FParse::Value(FCommandLine::Get(),TEXT("CheweenieTestOutput="),Root);
    auto Next=[&](){++SmokeStage;SmokeTime=0;};
    auto Tap=[&](FKey K){PC->InputKey(FInputKeyEventArgs(nullptr,INPUTDEVICEID_NONE,K,IE_Pressed,FPlatformTime::Cycles64()));PC->InputKey(FInputKeyEventArgs(nullptr,INPUTDEVICEID_NONE,K,IE_Released,FPlatformTime::Cycles64()));};
    auto Finish=[&](){
        auto O=MakeShared<FJsonObject>();TArray<TSharedPtr<FJsonValue>> Pass,Fail;
        for(auto& X:SmokePassed)Pass.Add(MakeShared<FJsonValueString>(X));for(auto& X:SmokeFailed)Fail.Add(MakeShared<FJsonValueString>(X));
        O->SetStringField(TEXT("scope"),TEXT("Real combined game map; synthetic keyboard/gamepad menu events and isolated save/preferences; physical hardware unverified"));
        O->SetArrayField(TEXT("passed"),Pass);O->SetArrayField(TEXT("failed"),Fail);FString Json;FJsonSerializer::Serialize(O,TJsonWriterFactory<>::Create(&Json));
        const bool Saved=FFileHelper::SaveStringToFile(Json,*(Root/(Resume?TEXT("core-config-resume.json"):TEXT("core-config.json"))));SmokeStage=999;FPlatformMisc::RequestExitWithStatus(false,Saved&&SmokeFailed.IsEmpty()?0:1);
    };
    if(Resume){if(SmokeTime>3){
        SmokeCheck(!S->Feature(ECozyFeature::ParkSports)&&!S->Feature(ECozyFeature::HockeyStory)&&!S->Feature(ECozyFeature::MenuArtwork),TEXT("New optional feature settings survive another process"));
        SmokeCheck(D->FantasyCostumeMask==7&&D->MartyJacket==1&&D->CostumeMask==6,TEXT("Independent outfit styles cat clothing and Marty jacket reload together"));
        SmokeCheck(D->SnackMask==(1<<S->Mission->Snacks.Num())-1&&D->VisitIndex==2,TEXT("Existing patrol progress survives the combined save schema"));
        SmokeCheck(FMath::IsNearlyEqual(Pref->CameraDistances[5],205.f)&&S->Feature(ECozyFeature::Cameras),TEXT("Sixth camera zoom reloads without disabling existing camera controls"));Finish();}return;}
    switch(SmokeStage){
    case 0:if(SmokeTime>(FApp::CanEverRender()?25:3)){
        SmokeCheck(S->Slot==TEXT("CheweenieCoreConfigAcceptance"),TEXT("Combined settings acceptance uses an isolated save"));
        S->Start(true);D->SnackMask=(1<<S->Mission->Snacks.Num())-1;D->VisitIndex=2;
        SmokeCheck(S->Feature(ECozyFeature::ParkSports)&&S->Feature(ECozyFeature::HockeyStory)&&S->Feature(ECozyFeature::MenuArtwork),TEXT("Older default feature bits enable newly integrated options"));
        S->Pause();S->Selection=2;S->Confirm();S->Selection=2;S->Confirm();
        const auto G=S->FeatureGroup();SmokeCheck(G.Contains(ECozyFeature::ParkSports)&&G.Contains(ECozyFeature::HockeyStory),TEXT("Pet settings expose both optional activities through existing controls"));
        S->Selection=G.IndexOfByKey(ECozyFeature::ParkSports);Tap(EKeys::Gamepad_FaceButton_Bottom);Next();}break;
    case 1:if(SmokeTime>.5){
        S->Back();S->Back();S->Back();Neighbourhood->GoOutside(P);
        const FVector Ball=Neighbourhood->Sports->SoccerBall->GetActorLocation();P->TeleportTo(Ball+FVector(-70,0,20),FRotator::ZeroRotator);
        SmokeCheck(Neighbourhood->Sports->Prompt(P).IsEmpty()&&!Neighbourhood->Sports->Interact(P),TEXT("Disabled sports reject interaction while the park remains walkable"));
        S->ToggleFeature(ECozyFeature::ParkSports);SmokeCheck(Neighbourhood->Sports->Prompt(P).Contains(TEXT("kick")),TEXT("Re-enabling sports restores nearby ball interaction without rebuilding the park"));S->ToggleFeature(ECozyFeature::ParkSports);
        S->ToggleFeature(ECozyFeature::HockeyStory);Hockey->Start();SmokeCheck(!Hockey->Active(),TEXT("Disabled hockey story cannot replace the current adventure"));
        S->ToggleFeature(ECozyFeature::HockeyStory);Hockey->Start();SmokeCheck(Hockey->Active(),TEXT("Enabled hockey story starts inside the cumulative game"));
        S->ToggleFeature(ECozyFeature::HockeyStory);SmokeCheck(!Hockey->Active()&&!Neighbourhood->Outside,TEXT("Disabling an active optional story safely returns Marty home"));
        S->Pause();S->Selection=2;S->Confirm();S->Selection=5;S->Confirm();Next();}break;
    case 2:if(SmokeTime>.5){
        SmokeCheck(S->Screen==ECozyScreen::Wardrobe&&S->Menu().Num()==8,TEXT("One wardrobe contains all pet outfit styles and existing jackets"));
        for(int I=0;I<3;++I){S->Selection=I;S->Confirm();S->Selection=I+3;S->Confirm();}
        Next();}break;
    case 3:if(SmokeTime>.5){
        SmokeCheck(D->FantasyCostumeMask==7&&D->CostumeMask==7&&P->Costume->VisiblePieces()>0,TEXT("Configured original fantasy overlays coexist with independent clothing toggles"));
        S->Selection=6;S->Adjust(1);Next();}break;
    case 4:if(SmokeTime>.5){
        SmokeCheck(D->MartyJacket==1&&D->CostumeMask==6&&P->Jackets->VisiblePieces()>0&&P->Costume->VisiblePieces()==0,TEXT("Jacket selector retains cat choices and avoids stacking Marty garments"));
        S->Back();S->Selection=10;S->Confirm();SmokeCheck(S->Screen==ECozyScreen::ControllerSetup,TEXT("Combined controls retain the released controller comfort setup"));
        S->Back();S->Selection=3;S->Confirm();S->Selection=S->FeatureGroup().IndexOfByKey(ECozyFeature::MenuArtwork);Tap(EKeys::Enter);
        Next();}break;
    case 5:if(SmokeTime>1){SmokeCheck(!S->Feature(ECozyFeature::MenuArtwork),TEXT("Menu artwork is configurable through graphics and sound switches"));if(FApp::CanEverRender())FScreenshotRequest::RequestScreenshot(Root/TEXT("CoreConfiguration.png"),true,false);Next();}break;
    case 6:if(SmokeTime>1){
        S->Back();S->Back();S->Back();S->Start(false);P->CameraPreset=5;P->ResetCameraZoom();P->ZoomCamera(-35);S->Save();
        const auto* Reload=Cast<UCozyAdventureSave>(UGameplayStatics::LoadGameFromSlot(S->Slot,0));
        SmokeCheck(Reload&&Reload->FantasyCostumeMask==7&&Reload->MartyJacket==1&&Reload->SnackMask==(1<<S->Mission->Snacks.Num())-1&&Reload->VisitIndex==2,TEXT("Combined configuration saves additively without replacing existing progress"));
        SmokeCheck(FMath::IsNearlyEqual(Pref->CameraDistances[5],205.f),TEXT("Additional camera presets use the existing saved zoom configuration"));Finish();}break;
    }
}
