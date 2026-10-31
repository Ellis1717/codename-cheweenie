#include "MartyCharacter.h"
#include "HalloweenCostume.h"
#include "CozyAdventure.h"
#include "CatCompanions.h"
#include "DogLife.h"
#include "Neighbourhood.h"
#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
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

void AMartyPlayMode::TickCostumeSmoke(float Delta)
{
    SmokeTime+=Delta;
    auto* PC=GetWorld()->GetFirstPlayerController();
    auto* P=PC?Cast<AMartyCharacter>(PC->GetPawn()):nullptr;
    if(!P||!Adventure||!Adventure->Progress||!Companions||!Companions->Cats[1].Actor.IsValid())return;
    auto* S=Adventure.Get();auto* D=S->Progress.Get();
    UHalloweenCostume* Outfits[]={P->Costume,Companions->Cats[0].Actor->FindComponentByClass<UHalloweenCostume>(),Companions->Cats[1].Actor->FindComponentByClass<UHalloweenCostume>()};
    FString Root=FPaths::ProjectSavedDir();FParse::Value(FCommandLine::Get(),TEXT("CheweenieTestOutput="),Root);
    auto Next=[&](){++SmokeStage;SmokeTime=0;};
    auto Key=[PC](FKey K,bool Down){PC->InputKey(FInputKeyEventArgs(nullptr,INPUTDEVICEID_NONE,K,Down?IE_Pressed:IE_Released,FPlatformTime::Cycles64()));};
    auto Shot=[&](const TCHAR* Name){if(FApp::CanEverRender())FScreenshotRequest::RequestScreenshot(Root/Name,true,false);};
    auto View=[&](FVector Target){auto* Camera=GetWorld()->SpawnActor<ACameraActor>(Target+FVector(90,-110,55),FRotator::ZeroRotator);Camera->SetActorRotation((Target-Camera->GetActorLocation()).Rotation());Camera->GetCameraComponent()->FieldOfView=42;PC->SetViewTarget(Camera);if(PC->GetHUD())PC->GetHUD()->bShowHUD=false;};
    auto Finish=[&](bool Resume){auto J=MakeShared<FJsonObject>();TArray<TSharedPtr<FJsonValue>> Pass,Fail;for(auto V:SmokePassed)Pass.Add(MakeShared<FJsonValueString>(V));for(auto V:SmokeFailed)Fail.Add(MakeShared<FJsonValueString>(V));J->SetArrayField(TEXT("passed"),Pass);J->SetArrayField(TEXT("failed"),Fail);J->SetStringField(TEXT("scope"),TEXT("Actual game map; synthetic keyboard/gamepad with menu/action fixtures. Physical controller and cloth simulation unverified."));FString Json;FJsonSerializer::Serialize(J,TJsonWriterFactory<>::Create(&Json));const bool Saved=FFileHelper::SaveStringToFile(Json,*(Root/(Resume?TEXT("costumes-resume.json"):TEXT("costumes-smoke.json"))));if(Resume)UGameplayStatics::DeleteGameInSlot(TEXT("CheweenieCostumeAcceptance"),0);FPlatformMisc::RequestExitWithStatus(false,Saved&&SmokeFailed.IsEmpty()?0:1);SmokeStage=1000;};
    if(FParse::Param(FCommandLine::Get(),TEXT("CheweenieCostumeResume")))
    {
        if(SmokeTime>3&&SmokeStage==0){SmokeCheck(D->CostumeMask==5,TEXT("Independent Marty/Rio on and Lakitu off choices survive a second process"));SmokeCheck(Outfits[0]->VisiblePieces()>0&&Outfits[1]->VisiblePieces()==0&&Outfits[2]->VisiblePieces()>0,TEXT("Reload applies only the saved costume choices"));Finish(true);}return;
    }
    switch(SmokeStage)
    {
    case 0:
        if(SmokeTime<3)break;
        SmokeCheck(D->CostumeMask==0,TEXT("New saves default to costumes off"));
        SmokeCheck(Outfits[0]->VisiblePieces()==0&&Outfits[1]->VisiblePieces()==0&&Outfits[2]->VisiblePieces()==0,TEXT("All three pets start without costume geometry"));
        S->Start(false);S->Pause();S->Selection=2;S->Confirm();S->Selection=5;Key(EKeys::Enter,true);Next();break;
    case 1:
        Key(EKeys::Enter,false);if(SmokeTime<.3f)break;
        SmokeCheck(S->Screen==ECozyScreen::Wardrobe&&S->Menu().Num()==4,TEXT("Keyboard opens the three-pet wardrobe from Controls"));
        Key(EKeys::Gamepad_FaceButton_Bottom,true);Next();break;
    case 2:
        Key(EKeys::Gamepad_FaceButton_Bottom,false);if(SmokeTime<.3f)break;
        SmokeCheck(D->CostumeMask==1&&Outfits[0]->VisiblePieces()>0,TEXT("Controller confirm enables Marty's hot-dog outfit independently"));
        Key(EKeys::Gamepad_DPad_Down,true);Next();break;
    case 3:
        Key(EKeys::Gamepad_DPad_Down,false);if(SmokeTime<.3f)break;
        SmokeCheck(S->Selection==1,TEXT("Controller D-pad selects Lakitu's outfit"));Key(EKeys::Enter,true);Next();break;
    case 4:
        Key(EKeys::Enter,false);if(SmokeTime<.3f)break;
        SmokeCheck(D->CostumeMask==3&&Outfits[1]->VisiblePieces()>0,TEXT("Keyboard confirm enables Lakitu's pumpkin outfit"));S->Selection=2;S->Confirm();Next();break;
    case 5:
        if(SmokeTime<.3f)break;
        SmokeCheck(D->CostumeMask==7&&Outfits[2]->VisiblePieces()>0,TEXT("Rio's bee outfit can be enabled alongside both friends"));
        for(int32 I=0;I<3;++I){SmokeCheck(Outfits[I]->CollisionFree(),FString::Printf(TEXT("Pet %d outfit never blocks traversal or navigation"),I));SmokeCheck(Outfits[I]->MaterialsReady(),FString::Printf(TEXT("Pet %d original costume materials and shapes loaded"),I));}
        Shot(TEXT("HalloweenMenu.png"));Next();break;
    case 6:
        if(SmokeTime<2)break;Key(EKeys::Gamepad_FaceButton_Right,true);Next();break;
    case 7:
        Key(EKeys::Gamepad_FaceButton_Right,false);if(SmokeTime<.3f)break;
        SmokeCheck(S->Screen==ECozyScreen::Controls,TEXT("Controller Back returns wardrobe to Controls"));S->Back();S->Back();SmokePosition=P->GetActorLocation();Key(EKeys::W,true);Next();break;
    case 8:
        if(SmokeTime<.7f)break;Key(EKeys::W,false);SmokeCheck(FVector::Dist2D(P->GetActorLocation(),SmokePosition)>20,TEXT("Marty still walks with costume enabled"));P->ReturnHome();SmokeMoving=false;Next();break;
    case 9:
        if(!SmokeMoving&&SmokeTime>1){View(P->GetActorLocation()+FVector(-4,0,3));SmokeMoving=true;}
        if(SmokeTime<6)break;Shot(TEXT("HalloweenMarty.png"));Next();break;
    case 10:
        if(SmokeTime<1)break;View(Companions->Position(0)+FVector(0,0,26));Next();break;
    case 11:
        if(SmokeTime<4)break;Shot(TEXT("HalloweenLakitu.png"));Next();break;
    case 12:
        if(SmokeTime<1)break;View(Companions->Position(1)+FVector(0,0,26));Next();break;
    case 13:
        if(SmokeTime<4)break;Shot(TEXT("HalloweenRio.png"));Next();break;
    case 14:
        if(SmokeTime<1)break;PC->SetViewTarget(P);if(PC->GetHUD())PC->GetHUD()->bShowHUD=true;
        Neighbourhood->GoOutside(P);SmokeCheck(Neighbourhood->Outside&&Outfits[0]->VisiblePieces()>0,TEXT("Costume stays available through outdoor door travel"));P->ReturnHome();
        S->Pause();S->Selection=2;S->Confirm();S->Selection=5;S->Confirm();S->Selection=1;S->Confirm();Next();break;
    case 15:
        if(SmokeTime<.3f)break;SmokeCheck(D->CostumeMask==5&&Outfits[1]->VisiblePieces()==0&&Outfits[0]->VisiblePieces()>0&&Outfits[2]->VisiblePieces()>0,TEXT("Lakitu outfit turns off without removing Marty or Rio outfits"));S->ToggleFeature(ECozyFeature::Companions);SmokeCheck(Outfits[2]->VisiblePieces()>0,TEXT("Costumes are independent of companion behaviour setting"));S->Save();Finish(false);break;
    }
}
