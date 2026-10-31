// Real server/client acceptance, opt-in only. Inputs are synthetic, not physical hardware.
#include "CozyCoop.h"
#include "HalloweenCostume.h"
#include "MartyGait.h"
#include "Components/PoseableMeshComponent.h"
#include "EngineUtils.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InputKeyEventArgs.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/CommandLine.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/Parse.h"
#include "HighResScreenshot.h"
#include "UnrealClient.h"

bool CostumeAlignedWithMarty(const AMartyCharacter* Pawn);

void ACozyCoopController::TickAcceptance(float Delta)
{
    if(!FParse::Param(FCommandLine::Get(),TEXT("CheweenieCoopSmoke"))||!HasActorBegunPlay())return;
    const bool Tribute=FParse::Param(FCommandLine::Get(),TEXT("CheweenieRatchetLan"));
    const bool Host=GetWorld()->GetNetMode()==NM_ListenServer;
    auto* P=Cast<ACozyCoopPet>(GetPawn());ACozyCoopPet* Other=nullptr;ACozyCoopToy* Toy=nullptr;int32 Count=0;
    for(TActorIterator<ACozyCoopPet> It(GetWorld());It;++It){++Count;if(*It!=P)Other=*It;}
    for(TActorIterator<ACozyCoopToy> It(GetWorld());It;++It)Toy=*It;
    auto Check=[&](bool OK,const TCHAR* Label){(OK?Passed:Failed).Add(Label);UE_LOG(LogTemp,Display,TEXT("COOP_CHECK %s %s"),OK?TEXT("PASS"):TEXT("FAIL"),Label);};
    auto Key=[&](FKey K,bool Down){InputKey(FInputKeyEventArgs(nullptr,INPUTDEVICEID_NONE,K,Down?IE_Pressed:IE_Released,FPlatformTime::Cycles64()));};
    auto Axis=[&](FKey K,float V){InputKey(FInputKeyEventArgs(nullptr,INPUTDEVICEID_NONE,K,V,Delta,1,FPlatformTime::Cycles64()));};
    auto Next=[&](){++Stage;StageTime=0;P->ServerAcceptanceStage(Stage);};
    if(FPlatformTime::Seconds()-Started>100){UE_LOG(LogTemp,Error,TEXT("COOP_CHECK timeout stage %d pawns %d"),Stage,Count);FPlatformMisc::RequestExitWithStatus(false,1);return;}
    if(Stage<13&&(!P||!Other||!Toy||Count!=2)){if(Stage>1)StageTime+=Delta;return;}
    if(Stage==0){if(P->AcceptanceStage<0)P->ServerAcceptanceStage(0);if(Other->AcceptanceStage<0){StageTime=0;return;}}
    if(Stage>0&&Stage<=12&&Other->AcceptanceStage<Stage)return;
    StageTime+=Delta;
    const bool First=StageTime<=Delta+.00001f;
    if(Stage>0&&Other){SawRemoteMove|=FVector::Dist2D(OtherStart,Other->GetActorLocation())>30;SawRemoteRun|=Other->GetVelocity().Size2D()>115;SawVoice|=Other->VoiceCount>0;}
    switch(Stage)
    {
    case 0:
        if(StageTime<2||P->PetId<0||Other->PetId<0)break;
        Check(Count==2,TEXT("Two separately possessed replicated pets exist"));Check(P->PetId!=Other->PetId,TEXT("Players receive unique pets"));
        Check(P->GetLocalRole()==(Host?ROLE_Authority:ROLE_AutonomousProxy),TEXT("Owner has correct network role"));
        Check(P->PetId==(Host?0:1),TEXT("Host starts Marty and guest starts Lakitu"));Check(P->GetCharacterMovement()->IsMovingOnGround(),TEXT("Owner lands on actual room floor"));
        Check(P->Skin->GetSkinnedAsset()!=nullptr,TEXT("Retained Marty skeletal asset is available"));
        if(Host){FString Error;GetWorld()->GetAuthGameMode<ACozyCoopMode>()->PreLogin(TEXT(""),TEXT("127.0.0.1"),FUniqueNetIdRepl(),Error);Check(Error.Contains(TEXT("room is full")),TEXT("Server admission rejects an occupied two-player room"));}
        Check(GetWorld()->GetGameState<ACozyCoopState>()->RatchetUnlocked==Tribute,TEXT("Host unlock availability replicates to both peers"));
        if(!Tribute){P->ServerChoosePet(3);Check(P->PetId!=3,TEXT("Locked Ratchet selection stays on the current pet"));}
        Check(!P->WearingCostume,TEXT("LAN costume defaults off for this player"));Key(Host?EKeys::T:EKeys::Gamepad_DPad_Up,true);
        OwnStart=P->GetActorLocation();OtherStart=Other->GetActorLocation();SetControlRotation(FRotator(-12,0,0));
        // Separate walking lanes prevent one player's capsule blocking the other in this test.
        Next();break;
    case 1:
        Key(Host?EKeys::T:EKeys::Gamepad_DPad_Up,false);if(StageTime<1)break;
        Check(P->WearingCostume&&Other->WearingCostume&&P->Costume->VisiblePieces()>0&&Other->Costume->VisiblePieces()>0,TEXT("Keyboard and gamepad costume choices reach both peers"));
        Check(CostumeAlignedWithMarty(Host?P:Other),TEXT("Marty's hot dog is lengthwise on both the owner and remote peer"));
        Key(EKeys::E,true);Next();break;
    case 2:
        Key(EKeys::E,false);if(StageTime<1)break;
        Check(Toy->Carrier&&(Host?Toy->Carrier==Other:Toy->Carrier==P),TEXT("Nearby guest picks up shared toy; distant host cannot steal it"));
        Next();break;
    case 3:
        if(!Host){if(First)Key(EKeys::Q,true);else if(StageTime>.2f)Key(EKeys::Q,false);}if(StageTime<1)break;
        Check(!Toy->Carrier,TEXT("Guest drop is synchronized to both peers"));Key(EKeys::W,true);Next();break;
    case 4:
        if(StageTime<.75f)break;Key(EKeys::W,false);Check(FVector::Dist2D(OwnStart,P->GetActorLocation())>35,TEXT("Keyboard movement works on network owner"));
        Key(EKeys::LeftShift,true);Next();break;
    case 5:
        Axis(EKeys::Gamepad_LeftY,.7f);if(StageTime<1)break;Axis(EKeys::Gamepad_LeftY,0);Key(EKeys::LeftShift,false);Next();break;
    case 6:
        Axis(EKeys::Gamepad_LeftY,0);if(StageTime<1)break;
        Check(SawRemoteMove,TEXT("Remote movement reached this peer"));Check(SawRemoteRun,TEXT("Remote hurry speed reached this peer"));
        Check(P->GetVelocity().Size2D()<3,TEXT("Synthetic stick release stops owner"));
        OwnStart=FVector(GetControlRotation().Yaw,0,0);Next();break;
    case 7:
        Axis(EKeys::Gamepad_RightX,.7f);if(StageTime<.6f)break;Axis(EKeys::Gamepad_RightX,0);
        Check(FMath::Abs(FRotator::NormalizeAxis(GetControlRotation().Yaw-OwnStart.X))>5,TEXT("Synthetic right stick orbits owning camera"));
        Key(EKeys::Gamepad_RightThumbstick,true);Key(EKeys::SpaceBar,true);Key(Host?EKeys::B:EKeys::Gamepad_LeftShoulder,true);Next();break;
    case 8:
        Key(EKeys::Gamepad_RightThumbstick,false);Key(EKeys::SpaceBar,false);Key(Host?EKeys::B:EKeys::Gamepad_LeftShoulder,false);
        if(StageTime<.2f)break;
        Check(P->CameraPreset==1,TEXT("Controller camera binding works in co-op"));Check(P->GetCharacterMovement()->IsFalling(),TEXT("Keyboard jump enters replicated falling movement"));Next();break;
    case 9:
        if(StageTime<1.5f)break;Check(P->VoiceCount>0&&SawVoice,TEXT("Both pet voice events replicate"));
        P->ServerVoice();P->ServerVoice(); // same-frame duplicate must be rate limited
        if(!Host)Key(EKeys::One,true);else P->ServerChoosePet(99);Next();break;
    case 10:
        Key(EKeys::One,false);if(StageTime<1)break;
        Check(P->PetId==(Host?0:1),TEXT("Occupied or invalid pet selection is rejected by server"));
        if(!Host)Key(Tribute?EKeys::Four:EKeys::Three,true);Next();break;
    case 11:
        Key(Tribute?EKeys::Four:EKeys::Three,false);if(StageTime<1)break;
        Check((Host?Other:P)->PetId==(Tribute?3:2),TEXT("Guest keyboard selection becomes the requested cat on both peers"));
        if(Tribute)Check((Host?Other:P)->PetName()==TEXT("Ratchet")&&(Host?Other:P)->Costume->VisiblePieces()==0,TEXT("Unlocked Ratchet identity and uncovered coat reach both peers"));else Check((Host?Other:P)->Costume->OutfitPet==2&&(Host?Other:P)->Costume->MaterialsReady(),TEXT("Changing to Rio replaces the outfit with her bee costume on both peers"));
        Next();break;
    case 12:
        if(!Host){if(First)Key(EKeys::Gamepad_DPad_Left,true);else if(StageTime>.2f)Key(EKeys::Gamepad_DPad_Left,false);}if(StageTime<1)break;
        Check((Host?Other:P)->PetId==(Tribute?2:1),TEXT("Guest controller can cycle away from the selected cat on both peers"));
        Check(P->VoiceCount==2,TEXT("Server rate limits repeated voice requests"));
        Check(P->IsActorTickEnabled(),TEXT("Owner motor remains active"));Key(Host?EKeys::T:EKeys::Gamepad_DPad_Up,true);Next();break;
    case 13:
        Key(Host?EKeys::T:EKeys::Gamepad_DPad_Up,false);
        if(StageTime<(FApp::CanEverRender()?20.f:1.f))break;
        Check(!P->WearingCostume&&(!Other||!Other->WearingCostume)&&P->Costume->VisiblePieces()==0,TEXT("Costumes can be removed again on both network peers"));
        if(FApp::CanEverRender()){FString Out;FParse::Value(FCommandLine::Get(),TEXT("CheweenieTestOutput="),Out);GetHighResScreenshotConfig().FilenameOverride=FPaths::Combine(Out,Host?TEXT("CoopHost.png"):TEXT("CoopGuest.png"));if(GEngine&&GEngine->GameViewport)GEngine->GameViewport->Viewport->TakeHighResScreenShot();}
        Next();break;
    case 14:
        if(StageTime<(Host?6.f:2.f))break;
        if(Host){Check(Count==1,TEXT("Guest disconnect removes the remote pawn"));Check(P->PetAvailable(1),TEXT("Disconnected pet becomes available again"));}
        {FString Out;FParse::Value(FCommandLine::Get(),TEXT("CheweenieTestOutput="),Out);auto J=MakeShared<FJsonObject>();TArray<TSharedPtr<FJsonValue>> Pass,Fail;for(auto S:Passed)Pass.Add(MakeShared<FJsonValueString>(S));for(auto S:Failed)Fail.Add(MakeShared<FJsonValueString>(S));J->SetArrayField(TEXT("passed"),Pass);J->SetArrayField(TEXT("failed"),Fail);J->SetStringField(TEXT("role"),Host?TEXT("listen-server"):TEXT("remote-client"));J->SetStringField(TEXT("input"),TEXT("Synthetic keyboard/gamepad; physical devices and separate-machine LAN unverified"));FString Json;FJsonSerializer::Serialize(J,TJsonWriterFactory<>::Create(&Json));FFileHelper::SaveStringToFile(Json,*FPaths::Combine(Out,Host?TEXT("coop-host.json"):TEXT("coop-guest.json")));FPlatformMisc::RequestExitWithStatus(false,Failed.IsEmpty()?0:1);Next();}break;
    }
}
