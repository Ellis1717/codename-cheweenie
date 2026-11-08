#include "MartyCharacter.h"
#include "DogJackets.h"
#include "HalloweenCostume.h"
#include "CozyAdventure.h"
#include "DogLife.h"
#include "Components/PoseableMeshComponent.h"
#include "Engine/SkinnedAsset.h"
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
#include "Misc/App.h"
#include "UnrealClient.h"

static bool JacketAligned(const AMartyCharacter* P)
{
    auto* Skin=P->Skin.Get();const FVector Front=(Skin->GetBoneTransformByName(TEXT("front_l_upper"),EBoneSpaces::WorldSpace).GetLocation()+Skin->GetBoneTransformByName(TEXT("front_r_upper"),EBoneSpaces::WorldSpace).GetLocation())*.5;
    const FVector Hips=(Skin->GetBoneTransformByName(TEXT("hind_l_upper"),EBoneSpaces::WorldSpace).GetLocation()+Skin->GetBoneTransformByName(TEXT("hind_r_upper"),EBoneSpaces::WorldSpace).GetLocation())*.5;
    const auto T=P->Jackets->JacketTransform();return FVector::DotProduct(T.GetRotation().GetAxisX(),(Front-Hips).GetSafeNormal())>.97&&FVector::Dist(T.GetLocation(),Skin->GetBoneTransformByName(TEXT("spine"),EBoneSpaces::WorldSpace).GetLocation())<30;
}
void AMartyPlayMode::TickJacketSmoke(float Delta)
{
    SmokeTime+=Delta;auto* PC=GetWorld()->GetFirstPlayerController();auto* P=PC?Cast<AMartyCharacter>(PC->GetPawn()):nullptr;
    auto* S=Adventure.Get();if(!P||!P->Jackets||!S||!S->Progress)return;auto* D=S->Progress.Get();auto* J=P->Jackets.Get();
    FString Root=FPaths::ProjectSavedDir();FParse::Value(FCommandLine::Get(),TEXT("CheweenieTestOutput="),Root);
    auto Next=[&](){++SmokeStage;SmokeTime=0;};
    auto Key=[&](FKey K,bool Down){PC->InputKey(FInputKeyEventArgs(nullptr,INPUTDEVICEID_NONE,K,Down?IE_Pressed:IE_Released,FPlatformTime::Cycles64()));};
    auto Shot=[&](const TCHAR* Name){if(FApp::CanEverRender())FScreenshotRequest::RequestScreenshot(Root/Name,true,false);};
    auto Wardrobe=[&](){PC->SetViewTarget(P);if(PC->GetHUD())PC->GetHUD()->bShowHUD=true;S->Pause();S->Selection=2;S->Confirm();S->Selection=5;S->Confirm();S->Selection=6;};
    auto Resume=[&](){S->Back();S->Back();S->Back();};
    auto View=[&](){const auto Mesh=P->Skin->GetComponentTransform();const FVector Target=Mesh.TransformPosition(FVector(-3,0,25));auto* Camera=GetWorld()->SpawnActor<ACameraActor>(Target+Mesh.TransformVectorNoScale(FVector(8,-150,38)),FRotator::ZeroRotator);Camera->SetActorRotation((Target-Camera->GetActorLocation()).Rotation());Camera->GetCameraComponent()->FieldOfView=42;PC->SetViewTarget(Camera);if(PC->GetHUD())PC->GetHUD()->bShowHUD=false;};
    const bool Reload=FParse::Param(FCommandLine::Get(),TEXT("CheweenieJacketResume"));
    auto Finish=[&]()
    {
        auto Report=MakeShared<FJsonObject>();auto Strings=[](const TArray<FString>& Values){TArray<TSharedPtr<FJsonValue>> R;for(const auto& V:Values)R.Add(MakeShared<FJsonValueString>(V));return R;};
        Report->SetArrayField(TEXT("passed"),Strings(SmokePassed));Report->SetArrayField(TEXT("failed"),Strings(SmokeFailed));Report->SetStringField(TEXT("scope"),TEXT("Actual map; synthetic keyboard/gamepad/mouse hitboxes, menu/save/camera/action fixtures. Physical hardware and cloth simulation unverified."));
        FString Json;FJsonSerializer::Serialize(Report,TJsonWriterFactory<>::Create(&Json));const bool Saved=FFileHelper::SaveStringToFile(Json,*(Root/(Reload?TEXT("jackets-resume.json"):TEXT("jackets-smoke.json"))));SmokeStage=999;FPlatformMisc::RequestExitWithStatus(false,Saved&&SmokeFailed.IsEmpty()?0:1);
    };
    if(SmokeStage==999)return;
    PC->InputKey(FInputKeyEventArgs(nullptr,INPUTDEVICEID_NONE,EKeys::Gamepad_LeftY,SmokeStage==15?.8f:0.f,Delta,1,FPlatformTime::Cycles64()));
    if(Reload){if(SmokeTime>3){SmokeCheck(D->MartyJacket==4&&D->CostumeMask==6,TEXT("Jacket and independent cat choices survive another process"));SmokeCheck(J->Style==4&&J->VisiblePieces()>0&&P->Costume->VisiblePieces()==0,TEXT("Reload restores only the selected Marty jacket"));SmokeCheck(J->CollisionFree()&&J->MaterialsReady()&&JacketAligned(P),TEXT("Reloaded jacket remains fitted, material-backed and collision-free"));Finish();}return;}
    if(SmokeTime<.4)return;
    switch(SmokeStage)
    {
    case 0:
        if(SmokeTime<(FApp::CanEverRender()?25:3))break;
        SmokeCheck(S->Slot==TEXT("CheweenieJacketAcceptance"),TEXT("Jacket checks use an isolated save"));SmokeCheck(D->MartyJacket==0&&J->VisiblePieces()==0,TEXT("New profiles default to no jacket"));
        S->Start(true);D->CostumeMask=6;Wardrobe();SmokeCheck(S->Screen==ECozyScreen::Wardrobe&&S->Menu().Num()==8,TEXT("Wardrobe keeps three costumes and adds a jacket selector"));Key(EKeys::Right,true);Next();break;
    case 1:
        Key(EKeys::Right,false);SmokeCheck(D->MartyJacket==1&&D->CostumeMask==6,TEXT("Keyboard selects yellow raincoat without changing cat costumes"));SmokeCheck(J->VisiblePieces()>0&&J->CollisionFree()&&J->MaterialsReady(),TEXT("Raincoat has visible original geometry and valid non-colliding materials"));SmokeCheck(J->SurfaceFacesOutward(),TEXT("Garment surfaces use outward normals and Unreal clockwise front faces"));Resume();View();Next();break;
    case 2:
        if(SmokeTime<1)break;SmokeCheck(JacketAligned(P),TEXT("Yellow coat follows the actual body axis"));Shot(TEXT("JacketYellow.png"));SmokeStage=102;SmokeTime=0;break;
    case 102:S->AdjustMartyJacket(1);SmokeStage=3;SmokeTime=0;break;
    case 3:
        if(SmokeTime<1)break;SmokeCheck(J->Style==2&&JacketAligned(P)&&J->CollisionFree(),TEXT("Red fleece switches live and stays fitted"));Shot(TEXT("JacketFleece.png"));SmokeStage=103;SmokeTime=0;break;
    case 103:Wardrobe();SmokeStage=4;SmokeTime=0;break;
    case 4:Key(EKeys::Gamepad_DPad_Right,true);Next();break;
    case 5:
        Key(EKeys::Gamepad_DPad_Right,false);SmokeCheck(D->MartyJacket==3,TEXT("Gamepad D-pad selects the navy winter coat"));Resume();View();Next();break;
    case 6:
        if(SmokeTime<1)break;SmokeCheck(J->Style==3&&JacketAligned(P)&&J->MaterialsReady(),TEXT("Navy winter coat has a fitted collar and loaded materials"));Shot(TEXT("JacketNavy.png"));SmokeStage=106;SmokeTime=0;break;
    case 106:Wardrobe();SmokeStage=7;SmokeTime=0;break;
    case 7:
        CastChecked<AMartyHUD>(PC->GetHUD())->NotifyHitBoxClick(TEXT("CozyPlus_6"));SmokeCheck(D->MartyJacket==4,TEXT("Mouse plus hitbox selects pink quilted coat"));Resume();View();Next();break;
    case 8:
        if(SmokeTime<1)break;SmokeCheck(J->Style==4&&JacketAligned(P)&&J->CollisionFree(),TEXT("Pink quilted coat stays aligned without blocking movement"));Shot(TEXT("JacketPink.png"));SmokeStage=108;SmokeTime=0;break;
    case 108:D->FurDetailPreference=1;SmokeStage=1008;SmokeTime=0;break;
    case 1008:
        if(SmokeTime<1)break;SmokeCheck(P->Skin->GetSkinnedAsset()->GetPathName().Contains(TEXT("MartyPhotoReference/"))&&JacketAligned(P),TEXT("Jacket attachment supports the actual Medium fur model"));D->DisabledFeatures|=1<<uint8(ECozyFeature::DetailedCoat);SmokeStage=9;SmokeTime=0;break;
    case 9:
        if(SmokeTime<1)break;SmokeCheck(P->Skin->GetSkinnedAsset()->GetPathName().Contains(TEXT("MartyPhotoSmooth/"))&&JacketAligned(P),TEXT("Jacket attachment supports the actual smooth Low fur model"));Shot(TEXT("JacketPinkLow.png"));SmokeStage=109;SmokeTime=0;break;
    case 109:D->DisabledFeatures&=~(1<<uint8(ECozyFeature::DetailedCoat));D->FurDetailPreference=2;P->Life->LieDown();SmokeStage=10;SmokeTime=0;break;
    case 10:
        if(SmokeTime<1)break;SmokeCheck(P->Skin->GetSkinnedAsset()->GetPathName().Contains(TEXT("MartyVisibleStrands/"))&&P->Life->Activity==EDogActivity::Lying&&JacketAligned(P),TEXT("Actual High-fur jacket follows the lying pose"));View();Shot(TEXT("JacketLying.png"));Next();break;
    case 11:P->Life->StandUp();SmokeStage=110;SmokeTime=0;break;
    case 110:P->Life->RollOver();SmokeStage=12;SmokeTime=0;break;
    case 12:
        if(SmokeTime<.6)break;SmokeCheck(P->Life->Activity==EDogActivity::Rolling&&JacketAligned(P),TEXT("Jacket follows rolling without changing trick state"));Shot(TEXT("JacketRolling.png"));Next();break;
    case 13:
        if(SmokeTime<2.5)break;Wardrobe();S->Selection=0;S->Confirm();Next();break;
    case 14:
        SmokeCheck(D->MartyJacket==0&&J->VisiblePieces()==0&&P->Costume->VisiblePieces()>0,TEXT("Existing hot-dog costume removes the jacket instead of stacking"));S->Selection=6;S->Adjust(-1);SmokeCheck(D->MartyJacket==4&&(D->CostumeMask&1)==0,TEXT("Backward cycling wraps to pink and removes the hot-dog costume"));Resume();PC->SetViewTarget(P);if(PC->GetHUD())PC->GetHUD()->bShowHUD=true;SmokePosition=P->GetActorLocation();Next();break;
    case 15:
        if(SmokeTime<.6)break;SmokeCheck(FVector::Dist2D(P->GetActorLocation(),SmokePosition)>10,TEXT("Synthetic gamepad movement works while wearing the jacket"));Key(EKeys::W,true);SmokePosition=P->GetActorLocation();Next();break;
    case 16:
        if(SmokeTime<.6)break;Key(EKeys::W,false);SmokeCheck(FVector::Dist2D(P->GetActorLocation(),SmokePosition)>10,TEXT("Keyboard movement works while wearing the jacket"));Key(EKeys::SpaceBar,true);Next();break;
    case 17:
        Key(EKeys::SpaceBar,false);SmokeCheck(P->GetCharacterMovement()->IsFalling()&&J->CollisionFree(),TEXT("Jacket leaves keyboard jumping and character collision active"));Next();break;
    case 18:
        if(SmokeTime<1.2)break;D->MartyJacket=4;D->CostumeMask=6;S->Save();Finish();break;
    }
}
