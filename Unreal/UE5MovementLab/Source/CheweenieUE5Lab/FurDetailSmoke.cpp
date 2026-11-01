// Real-map quality selection and isolated cross-process save acceptance.
#include "MartyCharacter.h"
#include "MartyGait.h"
#include "CozyAdventure.h"
#include "Components/PoseableMeshComponent.h"
#include "Engine/SkinnedAsset.h"
#include "GameFramework/PlayerController.h"
#include "InputKeyEventArgs.h"
#include "InputCoreTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/Parse.h"
#include "Misc/CommandLine.h"
#include "Misc/App.h"
#include "UnrealClient.h"

void AMartyPlayMode::TickFurDetailSmoke(float Delta)
{
    SmokeTime+=Delta;
    auto* PC=GetWorld()->GetFirstPlayerController();
    auto* Pawn=PC?Cast<AMartyCharacter>(PC->GetPawn()):nullptr;
    auto* S=Adventure.Get();if(!Pawn||!S||!S->Progress)return;
    auto* D=S->Progress.Get();
    const bool Medium=FParse::Param(FCommandLine::Get(),TEXT("CheweenieFurDetailMedium"));
    const bool Low=FParse::Param(FCommandLine::Get(),TEXT("CheweenieFurDetailLow"));
    FString Root=FPaths::ProjectSavedDir();FParse::Value(FCommandLine::Get(),TEXT("CheweenieTestOutput="),Root);
    auto Model=[&](const TCHAR* Folder){return Pawn->Skin->GetSkinnedAsset()&&Pawn->Skin->GetSkinnedAsset()->GetPathName().Contains(Folder)&&Pawn->Gait->Ready;};
    auto Key=[PC](FKey K,bool Down){PC->InputKey(FInputKeyEventArgs(nullptr,INPUTDEVICEID_NONE,K,Down?IE_Pressed:IE_Released,FPlatformTime::Cycles64()));};
    auto Next=[this](){++SmokeStage;SmokeTime=0;};
    auto Finish=[&]()
    {
        auto Report=MakeShared<FJsonObject>();auto Strings=[](const TArray<FString>& In){TArray<TSharedPtr<FJsonValue>> Out;for(const auto& V:In)Out.Add(MakeShared<FJsonValueString>(V));return Out;};
        Report->SetStringField(TEXT("scope"),TEXT("Real map; synthetic keyboard/gamepad with menu-selection and progress fixtures; three-process isolated save checks. Physical controllers unverified."));
        Report->SetArrayField(TEXT("passed"),Strings(SmokePassed));Report->SetArrayField(TEXT("failed"),Strings(SmokeFailed));
        FString Json;FJsonSerializer::Serialize(Report,TJsonWriterFactory<>::Create(&Json));
        const TCHAR* File=Medium?TEXT("fur-detail-medium.json"):Low?TEXT("fur-detail-low.json"):TEXT("fur-detail.json");
        const bool Saved=FFileHelper::SaveStringToFile(Json,*(Root/File));
        if(Low)UGameplayStatics::DeleteGameInSlot(S->Slot,0);
        FPlatformMisc::RequestExitWithStatus(false,Saved&&SmokeFailed.IsEmpty()?0:1);SmokeStage=100;
    };
    if(Medium||Low)
    {
        if(SmokeStage==0&&SmokeTime>3)
        {
            if(Medium)
            {
                SmokeCheck(S->FurDetail()==1&&D->FurDetailPreference==1,TEXT("Medium fur preference reloads in another process"));
                SmokeCheck(Model(TEXT("MartyPhotoReference/")),TEXT("Reload selects the actual medium coat and valid gait"));
                SmokeCheck(D->SnackMask==1&&FMath::IsNearlyEqual(D->Music,.35f),TEXT("Fur reload preserves patrol and sound preferences"));
            }
            else
            {
                SmokeCheck(S->FurDetail()==0&&D->FurDetailPreference==1,TEXT("Low setting reloads while remembering the preferred enabled coat"));
                SmokeCheck(Model(TEXT("MartyConnected/")),TEXT("Low reload selects the actual smooth coat"));
            }
            S->ToggleFeature(ECozyFeature::DetailedCoat);Next();
        }
        else if(SmokeStage==1&&SmokeTime>.3)
        {
            if(Medium)SmokeCheck(S->FurDetail()==0&&D->FurDetailPreference==1&&Model(TEXT("MartyConnected/")),TEXT("Legacy coat-off switch retains Medium and saves Low"));
            else
            {
                SmokeCheck(S->FurDetail()==1&&Model(TEXT("MartyPhotoReference/")),TEXT("Legacy coat-on switch restores Medium with a valid rig"));
                SmokeCheck(D->SnackMask==1&&FMath::IsNearlyEqual(D->Music,.35f),TEXT("Quality switches retain existing patrol and sound state"));
            }
            Finish();
        }
        return;
    }
    switch(SmokeStage)
    {
    case 0:
        if(SmokeTime<(FApp::CanEverRender()?50:3))break;
        SmokeCheck(S->FurDetail()==2,TEXT("Fresh saves default to High fur detail"));
        SmokeCheck(Model(TEXT("MartyVisibleStrands/")),TEXT("High detail selects the new raised-hair mesh"));
        SmokeCheck(S->Slot==TEXT("CheweenieFurDetailAcceptance"),TEXT("Fur acceptance uses an isolated save slot"));
        Key(EKeys::Enter,true);Next();break;
    case 1:
        if(SmokeTime<.2)break;Key(EKeys::Enter,false);
        SmokeCheck(S->Screen==ECozyScreen::Playing,TEXT("Keyboard starts the real game before adjusting detail"));
        Key(EKeys::Escape,true);Next();break;
    case 2:
        if(SmokeTime<.2)break;Key(EKeys::Escape,false);
        SmokeCheck(S->Screen==ECozyScreen::Pause,TEXT("Keyboard opens Pause for fur controls"));
        S->Selection=2;Key(EKeys::Gamepad_FaceButton_Bottom,true);Next();break;
    case 3:
        if(SmokeTime<.2)break;Key(EKeys::Gamepad_FaceButton_Bottom,false);
        SmokeCheck(S->Screen==ECozyScreen::Controls,TEXT("A-Cross opens Controls from Pause"));
        S->Selection=3;SmokeStage=30;SmokeTime=0;break;
    case 30:
        // Enter and A share an Enhanced Input action; settle the release first.
        if(SmokeTime<.1)break;Key(EKeys::Enter,true);SmokeStage=4;SmokeTime=0;break;
    case 4:
        if(SmokeTime<.2)break;Key(EKeys::Enter,false);
        S->Selection=S->FeatureGroup().Find(ECozyFeature::DetailedCoat);
        SmokeCheck(S->Screen==ECozyScreen::Features&&S->FeaturePage==2&&S->Menu().IsValidIndex(S->Selection)&&S->Menu()[S->Selection].Contains(TEXT("HIGH")),TEXT("Graphics controls display the current High fur level"));
        // Establish the preservation fixture after gameplay has paused and settled.
        D->SnackMask=1;D->Music=.35f;SmokePosition=Pawn->GetActorLocation();
        Key(EKeys::Left,true);Next();break;
    case 5:
        if(SmokeTime<.3)break;Key(EKeys::Left,false);
        SmokeCheck(S->FurDetail()==1,TEXT("Keyboard Left lowers High to Medium"));
        SmokeCheck(Model(TEXT("MartyPhotoReference/")),TEXT("Medium changes the actual mesh live and refreshes gait"));
        Key(EKeys::Gamepad_DPad_Right,true);Next();break;
    case 6:
        if(SmokeTime<.3)break;Key(EKeys::Gamepad_DPad_Right,false);
        SmokeCheck(S->FurDetail()==2&&Model(TEXT("MartyVisibleStrands/")),TEXT("D-pad Right restores High and the raised hairs"));
        SmokeStage=60;SmokeTime=0;break;
    case 60:
        // Keyboard Right and D-pad Right also share a single action.
        if(SmokeTime<.1)break;Key(EKeys::Right,true);SmokeStage=7;SmokeTime=0;break;
    case 7:
        if(SmokeTime<.3)break;Key(EKeys::Right,false);
        SmokeCheck(S->FurDetail()==0&&Model(TEXT("MartyConnected/")),TEXT("Keyboard Right wraps High to the smooth Low model"));
        Key(EKeys::Gamepad_FaceButton_Bottom,true);Next();break;
    case 8:
        if(SmokeTime<.3)break;Key(EKeys::Gamepad_FaceButton_Bottom,false);
        SmokeCheck(S->FurDetail()==1&&Model(TEXT("MartyPhotoReference/")),TEXT("A-Cross cycles Low to Medium without leaving the menu"));
        if(FApp::CanEverRender())FScreenshotRequest::RequestScreenshot(Root/TEXT("FurDetailMenu.png"),true,false);
        Next();break;
    case 9:
        if(SmokeTime<.5)break;Key(EKeys::Enter,true);Next();break;
    case 10:
        if(SmokeTime<.3)break;Key(EKeys::Enter,false);
        SmokeCheck(S->FurDetail()==2&&Model(TEXT("MartyVisibleStrands/")),TEXT("Enter cycles Medium to High live"));
        Key(EKeys::Left,true);Next();break;
    case 11:
        if(SmokeTime<.3)break;Key(EKeys::Left,false);
        {const float Shift=FVector::Dist(SmokePosition,Pawn->GetActorLocation());UE_LOG(LogTemp,Display,TEXT("Fur preservation: actor shift %.3f cm; snack mask %d; music %.3f"),Shift,D->SnackMask,D->Music);SmokeCheck(Shift<1&&D->SnackMask==1&&FMath::IsNearlyEqual(D->Music,.35f),TEXT("Changing detail keeps Marty still and preserves patrol and sound state"));}
        {const auto* Saved=Cast<UCozyAdventureSave>(UGameplayStatics::LoadGameFromSlot(S->Slot,0));SmokeCheck(Saved&&Saved->FurDetailPreference==1&&(Saved->DisabledFeatures&(1<<uint8(ECozyFeature::DetailedCoat)))==0,TEXT("The selected Medium quality is written to the save"));}
        Finish();break;
    }
}
