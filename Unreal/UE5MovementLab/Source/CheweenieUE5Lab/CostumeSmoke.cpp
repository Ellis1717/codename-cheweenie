#include "MartyCharacter.h"
#include "HalloweenCostume.h"
#include "CozyAdventure.h"
#include "CatCompanions.h"
#include "DogLife.h"
#include "Neighbourhood.h"
#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PoseableMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"
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

// Check visible garment geometry against anatomy, independently of attachment math.
bool CostumeAlignedWithMarty(const AMartyCharacter* Pawn)
{
    if(!Pawn||!Pawn->Skin)return false;
    auto* Skin=Pawn->Skin.Get();
    const FVector Front=(Skin->GetBoneTransformByName(TEXT("front_l_upper"),EBoneSpaces::WorldSpace).GetLocation()+Skin->GetBoneTransformByName(TEXT("front_r_upper"),EBoneSpaces::WorldSpace).GetLocation())*.5f;
    const FVector Hips=(Skin->GetBoneTransformByName(TEXT("hind_l_upper"),EBoneSpaces::WorldSpace).GetLocation()+Skin->GetBoneTransformByName(TEXT("hind_r_upper"),EBoneSpaces::WorldSpace).GetLocation())*.5f;
    const FVector Along=(Front-Hips).GetSafeNormal();
    const double Head=FVector::DotProduct(Skin->GetBoneTransformByName(TEXT("head"),EBoneSpaces::WorldSpace).GetLocation()-Hips,Along);
    const double Tail=FVector::DotProduct(Skin->GetBoneTransformByName(TEXT("tail"),EBoneSpaces::WorldSpace).GetLocation()-Hips,Along);
    TArray<UStaticMeshComponent*> Parts;Pawn->GetComponents(Parts);int32 Count=0;
    for(const auto* C:Parts)
    {
        const auto* Material=C->GetMaterial(0);if(!Material)continue;
        const FString Path=Material->GetPathName();
        if(Path!=TEXT("/Game/Adventure/Halloween/Bun.Bun")&&Path!=TEXT("/Game/Adventure/Halloween/Red.Red"))continue;
        ++Count;if(!C->IsVisible()||!C->GetStaticMesh()||FVector::DotProduct(C->GetForwardVector(),Along)<.98)return false;
        const double Center=FVector::DotProduct(C->GetComponentLocation()-Hips,Along);
        const double Half=C->GetStaticMesh()->GetBounds().BoxExtent.X*C->GetComponentScale().X;
        if(Center-Half<Tail-4||Center+Half>Head+2)return false;
    }
    return Count==3;
}

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
    auto BodyView=[&](bool Top){const FTransform Mesh=P->Skin->GetComponentTransform();const FVector Target=Mesh.TransformPosition(FVector(-3,0,25));auto* Camera=GetWorld()->SpawnActor<ACameraActor>(Target+Mesh.TransformVectorNoScale(Top?FVector(0,0,190):FVector(5,-150,38)),FRotator::ZeroRotator);Camera->SetActorRotation(Top?FRotator(-90,Mesh.Rotator().Yaw,0):(Target-Camera->GetActorLocation()).Rotation());Camera->GetCameraComponent()->FieldOfView=Top?50:42;PC->SetViewTarget(Camera);if(PC->GetHUD())PC->GetHUD()->bShowHUD=false;};
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
        if(SmokeTime<.7f)break;Key(EKeys::W,false);SmokeCheck(FVector::Dist2D(P->GetActorLocation(),SmokePosition)>20,TEXT("Marty still walks with costume enabled"));SmokeCheck(CostumeAlignedWithMarty(P),TEXT("Both buns and sausage stay lengthwise while Marty walks"));P->ReturnHome();SmokeMoving=false;Next();break;
    case 9:
        if(!SmokeMoving&&SmokeTime>1){View(P->GetActorLocation()+FVector(-4,0,3));SmokeMoving=true;}
        if(SmokeTime<6)break;SmokeCheck(CostumeAlignedWithMarty(P),TEXT("Hot dog runs hips to shoulders with head and tail clearance at rest"));Shot(TEXT("HalloweenMarty.png"));SmokeStage=20;SmokeTime=0;break;
    case 20:
        if(SmokeTime<.4f)break;BodyView(false);Next();break;
    case 21:
        if(SmokeTime<1.5f)break;Shot(TEXT("HalloweenMartySide.png"));Next();break;
    case 22:
        if(SmokeTime<.4f)break;P->SetActorRotation(FRotator(0,P->GetActorRotation().Yaw+90,0));Next();break;
    case 23:
        if(SmokeTime<.4f)break;SmokeCheck(CostumeAlignedWithMarty(P),TEXT("Hot dog keeps its front-to-back fit after turning"));BodyView(true);Next();break;
    case 24:
        if(SmokeTime<1)break;Shot(TEXT("HalloweenMartyTop.png"));Next();break;
    case 25:
        if(SmokeTime<.4f)break;PC->SetViewTarget(P);P->Life->LieDown();Next();break;
    case 26:
        if(SmokeTime<1)break;SmokeCheck(P->Life->Activity==EDogActivity::Lying&&CostumeAlignedWithMarty(P),TEXT("Hot dog stays lengthwise and fitted when Marty lies down"));BodyView(false);Next();break;
    case 27:
        if(SmokeTime<1)break;Shot(TEXT("HalloweenMartyLying.png"));SmokeStage=33;SmokeTime=0;break;
    case 33:
        if(SmokeTime<.4f)break;P->Life->StandUp();SmokeStage=28;SmokeTime=0;break;
    case 28:
        if(SmokeTime<.4f)break;SmokeCheck(CostumeAlignedWithMarty(P),TEXT("Hot dog returns to its fitted standing pose after getting up"));P->Life->RollOver();BodyView(false);Next();break;
    case 29:
        if(SmokeTime<.8f)break;SmokeCheck(P->Life->Activity==EDogActivity::Rolling&&CostumeAlignedWithMarty(P),TEXT("Hot dog follows the rolling torso without rotating across its back"));Shot(TEXT("HalloweenMartyRolling.png"));Next();break;
    case 30:
        if(SmokeTime<2)break;SmokeCheck(P->Life->Activity==EDogActivity::Idle&&CostumeAlignedWithMarty(P),TEXT("Hot dog recovers its fit after the roll"));S->ToggleFeature(ECozyFeature::DetailedCoat);Next();break;
    case 31:
        if(SmokeTime<.4f)break;SmokeCheck(CostumeAlignedWithMarty(P),TEXT("Lengthwise fit also works with the smooth Marty mesh"));S->ToggleFeature(ECozyFeature::DetailedCoat);Next();break;
    case 32:
        if(SmokeTime<.4f)break;SmokeCheck(CostumeAlignedWithMarty(P),TEXT("Lengthwise fit survives switching back to the detailed coat"));PC->SetViewTarget(P);if(PC->GetHUD())PC->GetHUD()->bShowHUD=true;SmokeStage=10;SmokeTime=0;break;
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
