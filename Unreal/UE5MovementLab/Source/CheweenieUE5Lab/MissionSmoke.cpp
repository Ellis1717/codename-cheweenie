#include "MartyCharacter.h"
#include "CozyAdventure.h"
#include "OpenMissions.h"
#include "CozyNavigation.h"
#include "Neighbourhood.h"
#include "DogLife.h"
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
void AMartyPlayMode::TickMissionSmoke(float Delta)
{
    SmokeTime+=Delta;auto* PC=GetWorld()->GetFirstPlayerController();auto* P=PC?Cast<AMartyCharacter>(PC->GetPawn()):nullptr;
    if(!P||!Adventure||!Adventure->Progress||!Neighbourhood||!Navigation)return;auto* S=Adventure.Get();auto* D=S->Progress.Get();
    FString Root=FPaths::ProjectSavedDir();FParse::Value(FCommandLine::Get(),TEXT("CheweenieTestOutput="),Root);
    auto Key=[&](FKey K,bool Down){PC->InputKey(FInputKeyEventArgs(nullptr,INPUTDEVICEID_NONE,K,Down?IE_Pressed:IE_Released,FPlatformTime::Cycles64()));};
    auto Next=[&](){++SmokeStage;SmokeTime=0;};auto Place=[&](FVector At){P->GetCharacterMovement()->StopMovementImmediately();P->TeleportTo(At+FVector(0,0,45),FRotator::ZeroRotator);};
    auto Shot=[&](const TCHAR* Name){if(FApp::CanEverRender())FScreenshotRequest::RequestScreenshot(Root/Name,true,false);};
    auto Finish=[&](bool Resume){auto O=MakeShared<FJsonObject>();TArray<TSharedPtr<FJsonValue>> Pass,Fail;for(const auto& V:SmokePassed)Pass.Add(MakeShared<FJsonValueString>(V));for(const auto& V:SmokeFailed)Fail.Add(MakeShared<FJsonValueString>(V));O->SetStringField(TEXT("scope"),TEXT("Real solo map; synthetic keyboard/controller journal input, real location polling and backpack stow, explicit legacy/action-state fixtures. Sports event integration is checked by the separate sports suite. Physical devices unverified."));O->SetArrayField(TEXT("passed"),Pass);O->SetArrayField(TEXT("failed"),Fail);FString Json;FJsonSerializer::Serialize(O,TJsonWriterFactory<>::Create(&Json));bool Saved=FFileHelper::SaveStringToFile(Json,*(Root/(Resume?TEXT("missions-resume.json"):TEXT("missions-play.json"))));FPlatformMisc::RequestExitWithStatus(false,Saved&&SmokeFailed.IsEmpty()?0:1);SmokeStage=1000;};
    if(FParse::Param(FCommandLine::Get(),TEXT("CheweenieMissionResume")))
    {
        if(SmokeStage==0&&SmokeTime>3)
        {
            SmokeCheck(S->FinishedMissionCount()==7,TEXT("Seven journal completion pawprints survive a second process"));
            SmokeCheck(S->MissionValue(TEXT("dogs"))==4&&S->MissionValue(TEXT("patches"))==3,TEXT("Partial and bounded progress reload by stable objective key"));
            SmokeCheck(D->TrackedOpenMission==TEXT("sports")&&!D->ShowMissionTracker,TEXT("Pinned mission and hidden tracker preference reload"));
            SmokeCheck(D->PraiseStars==3,TEXT("Cosmetic journal rewards do not alter the home upgrade economy"));
            SmokeCheck(!D->HalloweenActive,TEXT("Mission saves do not enable Halloween on restart"));
            S->Start(false);SmokeCheck(S->AllowsMovement()&&S->FinishedMissionCount()==7,TEXT("Continue preserves journal history and permits free play"));
            S->RecordMission(TEXT("dogs"),4);SmokeCheck(S->FinishedMissionCount()==7,TEXT("Repeated completed objectives cannot grant duplicate pawprints"));Finish(true);
        }return;
    }
    static int32 ReleasedStage=-1;
    if(ReleasedStage!=SmokeStage){for(FKey K:{EKeys::J,EKeys::Enter,EKeys::BackSpace,EKeys::Gamepad_FaceButton_Bottom,EKeys::Gamepad_FaceButton_Right})Key(K,false);ReleasedStage=SmokeStage;return;}
    switch(SmokeStage)
    {
    case 0:if(SmokeTime>(FApp::CanEverRender()?25:3))
        {
            SmokeCheck(CozyOpenMissions().Num()==8&&S->FinishedMissionCount()==0,TEXT("Eight optional missions are available without an acceptance chain"));
            SmokeCheck(D->TrackedOpenMission.IsNone()&&S->MissionValue(TEXT("park"))==0,TEXT("Fresh profile starts in unpinned free exploration"));
            S->Start(true);Key(EKeys::J,true);Next();
        }break;
    case 1:if(SmokeTime>.3){Key(EKeys::J,false);SmokeCheck(S->Screen==ECozyScreen::Missions&&!S->AllowsMovement(),TEXT("Keyboard J opens the journal and stops movement safely"));SmokeCheck(S->Menu().Num()==11,TEXT("Journal includes eight missions, unpin, visibility and back"));S->Selection=0;Key(EKeys::Enter,true);Next();}break;
    case 2:if(SmokeTime>.3){Key(EKeys::Enter,false);SmokeCheck(D->TrackedOpenMission==TEXT("explorer"),TEXT("Keyboard confirm pins a chosen mission"));SmokeCheck(FVector::Dist2D(Navigation->Landmarks()[0].Position,Neighbourhood->Park)<1&&Navigation->Goal().Label.StartsWith(TEXT("Door to")),TEXT("Pinned exploration keeps the park destination and guides an indoor player to the exit"));Shot(TEXT("MissionJournal.png"));Next();}break;
    case 3:if(SmokeTime>1){S->Back();SmokeCheck(S->Screen==ECozyScreen::Playing,TEXT("Back returns from the journal to exploration"));Neighbourhood->GoOutside(P);Place(Neighbourhood->Park);Next();}break;
    case 4:if(SmokeTime>1){SmokeCheck(S->MissionPercent(TEXT("explorer"))==50,TEXT("Actual park visit records partial progress"));SmokeCheck(FVector::Dist2D(Navigation->Goal().Position,Neighbourhood->School)<1,TEXT("Mission waypoint advances to the next unfinished location"));S->OpenMissions();Place(Neighbourhood->School);Next();}break;
    case 5:if(SmokeTime>.8){Key(EKeys::J,false);SmokeCheck(S->MissionPercent(TEXT("explorer"))==50,TEXT("Paused journal does not earn location progress"));Key(EKeys::Gamepad_FaceButton_Right,true);Next();}break;
    case 6:if(SmokeTime>1){Key(EKeys::Gamepad_FaceButton_Right,false);SmokeCheck(S->MissionPercent(TEXT("explorer"))==100&&S->FinishedMissionCount()==1,TEXT("School visit completes the exploration mission in free play"));SmokeCheck(S->Screen==ECozyScreen::Playing&&S->AllowsMovement(),TEXT("Mission completion does not end exploration or open a blocking modal"));SmokeCheck(S->MissionNotice.Contains(TEXT("Mission complete")),TEXT("Completion produces a visible non-blocking notice"));S->RecordMission(TEXT("school"),999);SmokeCheck(S->FinishedMissionCount()==1,TEXT("Repeat visits award one permanent pawprint only"));S->OpenMissions();S->Selection=8;Key(EKeys::Gamepad_FaceButton_Bottom,true);Next();}break;
    case 7:if(SmokeTime>.3){Key(EKeys::Gamepad_FaceButton_Bottom,false);SmokeCheck(D->TrackedOpenMission.IsNone(),TEXT("Controller confirm unpins the mission without losing completion"));S->Selection=9;Key(EKeys::Enter,true);Next();}break;
    case 8:if(SmokeTime>.3){Key(EKeys::Enter,false);SmokeCheck(!D->ShowMissionTracker,TEXT("Tracker can be hidden independently of mission progress"));S->Back();S->Pause();S->Selection=10;Key(EKeys::Gamepad_FaceButton_Bottom,true);Next();}break;
    case 9:if(SmokeTime>.3){Key(EKeys::Gamepad_FaceButton_Bottom,false);SmokeCheck(S->Screen==ECozyScreen::Missions,TEXT("Pause menu makes the journal accessible to gamepad players"));S->Back();SmokeCheck(S->Screen==ECozyScreen::Pause,TEXT("Journal remembers its pause-menu return destination"));S->Back();S->Start(false);Neighbourhood->GoInside(P);D->NeighbourDogMask=7;D->LakituBond=1;D->RioBond=2;D->HoleDepths={1,1,0};D->CarryItem=2;P->Life->Stow();D->MealBonusRemaining=100;D->RestBonusRemaining=100;Next();}break;
    case 10:if(SmokeTime>1)
        {
            SmokeCheck(S->MissionPercent(TEXT("neighbours"))==75,TEXT("Existing distinct dog greetings receive partial journal credit"));
            SmokeCheck(S->MissionPercent(TEXT("cats"))==100,TEXT("Existing cat bonds receive permanent completion credit"));
            SmokeCheck(S->MissionPercent(TEXT("digging"))==66,TEXT("Different dug patches count once each"));
            SmokeCheck(D->BagItems.Contains(2)&&S->MissionPercent(TEXT("rainbow"))==100,TEXT("Real backpack stow records Rainbow Guy even between polling ticks"));
            SmokeCheck(S->MissionPercent(TEXT("cozy"))==100,TEXT("Earned meal and rest state contributes to cozy care mission"));
            SmokeCheck(S->MissionPercent(TEXT("sports"))==0,TEXT("Unplayed sports receive no invented score credit"));
            const int32 Before=D->OpenMissionProgress.Num();S->RecordMission(TEXT("unknown-mission-key"),99);S->RecordMission(TEXT("dogs"),-1);SmokeCheck(D->OpenMissionProgress.Num()==Before&&S->MissionValue(TEXT("dogs"))==3,TEXT("Unknown and negative updates cannot corrupt progress"));
            S->RecordMission(TEXT("dogs"),999);S->RecordMission(TEXT("patches"),999);SmokeCheck(S->MissionValue(TEXT("dogs"))==4&&S->MissionValue(TEXT("patches"))==3,TEXT("Objective updates clamp to authored completion targets"));
            D->SnackMask=7;D->VisitIndex=3;D->Complete=true;Next();
        }break;
    case 11:if(SmokeTime>1)
        {
            SmokeCheck(S->MissionPercent(TEXT("patrol"))==100&&S->FinishedMissionCount()==7,TEXT("Legacy completed patrol migrates into the journal without erasing other history"));
            S->Start(true);SmokeCheck(D->SnackMask==0&&!D->Complete&&S->FinishedMissionCount()==7,TEXT("New patrol resets only the legacy patrol, preserving open mission completion"));
            S->TrackMission(TEXT("sports"));S->TrackMission(TEXT("missing"));SmokeCheck(D->TrackedOpenMission==TEXT("sports"),TEXT("Unknown mission pin cannot replace the valid saved target"));
            SmokeCheck(D->PraiseStars==3,TEXT("Journal completion leaves praise-star upgrades unchanged"));
            S->OpenMissions();S->Selection=0;Shot(TEXT("MissionCompleted.png"));Next();
        }break;
    case 12:if(SmokeTime>1){S->Back();D->ShowMissionTracker=true;Shot(TEXT("MissionTracker.png"));Next();}break;
    case 13:if(SmokeTime>1){D->ShowMissionTracker=false;S->Save();Finish(false);}break;
    }
}
