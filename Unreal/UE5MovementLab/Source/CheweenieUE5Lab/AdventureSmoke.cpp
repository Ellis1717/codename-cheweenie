// Native functional acceptance: drives the real pawn and action mappings through a complete patrol.
#include "MartyCharacter.h"
#include "CozyAdventure.h"
#include "GameFramework/PlayerController.h"
#include "InputKeyEventArgs.h"
#include "InputCoreTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/App.h"
#include "Misc/CommandLine.h"
#include "Misc/EngineVersion.h"
#include "Misc/FileHelper.h"
#include "Misc/Parse.h"
#include "Misc/Paths.h"
#include "UnrealClient.h"
#include "HAL/FileManager.h"

static FString AdventureOutput()
{
    FString Root=FPaths::ProjectSavedDir();
    FParse::Value(FCommandLine::Get(),TEXT("CheweenieTestOutput="),Root);
    IFileManager::Get().MakeDirectory(*Root,true);
    return Root;
}

void AMartyPlayMode::TickAdventureSmoke(float Delta)
{
    SmokeTime+=Delta;
    auto* PC=GetWorld()->GetFirstPlayerController();
    auto* Pawn=PC ? Cast<AMartyCharacter>(PC->GetPawn()) : nullptr;
    auto* Session=Adventure.Get();
    if(!Pawn || !Session || !Session->Progress)
    {
        if(SmokeTime>15) { UE_LOG(LogTemp,Error,TEXT("Adventure acceptance could not find the session/pawn")); FPlatformMisc::RequestExitWithStatus(false,1); }
        return;
    }
    auto Key=[PC](FKey K,bool Down){PC->InputKey(FInputKeyEventArgs(nullptr,INPUTDEVICEID_NONE,K,Down?IE_Pressed:IE_Released,FPlatformTime::Cycles64()));};
    auto Next=[this](){++SmokeStage;SmokeTime=0;};
    auto Shot=[](const TCHAR* Name){if(FApp::CanEverRender()) FScreenshotRequest::RequestScreenshot(AdventureOutput()/Name,true,false);};
    auto Finish=[this](bool Resume)
    {
        auto Report=MakeShared<FJsonObject>();
        auto Strings=[](const TArray<FString>& Values){TArray<TSharedPtr<FJsonValue>> Result;for(const auto& Value:Values)Result.Add(MakeShared<FJsonValueString>(Value));return Result;};
        Report->SetStringField(TEXT("engine"),FEngineVersion::Current().ToString());
        Report->SetStringField(TEXT("scope"),TEXT("Real mission traversal and native synthetic menu/interaction input; isolated acceptance save slot; physical hardware unverified"));
        Report->SetArrayField(TEXT("passed"),Strings(SmokePassed));Report->SetArrayField(TEXT("failed"),Strings(SmokeFailed));
        FString Json;FJsonSerializer::Serialize(Report,TJsonWriterFactory<>::Create(&Json));
        const bool Saved=FFileHelper::SaveStringToFile(Json,*(AdventureOutput()/(Resume?TEXT("adventure-resume.json"):TEXT("adventure-smoke.json"))));
        if(Resume) UGameplayStatics::DeleteGameInSlot(TEXT("CheweenieAdventureAcceptance"),0);
        FPlatformMisc::RequestExitWithStatus(false,Saved&&SmokeFailed.IsEmpty()?0:1);SmokeStage=1000;
    };
    if(FParse::Param(FCommandLine::Get(),TEXT("CheweenieResumeCheck")))
    {
        if(SmokeStage==0 && SmokeTime>2)
        {
            SmokeCheck(Session->Progress->Complete && Session->Progress->VisitIndex==Session->Mission->Visits.Num(),TEXT("Completed mission reloads in another process"));
            SmokeCheck(FMath::IsNearlyEqual(Session->Progress->Music,.35f),TEXT("Sound preference reloads in another process"));
            SmokeCheck(Session->Screen==ECozyScreen::Title,TEXT("Reload starts at title without auto-running gameplay"));
            Key(EKeys::Enter,true);Next();
        }
        else if(SmokeStage==1 && SmokeTime>.2)
        {
            Key(EKeys::Enter,false);SmokeCheck(Session->Screen==ECozyScreen::Playing&&Session->Progress->Complete,TEXT("Continue restores completed free roam"));
            Session->Pause();Session->Selection=4;Session->Confirm();Session->Selection=1;Session->Confirm();Next();
        }
        else if(SmokeStage==2 && SmokeTime>.2) {Key(EKeys::Gamepad_DPad_Down,true);Next();}
        else if(SmokeStage==3 && SmokeTime>.15)
        {
            Key(EKeys::Gamepad_DPad_Down,false);SmokeCheck(Session->Screen==ECozyScreen::Restart&&Session->Selection==1,TEXT("Fresh patrol requires choosing the explicit restart option"));
            Key(EKeys::Enter,true);Next();
        }
        else if(SmokeStage==4 && SmokeTime>.2)
        {
            Key(EKeys::Enter,false);
            SmokeCheck(Session->Screen==ECozyScreen::Playing&&!Session->Progress->Complete&&Session->Progress->SnackMask==0&&Session->Progress->VisitIndex==0&&FMath::IsNearlyEqual(Session->Progress->Music,.35f),TEXT("Confirmed restart clears mission progress and retains preferences"));
            Finish(true);
        }
        return;
    }
    auto Drive=[&](const FVector& Target,float Radius)
    {
        if(FVector::Dist2D(Pawn->GetActorLocation(),Target)<Radius)
        {if(SmokeMoving)Key(EKeys::W,false);SmokeMoving=false;return true;}
        PC->SetControlRotation(FRotator(-12,(Target-Pawn->GetActorLocation()).Rotation().Yaw,0));
        if(!SmokeMoving){Key(EKeys::W,true);SmokeMoving=true;}
        if(SmokeTime>25){Key(EKeys::W,false);SmokeMoving=false;SmokeCheck(false,TEXT("Mission waypoint traversal timed out"));Finish(false);}
        return false;
    };
    switch(SmokeStage)
    {
    case 0:
        if(SmokeTime<(FApp::CanEverRender()?60:2))break;
        SmokeCheck(Session->Screen==ECozyScreen::Title,TEXT("Branded title is the initial screen"));
        SmokeCheck(Session->LoadedSoundCount()==6,TEXT("Six original music/effect assets load"));
        Shot(TEXT("AdventureTitle.png"));SmokePosition=Pawn->GetActorLocation();Key(EKeys::W,true);Next();break;
    case 1:
        if(SmokeTime<.7)break;Key(EKeys::W,false);
        SmokeCheck(FVector::Dist2D(SmokePosition,Pawn->GetActorLocation())<1,TEXT("Title menu blocks gameplay movement"));
        Key(EKeys::Enter,true);Next();break;
    case 2:
        if(SmokeTime<.15)break;Key(EKeys::Enter,false);
        SmokeCheck(Session->Screen==ECozyScreen::Playing,TEXT("Enter starts the patrol"));
        Key(EKeys::Gamepad_Special_Right,true);Next();break;
    case 3:
        if(SmokeTime<.15)break;Key(EKeys::Gamepad_Special_Right,false);
        SmokeCheck(Session->Screen==ECozyScreen::Pause,TEXT("Menu/Options opens pause"));
        SmokePosition=Pawn->GetActorLocation();Key(EKeys::W,true);Next();break;
    case 4:
        if(SmokeTime<.6)break;Key(EKeys::W,false);
        SmokeCheck(FVector::Dist2D(SmokePosition,Pawn->GetActorLocation())<1,TEXT("Pause freezes player movement"));
        Key(EKeys::Gamepad_DPad_Down,true);Next();break;
    case 5:
        if(SmokeTime<.15)break;Key(EKeys::Gamepad_DPad_Down,false);
        SmokeCheck(Session->Selection==1,TEXT("D-pad navigates menu options"));
        // Setup the sound-settings row; confirm and adjustment still use real bound inputs.
        Session->Selection=3;Key(EKeys::Gamepad_FaceButton_Bottom,true);Next();break;
    case 6:
        if(SmokeTime<.15)break;Key(EKeys::Gamepad_FaceButton_Bottom,false);
        if(auto* HUD=PC->GetHUD<AMartyHUD>()) HUD->NotifyHitBoxClick(FName(TEXT("CozyRow_0")));
        SmokeCheck(Session->Screen==ECozyScreen::Settings,TEXT("A/Cross opens settings and selecting a volume row keeps them open"));
        Shot(TEXT("AdventureSettings.png"));
        Key(EKeys::Gamepad_DPad_Right,true);Next();break;
    case 7:
        if(SmokeTime<.15)break;Key(EKeys::Gamepad_DPad_Right,false);
        SmokeCheck(FMath::IsNearlyEqual(Session->Progress->Music,.35f),TEXT("D-pad adjusts and saves music preference"));
        Key(EKeys::Gamepad_FaceButton_Right,true);Next();break;
    case 8:
        if(SmokeTime<.15)break;Key(EKeys::Gamepad_FaceButton_Right,false);
        SmokeCheck(Session->Screen==ECozyScreen::Pause,TEXT("B/Circle returns to the parent menu"));
        Key(EKeys::Escape,true);Next();break;
    case 9:
        if(SmokeTime<.15)break;Key(EKeys::Escape,false);
        SmokeCheck(Session->Screen==ECozyScreen::Playing,TEXT("Escape resumes the patrol"));Next();break;
    case 10:
        if(!Drive(Session->Mission->Snacks[0].Position,30))break;
        SmokeCheck(Session->Progress->SnackMask==1,TEXT("Walking to first snack collects it"));Shot(TEXT("AdventurePlay.png"));Next();break;
    case 11:
        if(SmokeTime<.4)break;
        SmokeCheck(Session->Progress->SnackMask==1,TEXT("Standing on a collected snack grants no duplicate"));
        Next();break;
    case 12:
        if(!Drive(Session->Mission->Snacks[1].Position,30))break;
        SmokeCheck(Session->Progress->SnackMask==3,TEXT("Second snack advances patrol"));Next();break;
    case 13:
        if(!Drive(Session->Mission->Snacks[2].Position,30))break;
        SmokeCheck(Session->Progress->SnackMask==7,TEXT("All snacks unlock the companion visits"));
        Key(EKeys::E,true);Next();break;
    case 14:
        if(SmokeTime<.15)break;Key(EKeys::E,false);
        SmokeCheck(Session->Progress->VisitIndex==0,TEXT("Interaction outside the destination cannot advance progress"));Next();break;
    case 15:
        if(!Drive(Session->Mission->Visits[0].Position,70))break;
        Key(EKeys::E,true);Next();break;
    case 16:
        if(SmokeTime<.15)break;Key(EKeys::E,false);
        SmokeCheck(Session->Progress->VisitIndex==1,TEXT("Keyboard interaction completes Lakitu visit"));Next();break;
    case 17:
        if(!Drive(Session->Mission->Visits[1].Position,70))break;
        Key(EKeys::Gamepad_FaceButton_Left,true);Next();break;
    case 18:
        if(SmokeTime<.15)break;Key(EKeys::Gamepad_FaceButton_Left,false);
        SmokeCheck(Session->Progress->VisitIndex==2&&!Session->Progress->Complete,TEXT("X/Square completes Rio visit but still requires the fort"));Next();break;
    case 19:
        if(!Drive(Session->Mission->Visits[2].Position,70))break;
        Key(EKeys::E,true);Next();break;
    case 20:
        if(SmokeTime<.2)break;Key(EKeys::E,false);
        SmokeCheck(Session->Progress->Complete&&Session->Screen==ECozyScreen::Complete,TEXT("Cheeween Cozy completes the mission"));
        SmokeCheck(Session->Message.Contains(TEXT("Leo Ellis")),TEXT("Blanket Boss family cameo appears"));
        Shot(TEXT("AdventureComplete.png"));SmokePosition=Pawn->GetActorLocation();Key(EKeys::W,true);Next();break;
    case 21:
        if(SmokeTime<.7)break;Key(EKeys::W,false);
        SmokeCheck(FVector::Dist2D(SmokePosition,Pawn->GetActorLocation())<1,TEXT("Completion menu blocks movement"));
        Key(EKeys::Gamepad_FaceButton_Bottom,true);Next();break;
    case 22:
        if(SmokeTime<.15)break;Key(EKeys::Gamepad_FaceButton_Bottom,false);
        SmokeCheck(Session->Screen==ECozyScreen::Playing&&Session->Progress->Complete,TEXT("Victory can continue into free roam"));
        Session->Pause();Session->Selection=4;Session->Confirm();Session->Selection=1;
        Next();break;
    case 23:
        if(SmokeTime<.2)break;Key(EKeys::Enter,true);Next();break;
    case 24:
        if(SmokeTime<.15)break;Key(EKeys::Enter,false);
        SmokeCheck(Session->Screen==ECozyScreen::Restart&&Session->Selection==0,TEXT("New patrol requires confirmation and defaults to Keep"));
        Shot(TEXT("AdventureRestart.png"));
        Next();break;
    case 25:
        if(SmokeTime<.2)break;Key(EKeys::Enter,true);Next();break;
    case 26:
        if(SmokeTime<.15)break;Key(EKeys::Enter,false);
        SmokeCheck(Session->Progress->Complete&&Session->Screen==ECozyScreen::Title,TEXT("Cancel restart preserves completed progress"));
        {
            const auto* Saved=Cast<UCozyAdventureSave>(UGameplayStatics::LoadGameFromSlot(Session->Slot,0));
            SmokeCheck(Saved&&Saved->Complete&&Saved->SnackMask==7,TEXT("Mission checkpoints are written to the isolated save slot"));
        }
        Finish(false);break;
    }
}
