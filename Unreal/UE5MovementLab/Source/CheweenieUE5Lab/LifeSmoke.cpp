#include "MartyCharacter.h"
#include "DogLife.h"
#include "CozyAdventure.h"
#include "Neighbourhood.h"
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

void AMartyPlayMode::TickLifeSmoke(float Delta)
{
    SmokeTime+=Delta;auto* PC=GetWorld()->GetFirstPlayerController();auto* P=PC?Cast<AMartyCharacter>(PC->GetPawn()):nullptr;
    if(!P||!P->Life||!Adventure||!Adventure->Progress)return;auto* L=P->Life.Get();auto* D=Adventure->Progress.Get();
    FString Root=FPaths::ProjectSavedDir();FParse::Value(FCommandLine::Get(),TEXT("CheweenieTestOutput="),Root);
    auto Key=[PC](FKey K,bool Down){PC->InputKey(FInputKeyEventArgs(nullptr,INPUTDEVICEID_NONE,K,Down?IE_Pressed:IE_Released,FPlatformTime::Cycles64()));};
    auto Next=[this](){++SmokeStage;SmokeTime=0;};
    auto Place=[&](FVector V){P->GetCharacterMovement()->StopMovementImmediately();P->TeleportTo(V,FRotator::ZeroRotator);PC->SetControlRotation(FRotator(-12,0,0));};
    auto Shot=[&](const TCHAR* Name){if(FApp::CanEverRender())FScreenshotRequest::RequestScreenshot(Root/Name,true,false);};
    auto Finish=[&](bool Resume)
    {
        auto O=MakeShared<FJsonObject>();TArray<TSharedPtr<FJsonValue>> Pass,Fail;for(auto& S:SmokePassed)Pass.Add(MakeShared<FJsonValueString>(S));for(auto& S:SmokeFailed)Fail.Add(MakeShared<FJsonValueString>(S));
        O->SetStringField(TEXT("scope"),TEXT("Actual synthetic action bindings, timed pet actions and physics couch hop; teleported location/energy fixtures. Physical devices unverified."));
        O->SetArrayField(TEXT("passed"),Pass);O->SetArrayField(TEXT("failed"),Fail);FString Json;FJsonSerializer::Serialize(O,TJsonWriterFactory<>::Create(&Json));
        bool Saved=FFileHelper::SaveStringToFile(Json,*(Root/(Resume?TEXT("life-resume.json"):TEXT("life-smoke.json"))));
        if(Resume)UGameplayStatics::DeleteGameInSlot(TEXT("CheweenieLifeAcceptance"),0);
        FPlatformMisc::RequestExitWithStatus(false,Saved&&SmokeFailed.IsEmpty()?0:1);SmokeStage=1000;
    };
    if(FParse::Param(FCommandLine::Get(),TEXT("CheweenieLifeResume")))
    {
        if(SmokeStage==0&&SmokeTime>3)
        {
            SmokeCheck(D->PetEnergy>40&&D->PetEnergy<45,TEXT("Energy reloads in a second process"));
            SmokeCheck(D->CarryItem==1&&D->ItemHoles[0]==0,TEXT("Carried pebble and buried ball reload without duplication"));
            SmokeCheck(D->HoleDepths[0]==3&&D->PetTVOn,TEXT("Dug hole and TV state persist"));
            SmokeCheck(L->Activity==EDogActivity::Idle&&Adventure->Screen==ECozyScreen::Title,TEXT("Transient actions do not resume on the title screen"));
            Key(EKeys::Enter,true);Next();
        }
        else if(SmokeStage==1&&SmokeTime>.5){Key(EKeys::Enter,false);SmokeCheck(D->CarryItem==1&&Adventure->Screen==ECozyScreen::Playing,TEXT("Continue keeps the carried item at home"));Finish(true);}
        return;
    }
    switch(SmokeStage)
    {
    case 0:
        if(SmokeTime>(FApp::CanEverRender()?40:3))
        {
            SmokeCheck(L->Patches.Num()==3&&D->PetEnergy>=80,TEXT("Three dig patches and compatible default energy initialize"));
            Adventure->Start(true);D->PetEnergy=20;L->Excitement=0;Next();
        }break;
    case 1:
        if(SmokeTime>.2){SmokeCheck(L->Mood()==TEXT("Tired")&&P->GetCharacterMovement()->MaxWalkSpeed==70,TEXT("Low energy makes Marty tired and slows his walk"));Key(EKeys::LeftShift,true);Next();}break;
    case 2:
        if(SmokeTime>.2){SmokeCheck(P->GetCharacterMovement()->MaxWalkSpeed==70,TEXT("Tired Marty cannot hurry"));Key(EKeys::LeftShift,false);D->PetEnergy=80;L->Excitement=0;Place(FVector(0,-230,35));Next();}break;
    case 3:
        if(SmokeTime>.4){SmokeCheck(L->Mood()==TEXT("Normal"),TEXT("Rested Marty has a normal mood"));Key(EKeys::Gamepad_FaceButton_Left,true);Next();}break;
    case 4:
        if(SmokeTime>.2){Key(EKeys::Gamepad_FaceButton_Left,false);SmokeCheck(D->CarryItem==0&&L->Mood()==TEXT("Excited"),TEXT("Controller picks up the ball and makes Marty excited"));Neighbourhood->GoOutside(P);Place(L->Patches[0]+FVector(0,0,35));Next();}break;
    case 5:
    case 7:
    case 9:
        if(SmokeTime>.3){Key(EKeys::F,true);Next();}break;
    case 6:
    case 8:
    case 10:
        if(SmokeTime>.1)Key(EKeys::F,false);
        if(SmokeStage==10&&SmokeTime>.3&&!SmokeMoving){Shot(TEXT("LifeDig.png"));SmokeMoving=true;}
        if(SmokeTime>1.2)
        {
            const int32 Expected=(SmokeStage-4)/2;
            SmokeCheck(D->HoleDepths[0]==Expected,FString::Printf(TEXT("Timed digging advances hole to depth %d"),Expected));
            if(SmokeStage==10){SmokeCheck(D->PetEnergy<66,TEXT("Digging consumes energy"));SmokeMoving=false;}
            Next();
        }break;
    case 11:
        if(SmokeTime>.2){Key(EKeys::E,true);Next();}break;
    case 12:
        if(SmokeTime>.2){Key(EKeys::E,false);SmokeCheck(D->CarryItem==-1&&D->ItemHoles[0]==0,TEXT("Interact buries the carried ball in the open hole"));Key(EKeys::Gamepad_RightTrigger,true);Next();}break;
    case 13:
        if(SmokeTime>.1)Key(EKeys::Gamepad_RightTrigger,false);
        if(SmokeTime>1.2){SmokeCheck(D->ItemHoles[0]==-1,TEXT("Controller digging recovers buried treasure"));Key(EKeys::E,true);Next();}break;
    case 14:
        if(SmokeTime>.2){Key(EKeys::E,false);SmokeCheck(D->CarryItem==0,TEXT("Recovered ball can be picked up again"));Key(EKeys::Q,true);Next();}break;
    case 15:
        if(SmokeTime>.2){Key(EKeys::Q,false);SmokeCheck(D->CarryItem==-1&&D->ItemHoles[0]==-1,TEXT("Dropping releases the toy without burying or duplicating it"));Key(EKeys::E,true);Next();}break;
    case 16:
        if(SmokeTime>.2){Key(EKeys::E,false);Next();}break;
    case 17:
        if(SmokeTime>.2){Key(EKeys::E,true);Next();}break;
    case 18:
        if(SmokeTime>.2){Key(EKeys::E,false);SmokeCheck(D->ItemHoles[0]==0,TEXT("Dropped toy can be picked up and buried again"));P->ReturnHome();Place(L->Bowl+FVector(-35,0,35));D->PetEnergy=20;Next();}break;
    case 19:
        if(SmokeTime>.3){Key(EKeys::E,true);Next();}break;
    case 20:
        if(SmokeTime>.1)Key(EKeys::E,false);
        if(SmokeTime>.4&&!SmokeMoving){Shot(TEXT("LifeKitchen.png"));SmokeMoving=true;}
        if(SmokeTime>1.8){SmokeCheck(D->PetEnergy>=49&&L->Mood()==TEXT("Excited"),TEXT("Eating kitchen food restores energy and excitement"));SmokeYaw=D->PetEnergy;Adventure->Interact();SmokeCheck(L->Activity!=EDogActivity::Eating,TEXT("Food cooldown prevents instant repeated meals"));SmokeMoving=false;Next();}break;
    case 21:
        if(SmokeTime>.2){P->ReturnHome();SmokeMoving=false;Next();}break;
    case 22:
        if(SmokeTime>.6&&!SmokeMoving){Key(EKeys::SpaceBar,true);SmokeMoving=true;SmokePitch=SmokeTime;}
        if(SmokeMoving&&SmokeTime-SmokePitch>.1)Key(EKeys::SpaceBar,false);
        if(SmokeMoving&&SmokeTime-SmokePitch>.25){SmokeCheck(P->GetCharacterMovement()->IsFalling()&&P->GetActorLocation().Z>65,TEXT("Space performs a real physics jump"));SmokeCheck(D->PetEnergy<SmokeYaw-6,TEXT("Jumping spends energy"));Shot(TEXT("LifeJump.png"));SmokeMoving=false;Next();}break;
    case 23:
        if(SmokeTime>1.2){Place(FVector(190,-100,35));Next();}break;
    case 24:
        if(SmokeTime>.4){Key(EKeys::Gamepad_FaceButton_Left,true);Next();}break;
    case 25:
        if(SmokeTime>.1)Key(EKeys::Gamepad_FaceButton_Left,false);
        if(SmokeTime>1.9){UE_LOG(LogTemp,Display,TEXT("COUCH_HOP position=%s grounded=%d"),*P->GetActorLocation().ToString(),P->GetCharacterMovement()->IsMovingOnGround());SmokeCheck(L->OnCouch(),TEXT("Assisted controller hop lands on the real sofa collider"));D->PetEnergy=60;Key(EKeys::E,true);Next();}break;
    case 26:
        if(SmokeTime>.1)Key(EKeys::E,false);
        if(SmokeTime>1.2){SmokeCheck(L->Activity==EDogActivity::WatchingTV&&D->PetTVOn&&D->PetEnergy>61,TEXT("Watching original TV on the couch restores energy"));Shot(TEXT("LifeCouchTV.png"));Next();}break;
    case 27:
        if(SmokeTime>.2){Key(EKeys::Escape,true);Next();}break;
    case 28:
        if(SmokeTime>.2){Key(EKeys::Escape,false);SmokeYaw=D->PetEnergy;Next();}break;
    case 29:
        if(SmokeTime>.8){SmokeCheck(FMath::IsNearlyEqual(D->PetEnergy,SmokeYaw,.01f),TEXT("Pause freezes pet energy and action timers"));Key(EKeys::Escape,true);Next();}break;
    case 30:
        if(SmokeTime>.2){Key(EKeys::Escape,false);Key(EKeys::S,true);Next();}break;
    case 31:
        if(SmokeTime>.3){Key(EKeys::S,false);SmokeCheck(L->Activity==EDogActivity::Idle,TEXT("Walking away stops TV rest"));Neighbourhood->GoOutside(P);Place(Neighbourhood->Entrance+FVector(450,-300,35));Next();}break;
    case 32:
        if(SmokeTime>.4){Key(EKeys::E,true);Next();}break;
    case 33:
        if(SmokeTime>.2){Key(EKeys::E,false);SmokeCheck(D->CarryItem==1&&D->ItemHoles[0]==0,TEXT("Another item can be carried while the first stays buried"));D->PetEnergy=42;Adventure->Save();Next();}break;
    case 34:
        if(SmokeTime>.2)Finish(false);break;
    }
}
