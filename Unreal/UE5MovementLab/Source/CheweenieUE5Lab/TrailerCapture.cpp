// Opt-in real gameplay recording. HUD hidden; isolated save and scripted camera.
#include "MartyCharacter.h"
#include "CozyAdventure.h"
#include "CatCompanions.h"
#include "Neighbourhood.h"
#include "DogLife.h"
#include "AnimalGait.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Misc/App.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "UnrealClient.h"
#include "EngineUtils.h"
#include "Engine/StaticMeshActor.h"
#include "Components/TextRenderComponent.h"

void AMartyPlayMode::TickTrailerCapture(float Delta)
{
#if !UE_BUILD_SHIPPING
    SmokeTime+=Delta;
    auto* PC=GetWorld()->GetFirstPlayerController();auto* P=PC?Cast<AMartyCharacter>(PC->GetPawn()):nullptr;
    if(!P||!Adventure||!Adventure->Progress||!Companions||!Neighbourhood||!P->Life)return;
    if(FParse::Param(FCommandLine::Get(),TEXT("CheweenieAnimalMotion")))
    {
        static TWeakObjectPtr<ACameraActor> View;
        static int32 Frame=0,Warm=24;
        static FString Previous;
        const FString Folder=FPaths::ProjectSavedDir()/TEXT("AnimalMotion");
        if(!View.IsValid())
        {
            if(SmokeTime<12)return;
            IFileManager::Get().MakeDirectory(*Folder,true);Adventure->Start(true);Neighbourhood->GoOutside(P);
            auto& Dog=Neighbourhood->Dogs[0];
            P->GetCharacterMovement()->StopMovementImmediately();
            const bool Placed=P->TeleportTo(Dog.Home+FVector(-100,0,45),FRotator(0,180,0));
            Adventure->Progress->DisabledFeatures&=~(1<<int32(ECozyFeature::Companions));Companions->CallBoth();Dog.Activity=ENeighbourActivity::Follow;
            UE_LOG(LogTemp,Display,TEXT("ANIMAL_CAPTURE staged=%d outside=%d catsEnabled=%d catsOutside=%d/%d player=%s dog=%s cats=%s/%s"),Placed,Neighbourhood->Outside,Companions->Enabled(),Companions->Outdoors(0),Companions->Outdoors(1),*P->GetActorLocation().ToString(),*Dog.Position.ToString(),*Companions->Position(0).ToString(),*Companions->Position(1).ToString());
            if(!Placed||!Companions->Outdoors(0)||!Companions->Outdoors(1)){FApp::SetUseFixedTimeStep(false);FPlatformMisc::RequestExitWithStatus(false,1);return;}
            if(PC->GetHUD())PC->GetHUD()->bShowHUD=false;
            Adventure->Progress->DisabledFeatures|=1<<int32(ECozyFeature::WorldLabels);
            View=GetWorld()->SpawnActor<ACameraActor>();View->GetCameraComponent()->SetFieldOfView(55);
            FApp::SetUseFixedTimeStep(true);FApp::SetFixedDeltaTime(1.0/24.0);
        }
        if(!Previous.IsEmpty()&&!IFileManager::Get().FileExists(*Previous))return;
        if(Frame>=120)
        {
            FApp::SetUseFixedTimeStep(false);
            FFileHelper::SaveStringToFile(TEXT("120 frames / 24 fps. Real-map dog and feline IK, scripted movement/camera, HUD hidden, isolated acceptance save. Physical controllers unverified."),*(Folder/TEXT("capture-complete.txt")));
            FPlatformMisc::RequestExitWithStatus(false,0);return;
        }
        const FVector Centre=(P->GetActorLocation()+Neighbourhood->Dogs[0].Position+Companions->Position(0)+Companions->Position(1))*.25f+FVector(0,0,35),Eye=Centre+FVector(20,500,190);
        View->SetActorLocationAndRotation(Eye,(Centre-Eye).Rotation());PC->SetViewTarget(View.Get());
        if(Warm>0){--Warm;return;}
        if(Frame<100)P->AddMovementInput(FVector(-1,0,0),.65f);
        if(Frame==60)UE_LOG(LogTemp,Display,TEXT("ANIMAL_CAPTURE midpoint player=%s dog=%s cats=%s/%s"),*P->GetActorLocation().ToString(),*Neighbourhood->Dogs[0].Position.ToString(),*Companions->Position(0).ToString(),*Companions->Position(1).ToString());
        Previous=Folder/FString::Printf(TEXT("frame_%04d.png"),Frame);FScreenshotRequest::RequestScreenshot(Previous,false,false);++Frame;return;
    }
    static TWeakObjectPtr<ACameraActor> Camera;
    static int32 Shot=-1,Frame=0,Warm=0;
    static FString Previous;
    const FString Root=FPaths::ProjectSavedDir()/TEXT("TrailerCapture");
    constexpr int32 FramesPerShot=108;
    if(Shot<0)
    {
        if(SmokeTime<25)return;
        IFileManager::Get().MakeDirectory(*Root,true);
        Adventure->Start(true);Adventure->Progress->DisabledFeatures=1<<int32(ECozyFeature::ActionViews);
        Adventure->Progress->CostumeMask=Adventure->Progress->FantasyCostumeMask=0;
        Adventure->Progress->PetEnergy=100;
        PC->GetHUD()->bShowHUD=false;PC->bShowMouseCursor=false;
        for(TActorIterator<AActor> It(GetWorld());It;++It){TArray<UTextRenderComponent*> Labels;It->GetComponents(Labels);for(auto* Label:Labels)Label->SetVisibility(false);}
        Camera=GetWorld()->SpawnActor<ACameraActor>();Camera->GetCameraComponent()->SetFieldOfView(65);
        Camera->GetCameraComponent()->PostProcessSettings.bOverride_AutoExposureBias=true;
        Camera->GetCameraComponent()->PostProcessSettings.AutoExposureBias=.8f;
        PC->SetViewTarget(Camera.Get());
        FApp::SetUseFixedTimeStep(true);FApp::SetFixedDeltaTime(1.0/24.0);
        Shot=0;Warm=48;
        P->TeleportTo(FVector(0,-180,35),FRotator::ZeroRotator);
        Companions->CallBoth();
    }
    if(!Previous.IsEmpty()&&!IFileManager::Get().FileExists(*Previous))return;
    if(Frame>=FramesPerShot)
    {
        ++Shot;Frame=0;Warm=48;P->Life->CancelAction();P->GetCharacterMovement()->StopMovementImmediately();
        if(Shot==4)
        {
            FApp::SetUseFixedTimeStep(false);
            FFileHelper::SaveStringToFile(TEXT("432 frames at 24fps. Four 4.5-second real native gameplay shots: companion head bumps, indoor following, tricks, outdoor following. Isolated save. HUD hidden; preferences unchanged. Scripted camera and staging."),*(Root/TEXT("capture-complete.txt")));
            FPlatformMisc::RequestExitWithStatus(false,0);return;
        }
        P->TeleportTo(FVector(0,-180,35),FRotator::ZeroRotator);
        if(Shot==3)Neighbourhood->GoOutside(P);
        Companions->CallBoth();
    }
    const FVector Center=P->GetActorLocation();
    const float Orbit=Shot==0?float(Frame)*.08f:Shot==2?-12.f+float(Frame)*.13f:0.f;
    const FVector Offset=FRotator(0,Orbit,0).RotateVector(Shot==2?FVector(190,70,105):FVector(215,80,135));
    const FVector Focus=Center+FVector(0,0,14);FVector View=Center+Offset;
    FHitResult Hit;FCollisionQueryParams Query(SCENE_QUERY_STAT(TrailerCamera),false,P);
    for(const auto& Cat:Companions->Cats)if(Cat.Actor.IsValid())Query.AddIgnoredActor(Cat.Actor.Get());
    if(GetWorld()->LineTraceSingleByChannel(Hit,Focus,View,ECC_Camera,Query)&&FVector::Dist(Focus,Hit.Location)>240)View=Hit.Location+(Focus-Hit.Location).GetSafeNormal()*18;
    if(Shot<3){View.X=FMath::Clamp(View.X,-340.0,340.0);View.Y=FMath::Clamp(View.Y,-340.0,340.0);}
    Camera->SetActorLocation(View);
    Camera->SetActorRotation((Focus-View).Rotation());
    PC->SetViewTarget(Camera.Get());
    if(Warm>0){--Warm;return;}
    if(Shot==0){if(Frame==12)Companions->Pet(0);if(Frame==58)Companions->Pet(1);}
    if(Shot==1&&Frame<36)P->AddMovementInput(FVector(0,-1,0),.35f);
    if(Shot==3&&Frame<72)P->AddMovementInput(FVector(1,0,0),.55f);
    if(Shot==2){if(Frame==10)P->Life->RollOver();if(Frame==65)P->Life->ShakePaw();}
    Previous=Root/FString::Printf(TEXT("shot%d_%04d.png"),Shot,Frame);
    FScreenshotRequest::RequestScreenshot(Previous,false,false);
    ++Frame;
#endif
}
