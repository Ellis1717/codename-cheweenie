#include "CozyModeHub.h"
#include "CozyAdventure.h"
#include "CozyCoop.h"
#include "MartyCharacter.h"
#include "Neighbourhood.h"
#include "HalloweenSpecial.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Engine/SkinnedAsset.h"
#include "Components/PoseableMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "InputKeyEventArgs.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/Parse.h"
#include "Misc/CommandLine.h"
#include "Misc/App.h"
#include "UnrealClient.h"

// Opt-in, isolated fixtures. The game-instance state survives actual map travel.
void UCozyModeHub::TickAcceptance(float Delta)
{
    const bool Archive=FParse::Param(FCommandLine::Get(),TEXT("CheweenieHubArchiveSmoke"));
    const bool Lan=FParse::Param(FCommandLine::Get(),TEXT("CheweenieHubLan"));
    const bool Guest=FParse::Param(FCommandLine::Get(),TEXT("CheweenieHubGuest"));
    if(!Archive&&!FParse::Param(FCommandLine::Get(),TEXT("CheweenieHubSmoke")))return;
    TestTime+=Delta;TestTotal+=Delta;
    auto* S=Session();auto* PC=UGameplayStatics::GetPlayerController(this,0);if(!PC)return;
    auto* Pawn=Cast<AMartyCharacter>(PC->GetPawn());if(!Pawn)return;
    FString Root=FPaths::ProjectSavedDir();FParse::Value(FCommandLine::Get(),TEXT("CheweenieTestOutput="),Root);
    auto Check=[&](bool OK,const TCHAR* Label){(OK?TestPassed:TestFailed).Add(Label);};
    auto Next=[&](){++TestStage;TestTime=0;};
    auto Key=[&](FKey K,bool Down){PC->InputKey(FInputKeyEventArgs(nullptr,INPUTDEVICEID_NONE,K,Down?IE_Pressed:IE_Released,FPlatformTime::Cycles64()));};
    auto Select=[&](int32 Row){Selection=Row;if(S)S->Selection=Row;};
    auto Pick=[&](int32 Row){Select(Row);if(S)S->Confirm();else Confirm();};
    auto OpenMenu=[&](){if(S)S->OpenModes();else CastChecked<ACozyCoopController>(PC)->SetHubMenu(true);};
    auto Finish=[&]()
    {
        auto Report=MakeShared<FJsonObject>();auto Strings=[](const TArray<FString>& Values){TArray<TSharedPtr<FJsonValue>> R;for(const auto& V:Values)R.Add(MakeShared<FJsonValueString>(V));return R;};
        Report->SetStringField(TEXT("scope"),TEXT("Actual map and map travel; synthetic input plus menu/progress fixtures. Physical controllers and separate PCs unverified."));
        Report->SetArrayField(TEXT("passed"),Strings(TestPassed));Report->SetArrayField(TEXT("failed"),Strings(TestFailed));
        FString Json;FJsonSerializer::Serialize(Report,TJsonWriterFactory<>::Create(&Json));
        const TCHAR* File=Archive?TEXT("hub-archive-return.json"):Lan?(Guest?TEXT("hub-guest.json"):TEXT("hub-host.json")):TEXT("hub-smoke.json");
        const bool Saved=FFileHelper::SaveStringToFile(Json,*(Root/File));TestStage=999;
        FPlatformMisc::RequestExitWithStatus(false,Saved&&TestFailed.IsEmpty()?0:1);
    };
    if(TestStage==999)return;
    if(TestTotal>210){Check(false,TEXT("Mode hub acceptance completed within time budget"));Finish();return;}
    if(Archive)
    {
        if(TestStage||TestTime<3||!S||!S->Progress)return;
        if(FParse::Param(FCommandLine::Get(),TEXT("CheweenieArchiveReturned")))
        {
            Check(S->Screen==ECozyScreen::Modes,TEXT("Latest hub opens after the archived executable exits"));
            Check(S->Progress->SnackMask==1&&FurDetail()==1,TEXT("Archive round trip retains latest progress and local detail"));Finish();
        }
        else {S->Progress->SnackMask=1;S->Save();if(FurDetail()!=1)AdjustFur(-1);OpenMenu();NewPage(ECozyHubPage::Archives);if(Archives.IsEmpty()){Check(false,TEXT("Archive fixture exists"));Finish();return;}Pick(0);TestStage=1;}
        return;
    }
    if(Lan)
    {
        if(TestTime<.4)return;
        auto* Pet=Cast<ACozyCoopPet>(Pawn);TArray<ACozyCoopPet*> Pets;
        for(TActorIterator<ACozyCoopPet> It(GetWorld());It;++It)Pets.Add(*It);
        if(TestStage==0&&TestTime>3&&S&&S->Progress)
        {
            Check(S->Screen==ECozyScreen::Modes,TEXT("LAN starts from the unified solo hub"));
            S->Progress->SnackMask=1;S->Save();AdjustFur(Guest?1:-1);
            if(Guest){Pick(2);Octets[0]=127;Octets[1]=Octets[2]=0;Octets[3]=1;Next();Pick(0);}
            else{Pick(1);Next();Pick(0);}return;
        }
        if(TestStage==1&&Pet&&Pets.Num()==2&&TestTime>2)
        {
            Check(GetWorld()->GetNetMode()==(Guest?NM_Client:NM_ListenServer),TEXT("Hub travel creates the expected actual network role"));
            Check(Pets[0]->PetId!=Pets[1]->PetId,TEXT("Two pets replicate with distinct selections"));
            Check(FurDetail()==(Guest?0:1),TEXT("Each PC retains its independent fur detail"));
            if(Guest){Pet->ServerAcceptanceStage(20);Key(EKeys::Escape,true);}
            Next();return;
        }
        if(TestStage==2)
        {
            if(Guest)
            {
                Key(EKeys::Escape,false);Check(CastChecked<ACozyCoopController>(PC)->MenuOpen,TEXT("Keyboard opens client mode menu"));
                Request(TEXT("patrol"));Pick(0);Check(GetWorld()->GetNetMode()==NM_Client&&Page==ECozyHubPage::Modes,TEXT("Client can cancel leaving without disconnecting"));
                Request(TEXT("patrol"));Select(1);Next();Key(EKeys::Gamepad_FaceButton_Bottom,true);
            }
            else if(Pets.Num()==1&&TestTime>4){Check(true,TEXT("Host remains playable after guest leaves"));OpenMenu();Request(TEXT("hub"));Next();Pick(1);}
            return;
        }
        if(TestStage==3&&S&&TestTime>1)
        {
            Key(EKeys::Gamepad_FaceButton_Bottom,false);
            Check(GetWorld()->GetNetMode()==NM_Standalone,TEXT("Room departure returns to actual standalone world"));
            Check(S->Progress->SnackMask==1&&FurDetail()==(Guest?0:1),TEXT("LAN round trip preserves each checkpoint and detail"));
            Check(S->Screen==(Guest?ECozyScreen::Playing:ECozyScreen::Modes),TEXT("Confirmed mode resumes its intended screen"));Finish();
        }
        return;
    }
    if(TestTime<.35)return;
    switch(TestStage)
    {
    case 0:
        if(TestTime<(FApp::CanEverRender()?25:3)||!S||!S->Progress)break;
        Check(S->Slot==TEXT("CheweenieHubAcceptance"),TEXT("Hub test uses isolated progress"));
        Check(S->Screen==ECozyScreen::Modes&&Rows().Num()==9,TEXT("Unified startup shows the core adventure and utility choices"));
        Check(!RatchetAvailable(),TEXT("Fresh profile keeps Ratchet gated"));
        if(FApp::CanEverRender())FScreenshotRequest::RequestScreenshot(Root/TEXT("ModeHub.png"),true,false);
        Request(TEXT("ratchet"));Check(S->Screen==ECozyScreen::Modes&&!Status.IsEmpty(),TEXT("Locked Ratchet choice stays in the hub"));
        S->Progress->SnackMask=1;S->Progress->Music=.35f;S->Save();Next();break;
    case 1:
        Pick(3);Check(Page==ECozyHubPage::Graphics,TEXT("Graphics opens within the hub"));Select(0);Key(EKeys::Left,true);Next();break;
    case 2:
        Key(EKeys::Left,false);Check(FurDetail()==1,TEXT("Keyboard changes hub fur from High to Medium"));Next();break;
    case 3:
        Key(EKeys::Gamepad_DPad_Right,true);Next();break;
    case 4:
        Key(EKeys::Gamepad_DPad_Right,false);Check(FurDetail()==2,TEXT("Synthetic D-pad changes hub detail back to High"));CastChecked<AMartyHUD>(PC->GetHUD())->NotifyHitBoxClick(TEXT("CozyMinus_0"));Next();break;
    case 5:
        Check(FurDetail()==1,TEXT("Mouse hitbox adjusts hub detail live"));Back();Pick(0);Next();break;
    case 6:
        Check(S&&S->Screen==ECozyScreen::Playing,TEXT("Cozy Patrol starts from hub"));
        Check(S&&S->Progress->SnackMask==1&&FMath::IsNearlyEqual(S->Progress->Music,.35f),TEXT("Mode entry retains checkpoint and sound"));
        Key(EKeys::Escape,true);Next();break;
    case 7:
        Key(EKeys::Escape,false);Check(S&&S->Screen==ECozyScreen::Pause,TEXT("Escape opens real solo pause"));Select(8);Next();break;
    case 8:
        Key(EKeys::Gamepad_FaceButton_Bottom,true);Next();break;
    case 9:
        Key(EKeys::Gamepad_FaceButton_Bottom,false);Check(S&&S->Screen==ECozyScreen::Modes,TEXT("A-Cross opens Modes from Pause"));Request(TEXT("explore"));Next();break;
    case 10:
        Check(S&&GetWorld()->GetAuthGameMode<AMartyPlayMode>()->Neighbourhood->Outside,TEXT("Explore mode enters the actual exterior"));
        OpenMenu();Request(TEXT("halloween"));Next();break;
    case 11:
        Check(S&&S->Progress->HalloweenActive,TEXT("Halloween mode activates Bork or Treat"));
        S->Progress->HalloweenStampMask=1;S->Save();OpenMenu();Pick(0);Next();break;
    case 12:
        Check(S&&S->Progress->HalloweenActive&&S->Progress->HalloweenStampMask==1,TEXT("Core Continue keeps the active world event and collection"));
        GetWorld()->GetAuthGameMode<AMartyPlayMode>()->Festival->Toggle();
        Check(S&&!S->Progress->HalloweenActive&&S->Progress->HalloweenStampMask==1,TEXT("Returning to patrol disables event and retains earned stickers"));
        OpenMenu();Pick(2);Octets[0]=Octets[1]=Octets[2]=Octets[3]=0;Pick(0);
        Check(!Connecting&&!Status.IsEmpty(),TEXT("Invalid join address is rejected without travel"));
        Octets[0]=127;Octets[3]=1;Back();Pick(1);Next();break;
    case 13:
        Check(Page==ECozyHubPage::Host,TEXT("Host page opens without leaving solo"));
        if(FApp::CanEverRender())FScreenshotRequest::RequestScreenshot(Root/TEXT("ModeHubHost.png"),true,false);
        Next();break;
    case 14:
        Next();Pick(0);break;
    case 15:
        if(S||GetWorld()->GetNetMode()!=NM_ListenServer)break;
        Check(GetWorld()->URL.Port==Port,TEXT("Hub host travels into a real listen server on selected port"));
        Check(FurDetail()==1&&Pawn->Skin->GetSkinnedAsset()->GetPathName().Contains(TEXT("MartyPhotoReference/")),TEXT("Medium local detail survives solo-to-room travel"));
        OpenMenu();Request(TEXT("patrol"));Check(Page==ECozyHubPage::Leave,TEXT("Leaving a hosted room requires a concrete confirmation"));Pick(0);
        Check(Page==ECozyHubPage::Modes&&GetWorld()->GetNetMode()==NM_ListenServer,TEXT("Keep playing cancels room departure"));
        Request(TEXT("patrol"));Next();Pick(1);break;
    case 16:
        if(!S||S->Screen!=ECozyScreen::Playing||GetWorld()->GetNetMode()!=NM_Standalone)break;
        Check(FurDetail()==1&&S->Progress->SnackMask==1,TEXT("Confirmed room departure restores standalone checkpoint and detail"));
        S->Progress->RatchetUnlocked=true;S->Save();OpenMenu();Next();Request(TEXT("ratchet"));break;
    case 17:
        if(S)break;
        Check(GetWorld()->URL.HasOption(TEXT("RatchetVisit"))&&Cast<ACozyCoopPet>(Pawn)&&Cast<ACozyCoopPet>(Pawn)->PetId==3,TEXT("Unlocked Ratchet mode travels into the actual memory walk"));
        OpenMenu();Next();Request(TEXT("hub"));break;
    case 18:
        if(!S||S->Screen!=ECozyScreen::Modes)break;
        Check(RatchetAvailable()&&S->Progress->HalloweenStampMask==1,TEXT("Memory return preserves unlock and Halloween collection"));
        Pick(2);Octets[0]=127;Octets[1]=0;Octets[2]=0;Octets[3]=1;Port=18889;Pick(0);Next();break;
    case 19:
        if(TestTime<1)break;
        Check(Connecting,TEXT("Join begins an actual pending network connection"));Back();
        Check(!Connecting&&Page==ECozyHubPage::Modes,TEXT("Back cancels pending connection and restores mode menu"));
        Pick(4);Next();break;
    case 20:
        Check(Page==ECozyHubPage::Archives&&Rows().Last()==TEXT("Back"),TEXT("Archive page remains navigable with or without local builds"));
        Check(S&&S->Progress->Music==.35f&&FurDetail()==1,TEXT("Mode sequence retains local settings"));if(FApp::CanEverRender())FScreenshotRequest::RequestScreenshot(Root/TEXT("ModeHubArchives.png"),true,false);Next();break;
    case 21:Finish();break;
    }
}
