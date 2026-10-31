#include "MartyCharacter.h"
#include "HalloweenSpecial.h"
#include "CozyAdventure.h"
#include "CozyNavigation.h"
#include "Neighbourhood.h"
#include "DogLife.h"
#include "CatCompanions.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "Engine/Texture2D.h"
#include "InputKeyEventArgs.h"
#include "Kismet/GameplayStatics.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/Parse.h"
#include "Misc/CommandLine.h"
#include "UnrealClient.h"

void AMartyPlayMode::TickFestivalSmoke(float Delta)
{
    SmokeTime+=Delta;auto* PC=GetWorld()->GetFirstPlayerController();auto* P=PC?Cast<AMartyCharacter>(PC->GetPawn()):nullptr;
    if(!P||!Adventure||!Adventure->Progress||!Festival||Festival->Porches.Num()!=8)return;
    auto* S=Adventure.Get();auto* D=S->Progress.Get();auto* F=Festival.Get();
    FString Root=FPaths::ProjectSavedDir();FParse::Value(FCommandLine::Get(),TEXT("CheweenieTestOutput="),Root);
    auto Next=[&](){++SmokeStage;SmokeTime=0;};
    auto Key=[PC](FKey K,bool Down){PC->InputKey(FInputKeyEventArgs(nullptr,INPUTDEVICEID_NONE,K,Down?IE_Pressed:IE_Released,FPlatformTime::Cycles64()));};
    auto Shot=[&](const TCHAR* Name){if(FApp::CanEverRender())FScreenshotRequest::RequestScreenshot(Root/Name,true,false);};
    auto Finish=[&](bool Resume){auto J=MakeShared<FJsonObject>();TArray<TSharedPtr<FJsonValue>> Pass,Fail;for(auto V:SmokePassed)Pass.Add(MakeShared<FJsonValueString>(V));for(auto V:SmokeFailed)Fail.Add(MakeShared<FJsonValueString>(V));J->SetArrayField(TEXT("passed"),Pass);J->SetArrayField(TEXT("failed"),Fail);J->SetStringField(TEXT("scope"),TEXT("Actual game map, synthetic keyboard/controller, isolated checkpoint plus position/action/cosmetic fixtures; physical controllers unverified. Local map source details excluded."));J->SetBoolField(TEXT("private_layout"),Neighbourhood->PrivateMapLoaded);FString Json;FJsonSerializer::Serialize(J,TJsonWriterFactory<>::Create(&Json));const bool Saved=FFileHelper::SaveStringToFile(Json,*(Root/(Resume?TEXT("festival-resume.json"):TEXT("festival-smoke.json"))));if(Resume)UGameplayStatics::DeleteGameInSlot(TEXT("CheweenieFestivalAcceptance"),0);FPlatformMisc::RequestExitWithStatus(false,Saved&&SmokeFailed.IsEmpty()?0:1);SmokeStage=1000;};
    if(FParse::Param(FCommandLine::Get(),TEXT("CheweenieFestivalResume")))
    {
        if(SmokeStage==0&&SmokeTime>3){SmokeCheck(S->Screen==ECozyScreen::Halloween,TEXT("Standalone special launch flag opens the seasonal menu on reload"));SmokeCheck(!D->HalloweenActive&&D->HalloweenStampMask==3,TEXT("Partial replay and disabled Halloween preference survive another process"));SmokeCheck(D->HalloweenBadge&&!D->HalloweenRoundComplete,TEXT("Permanent Pumpkin Pal badge survives replay and reload"));SmokeCheck(D->SnackMask==1&&D->VisitIndex==0&&D->RatchetUnlocked,TEXT("Main patrol and previously earned character remain intact after reload"));SmokeCheck(!F->DecorationsVisible(),TEXT("Paused event restores ordinary scenery visibility on reload"));S->OpenHalloween();S->Selection=0;S->Confirm();Next();}
        else if(SmokeStage==1&&SmokeTime>.3f){SmokeCheck(F->Active()&&F->StampCount()==2&&Neighbourhood->Outside,TEXT("Continue resumes the saved sticker trail outside"));Finish(true);}return;
    }
    if(SmokeStage>=16&&SmokeStage<=21)
    {
        if(SmokeTime<.2f)return;const int32 I=SmokeStage-14;P->TeleportTo(F->Porches[I].Position,FRotator::ZeroRotator);S->Interact();SmokeCheck((D->HalloweenStampMask&(1<<I))!=0,FString::Printf(TEXT("Porch %d awards its own unique sticker"),I+1));Next();return;
    }
    switch(SmokeStage)
    {
    case 0:
        if(SmokeTime<3)break;SmokeCheck(!D->HalloweenActive&&F->StampCount()==0&&!D->HalloweenBadge,TEXT("A new checkpoint starts with an optional empty event"));
        D->SnackMask=1;D->VisitIndex=0;D->RatchetUnlocked=false;S->Save();S->Selection=7;Key(EKeys::Enter,true);Next();break;
    case 1:
        Key(EKeys::Enter,false);if(SmokeTime<.3f)break;SmokeCheck(S->Screen==ECozyScreen::Halloween&&S->Menu().Num()==5,TEXT("Keyboard opens the Halloween menu from the ordinary title"));
        {auto* T=LoadObject<UTexture2D>(nullptr,TEXT("/Game/Adventure/HalloweenSpecial/HalloweenCover.HalloweenCover"));SmokeCheck(T&&T->GetSizeX()>1000,TEXT("Original seasonal title illustration loads in the real game"));}
        Shot(TEXT("FestivalTitle.png"));SmokeStage=12;SmokeTime=0;break;
    case 12:
        if(SmokeTime<.5f)break;S->Selection=2;Key(EKeys::Gamepad_FaceButton_Bottom,true);SmokeStage=2;SmokeTime=0;break;
    case 2:
        Key(EKeys::Gamepad_FaceButton_Bottom,false);if(SmokeTime<.4f)break;SmokeCheck(S->Screen==ECozyScreen::Wardrobe,TEXT("Controller opens optional costumes from the event menu"));Key(EKeys::Gamepad_FaceButton_Right,true);Next();break;
    case 3:
        Key(EKeys::Gamepad_FaceButton_Right,false);if(SmokeTime<.3f)break;SmokeCheck(S->Screen==ECozyScreen::Halloween,TEXT("Costume Back returns to the seasonal menu"));S->Selection=0;Key(EKeys::Gamepad_FaceButton_Bottom,true);Next();break;
    case 4:
        Key(EKeys::Gamepad_FaceButton_Bottom,false);if(SmokeTime<.4f)break;
        SmokeCheck(F->Active()&&Neighbourhood->Outside&&S->Screen==ECozyScreen::Playing,TEXT("Controller starts Bork or Treat through the real outside transition"));
        SmokeCheck(D->SnackMask==1&&D->VisitIndex==0&&!D->RatchetUnlocked&&D->CostumeMask==0,TEXT("Starting the event preserves patrol, unlock and costume choices"));
        SmokeCheck(F->DecorationsVisible()&&F->DecorationsCollisionFree(),TEXT("Optional porch decorations render without collision or navigation blocking"));
        SmokeCheck(Navigation->Goal().Label.Contains(TEXT("Bork or Treat")),TEXT("Existing map and compass track the Halloween objective"));
        for(int32 I=0;I<8;++I){FCollisionQueryParams Q(SCENE_QUERY_STAT(FestivalApproach),false,P);SmokeCheck(!GetWorld()->OverlapBlockingTestByChannel(F->Porches[I].Position,FQuat::Identity,ECC_WorldStatic,FCollisionShape::MakeSphere(14),Q),FString::Printf(TEXT("Porch %d has a clear doorstep approach"),I+1));}
        P->TeleportTo(F->Porches[0].Position+FVector(0,120,5),FRotator(0,-90,0));PC->SetControlRotation(FRotator(-12,-90,0));SmokePosition=P->GetActorLocation();Key(EKeys::W,true);Next();break;
    case 5:
        if(SmokeTime<.55f)break;Key(EKeys::W,false);SmokeCheck(FVector::Dist2D(P->GetActorLocation(),SmokePosition)>25&&!F->Prompt().IsEmpty(),TEXT("Keyboard can walk up to a decorated porch and reveal the prompt"));Key(EKeys::E,true);Next();break;
    case 6:
    {
        Key(EKeys::E,false);if(SmokeTime<.3f)break;SmokeCheck(D->HalloweenStampMask==1&&S->Message.Contains(TEXT("Fox Ellis")),TEXT("Real interact collects one sticker with the original family greeting"));S->Interact();SmokeCheck(D->HalloweenStampMask==1&&S->Message.Contains(TEXT("Already")),TEXT("Repeated visits cannot farm duplicate stickers"));
        // A posed capture fixture shows the retained optional costume and decoration scale.
        D->CostumeMask=1;const FVector Target=F->Porches[0].Position+FVector(0,0,65);auto* C=GetWorld()->SpawnActor<ACameraActor>(Target+FVector(240,340,125),(FVector(-240,-340,-125)).Rotation());C->GetCameraComponent()->FieldOfView=62;PC->SetViewTarget(C);Next();break;
    }
    case 7:
        if(SmokeTime<4)break;Shot(TEXT("FestivalPorch.png"));Next();break;
    case 8:
        if(SmokeTime<.5f)break;PC->SetViewTarget(P);P->TeleportTo(Neighbourhood->Entrance+FVector(-800,-800,0),FRotator::ZeroRotator);SmokeCheck(!F->Interact(P)&&D->HalloweenStampMask==1,TEXT("A distant interact cannot award a doorstep sticker"));S->Pause();P->TeleportTo(F->Porches[1].Position,FRotator::ZeroRotator);S->Interact();SmokeCheck(D->HalloweenStampMask==1,TEXT("Paused menus cannot collect stickers"));S->Back();Key(EKeys::Gamepad_FaceButton_Left,true);Next();break;
    case 9:
        Key(EKeys::Gamepad_FaceButton_Left,false);if(SmokeTime<.3f)break;SmokeCheck(D->HalloweenStampMask==3,TEXT("Controller interact collects a different porch sticker"));P->ReturnHome();P->TeleportTo(S->Mission->Snacks[1].Position+FVector(0,0,28),FRotator::ZeroRotator);Next();break;
    case 10:
        if(SmokeTime<.4f)break;SmokeCheck(D->SnackMask==1&&!F->Interact(P),TEXT("Active Halloween pauses the indoor patrol and has no indoor doorstep pickup"));S->Pause();S->Selection=7;S->Confirm();SmokeCheck(S->Screen==ECozyScreen::Halloween,TEXT("Pause menu also exposes the seasonal controls"));S->Selection=1;S->Confirm();Next();break;
    case 11:
        if(SmokeTime<.3f)break;SmokeCheck(!F->Active()&&!F->DecorationsVisible()&&D->HalloweenStampMask==3,TEXT("Disabling the event restores normal presentation without losing stamps"));S->Selection=1;S->Confirm();S->Selection=0;S->Confirm();SmokeCheck(F->StampCount()==2&&Neighbourhood->Outside,TEXT("Re-enabling and continuing retains the collection"));SmokeStage=16;SmokeTime=0;break;
    case 22:
        if(SmokeTime<.3f)break;SmokeCheck(F->StampCount()==8&&!D->HalloweenBadge&&!D->HalloweenRoundComplete,TEXT("Eight stickers unlock the return-home objective before the reward"));SmokeCheck(F->Objective().Contains(TEXT("headquarters")),TEXT("Full paw-pass directs the player back to headquarters"));Key(EKeys::H,true);Next();break;
    case 23:
        Key(EKeys::H,false);if(SmokeTime<.5f)break;SmokeCheck(!Neighbourhood->Outside&&!F->Prompt().IsEmpty(),TEXT("Home recovery offers the clubhouse finale indoors too"));Key(EKeys::E,true);Next();break;
    case 24:
        Key(EKeys::E,false);if(SmokeTime<.3f)break;SmokeCheck(D->HalloweenBadge&&D->HalloweenRoundComplete&&S->Message.Contains(TEXT("Leo Ellis")),TEXT("Returning the card earns the permanent Pumpkin Pal badge and cozy family finale"));SmokeCheck(D->SnackMask==1&&D->VisitIndex==0&&!D->RatchetUnlocked,TEXT("Seasonal completion does not complete the main patrol or grant Ratchet early"));
        {const auto* Saved=Cast<UCozyAdventureSave>(UGameplayStatics::LoadGameFromSlot(S->Slot,0));SmokeCheck(Saved&&Saved->HalloweenBadge&&Saved->HalloweenRoundComplete,TEXT("Completed seasonal reward is written to the checkpoint"));}
        Shot(TEXT("FestivalFinale.png"));Next();break;
    case 25:
        if(SmokeTime<.5f)break;D->RatchetUnlocked=true;S->OpenHalloween();S->Selection=3;S->Confirm();SmokeCheck(D->HalloweenStampMask==0&&D->HalloweenBadge&&!D->HalloweenRoundComplete&&D->RatchetUnlocked,TEXT("Explicit replay clears only the stamp round and preserves earned rewards"));
        for(int32 I=0;I<2;++I){P->TeleportTo(F->Porches[I].Position,FRotator::ZeroRotator);S->Interact();}S->OpenHalloween();S->Selection=1;S->Confirm();S->Save();Next();break;
    case 26:if(SmokeTime>.3f)Finish(false);break;
    }
}
