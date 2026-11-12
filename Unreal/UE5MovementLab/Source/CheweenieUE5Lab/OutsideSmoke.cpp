#include "MartyCharacter.h"
#include "Neighbourhood.h"
#include "AnimalGait.h"
#include "Components/PoseableMeshComponent.h"
#include "CozyAdventure.h"
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
#include "Components/TextRenderComponent.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/HUD.h"
void AMartyPlayMode::TickOutsideSmoke(float Delta)
{
    SmokeTime+=Delta;auto* PC=GetWorld()->GetFirstPlayerController();auto* P=PC?Cast<AMartyCharacter>(PC->GetPawn()):nullptr;
    if(!P||!Adventure||!Adventure->Progress||!Neighbourhood)return;auto* N=Neighbourhood.Get();
    FString Root=FPaths::ProjectSavedDir();FParse::Value(FCommandLine::Get(),TEXT("CheweenieTestOutput="),Root);
    auto Key=[PC](FKey K,bool Down){PC->InputKey(FInputKeyEventArgs(nullptr,INPUTDEVICEID_NONE,K,Down?IE_Pressed:IE_Released,FPlatformTime::Cycles64()));};
    auto Next=[this](){++SmokeStage;SmokeTime=0;};
    auto Shot=[&](const TCHAR* Name){if(FApp::CanEverRender())FScreenshotRequest::RequestScreenshot(Root/Name,true,false);};
    if(FParse::Param(FCommandLine::Get(),TEXT("CheweenieDogSmoke")))
    {
        auto& D=N->Dogs[0];
        static TWeakObjectPtr<ACameraActor> DogCamera;
        static FTransform PriorKnee,PriorSpine;
        static float PriorCycle=0;
        auto FrameDog=[&](const FNeighbourDog& Dog)
        {
            if(!FApp::CanEverRender())return;
            if(!DogCamera.IsValid())DogCamera=GetWorld()->SpawnActor<ACameraActor>();
            const FVector View=Dog.Position+FVector(-300,-310,205),Focus=Dog.Position+FVector(-10,0,95);
            DogCamera->SetActorLocationAndRotation(View,(Focus-View).Rotation());DogCamera->GetCameraComponent()->SetFieldOfView(55);
            PC->SetViewTarget(DogCamera.Get());if(PC->GetHUD())PC->GetHUD()->bShowHUD=false;
        };
        auto NearDog=[&](){P->GetCharacterMovement()->StopMovementImmediately();P->TeleportTo(D.Position+FVector(-100,0,40),FRotator::ZeroRotator);};
        switch(SmokeStage)
        {
        case 0: if(SmokeTime>(FApp::CanEverRender()?40:3))
            {
                Adventure->Start(true);N->GoOutside(P);
                SmokeCheck(D.Name==TEXT("Bauer")&&N->Dogs[1].Name==TEXT("Hank")&&N->Dogs[2].Name==TEXT("Georgia")&&N->Dogs[3].Name==TEXT("Pieper"),TEXT("Owner roster defaults preserve all four names"));
                SmokeCheck(N->Dogs[1].Body->GetActorScale3D().X>D.Body->GetActorScale3D().X&&D.Gait.IsValid()&&D.Gait->Ready&&D.Gait->LegStates().Num()==4,TEXT("Distinct skinned dog sizes and four articulated limb chains exist on the real map"));
                SmokeCheck(D.Bond==0&&Adventure->Progress->NeighbourDogBonds.IsEmpty(),TEXT("Legacy save with no bond array starts safely at zero"));
                SmokePosition=D.Position;P->TeleportTo(D.Home+FVector(-700,0,40),FRotator::ZeroRotator);Next();
            }break;
        case 1: if(SmokeTime>9)
            {
                SmokeCheck(FVector::Dist2D(D.Position,SmokePosition)>35&&FVector::Dist2D(D.Position,D.Home)<300,TEXT("Autonomous roaming moves the whole dog within its home area"));
                SmokeCheck(FMath::Abs(D.Position.Z-D.Home.Z)<5,TEXT("Roaming keeps paws on exterior ground"));
                NearDog();Next();
            }break;
        case 2: if(SmokeTime>.5){SmokeCheck(N->Prompt(P).Contains(TEXT("say hello to Bauer")),TEXT("Nearby dog stops for an accessible greeting prompt"));Key(EKeys::E,true);Next();}break;
        case 3: if(SmokeTime>.2)
            {
                Key(EKeys::E,false);SmokeCheck(D.Met&&D.Visit==1&&N->MetDogs==1&&(Adventure->Progress->NeighbourDogMask&1),TEXT("Keyboard greeting saves existing meeting slot without duplicate friendship"));
                SmokeCheck(N->Prompt(P).Contains(TEXT("play bow")),TEXT("Greeting advances to an explicit play interaction"));
                N->Interact(P);SmokeCheck(D.Visit==1&&D.Bond==0,TEXT("Interaction cooldown prevents held input from skipping a stage"));
                SmokePosition=D.Position;PriorCycle=D.Gait->Cycle;PriorKnee=D.Gait->Skin->GetBoneTransformByName(TEXT("front_l_lower"),EBoneSpaces::ComponentSpace);Adventure->Pause();Next();
            }break;
        case 4: if(SmokeTime>1)
            {
                SmokeCheck(D.Position.Equals(SmokePosition,.01)&&D.Cooldown>0,TEXT("Pause freezes NPC movement and interaction timers"));
                SmokeCheck(!D.Label->IsVisible(),TEXT("NPC labels hide behind menus"));SmokeCheck(D.Gait->Cycle==PriorCycle&&D.Gait->Skin->GetBoneTransformByName(TEXT("front_l_lower"),EBoneSpaces::ComponentSpace).Equals(PriorKnee,.001),TEXT("Pause freezes the skinned leg pose and gait cycle"));Adventure->Pause();Next();
            }break;
        case 5: if(SmokeTime>1.5){Key(EKeys::Gamepad_FaceButton_Left,true);Next();}break;
        case 6: if(SmokeTime>.2)
            {
                Key(EKeys::Gamepad_FaceButton_Left,false);
                SmokeCheck(D.Activity==ENeighbourActivity::Play&&D.Bond==1&&D.Visit==2,TEXT("Synthetic controller interaction begins a playful chase and raises bond"));
                SmokeCheck(Adventure->Progress->NeighbourDogBonds.Num()==8&&Adventure->Progress->NeighbourDogBonds[0]==1,TEXT("Friendship serializes into a compatible appended save array"));
                SmokePosition=D.Position;PriorKnee=D.Gait->Skin->GetBoneTransformByName(TEXT("front_l_lower"),EBoneSpaces::ComponentSpace);PriorSpine=D.Gait->Skin->GetBoneTransformByName(TEXT("chest"),EBoneSpaces::ComponentSpace);Next();
            }break;
        case 7: if(SmokeTime>2)
            {
                SmokeCheck(FVector::Dist2D(D.Position,SmokePosition)>30,TEXT("Play state produces real movement rather than only dialogue"));
                SmokeCheck(!D.Gait->Skin->GetBoneTransformByName(TEXT("front_l_lower"),EBoneSpaces::ComponentSpace).Equals(PriorKnee,.001)&&D.Gait->MaxJointBend>15,TEXT("Moving dog bends a real weighted elbow/knee rather than swinging a rigid leg"));
                SmokeCheck(!D.Gait->Skin->GetBoneTransformByName(TEXT("chest"),EBoneSpaces::ComponentSpace).Equals(PriorSpine,.001),TEXT("Dog torso and shoulders change pose during locomotion"));
                bool Finite=true;for(const auto& L:D.Gait->LegStates())for(FName B:{L.Upper,L.Lower,L.Hock,L.Paw})Finite&=!D.Gait->Skin->GetBoneTransformByName(B,EBoneSpaces::ComponentSpace).ContainsNaN();
                SmokeCheck(Finite&&D.Gait->PlantedPaws>=2,TEXT("All four limb chains remain finite with supporting paws on the ground"));
                SmokeCheck(D.Gait.IsValid()&&D.Gait->Ready&&!D.Gait->Skin->GetBoneTransformByName(TEXT("tail_tip"),EBoneSpaces::ComponentSpace).ContainsNaN(),TEXT("Weighted tail chain has a finite live pose in the real world"));
                NearDog();Next();
            }break;
        case 8: if(SmokeTime>.4){Key(EKeys::E,true);Next();}break;
        case 9: if(SmokeTime>.2)
            {
                Key(EKeys::E,false);SmokeCheck(D.Activity==ENeighbourActivity::Follow&&N->Prompt(P).Contains(TEXT("say goodbye")),TEXT("Keyboard invitation enables walking buddy and clear dismissal prompt"));
                SmokePosition=D.Position;P->TeleportTo(D.Position+FVector(-500,0,40),FRotator::ZeroRotator);Next();
            }break;
        case 10: if(SmokeTime>2)
            {
                SmokeCheck(FVector::Dist2D(D.Position,SmokePosition)>80&&D.Activity==ENeighbourActivity::Follow,TEXT("Walking buddy follows a moving-away player"));
                bool Grounded=true;for(const auto& L:D.Gait->LegStates())if(L.Planted)Grounded&=FVector::Dist(D.Gait->Skin->GetBoneTransformByName(L.Paw,EBoneSpaces::WorldSpace).GetLocation(),L.Contact)<.5f;
                SmokeCheck(Grounded,TEXT("Stance paws match their fixed world contacts while the body advances"));
                NearDog();FrameDog(D);Next();
            }break;
        case 11: if(SmokeTime>.5){Shot(TEXT("NeighbourDogs.png"));Key(EKeys::Gamepad_FaceButton_Left,true);Next();}break;
        case 12: if(SmokeTime>.2)
            {
                Key(EKeys::Gamepad_FaceButton_Left,false);SmokeCheck(D.Activity==ENeighbourActivity::Return&&D.Visit==0,TEXT("Controller goodbye sends buddy home"));
                P->TeleportTo(D.Home+FVector(-750,0,40),FRotator::ZeroRotator);Next();
            }break;
        case 13: if(SmokeTime>8)
            {
                SmokeCheck(FVector::Dist2D(D.Position,D.Home)<300,TEXT("Dismissed buddy returns to the home roaming area"));
                // Exercise a real house wall: a fast target must not tunnel through it.
                D.Position=D.Home+FVector(-400,0,0);D.Target=D.Home+FVector(0,-700,0);D.Activity=ENeighbourActivity::Follow;
                P->TeleportTo(D.Home+FVector(0,-1100,40),FRotator::ZeroRotator);Next();
            }break;
        case 14: if(SmokeTime>4)
            {
                const FVector House=D.Home+FVector(0,-700,0);
                SmokeCheck(!(FMath::Abs(D.Position.X-House.X)<390&&FMath::Abs(D.Position.Y-House.Y)<340),TEXT("Body sweeps keep a following dog outside the solid house footprint"));
                P->TeleportTo(D.Position+FVector(-2000,0,40),FRotator::ZeroRotator);Next();
            }break;
        case 15: if(SmokeTime>.3)
            {
                SmokeCheck(D.Activity==ENeighbourActivity::Return,TEXT("Long-distance travel safely ends a buddy walk"));
                N->GoInside(P);Next();
            }break;
        case 16: if(SmokeTime>.4)
            {
                SmokeCheck(!N->Outside&&D.Visit==0&&D.Position.Equals(D.Home,.1),TEXT("Indoor return resets outdoor visits and positions"));
                SmokeCheck(D.Bond==1&&Adventure->Progress->NeighbourDogBonds[0]==1,TEXT("Returning home retains earned friendship"));
                auto* Reload=Cast<UCozyAdventureSave>(UGameplayStatics::LoadGameFromSlot(TEXT("CheweenieOutsideAcceptance"),0));
                SmokeCheck(Reload&&Reload->NeighbourDogBonds.Num()==8&&Reload->NeighbourDogBonds[0]==1,TEXT("Real disk save round trip retains friendship"));
                N->GoOutside(P);P->TeleportTo(D.Position+FVector(0,0,400),FRotator::ZeroRotator);
                SmokeCheck(!N->Interact(P)&&!N->Prompt(P).Contains(TEXT("say hello")),TEXT("Different-floor proximity cannot interact with a dog"));
                NearDog();for(int32 J=0;J<5;++J){D.Visit=1;D.Cooldown=0;D.Activity=ENeighbourActivity::Sniff;N->Interact(P);}
                SmokeCheck(D.Bond==3&&Adventure->Progress->NeighbourDogBonds[0]==3,TEXT("Repeated play safely caps saved friendship at three"));
                D.Activity=ENeighbourActivity::Follow;auto& Other=N->Dogs[1];Other.Visit=2;Other.Cooldown=0;
                P->TeleportTo(Other.Position+FVector(-100,0,40),FRotator::ZeroRotator);N->Interact(P);
                SmokeCheck(Other.Activity==ENeighbourActivity::Follow&&D.Activity==ENeighbourActivity::Return,TEXT("Inviting another buddy sends the previous companion home"));
                N->ResetDogVisits();P->TeleportTo(N->Dogs[1].Home+FVector(-150,0,45),FRotator::ZeroRotator);PC->SetControlRotation(FRotator(-10,0,0));FrameDog(N->Dogs[1]);Next();
            }break;
        case 17: if(SmokeTime>.6){Shot(TEXT("NeighbourHank.png"));Next();}break;
        case 18: if(SmokeTime>.4){P->TeleportTo(N->Dogs[2].Home+FVector(-150,0,45),FRotator::ZeroRotator);FrameDog(N->Dogs[2]);Next();}break;
        case 19: if(SmokeTime>.6){Shot(TEXT("NeighbourPotcake.png"));Next();}break;
        case 20: if(SmokeTime>.4)
            {
                auto Report=MakeShared<FJsonObject>();TArray<TSharedPtr<FJsonValue>> Pass,Fail;
                for(auto& S:SmokePassed)Pass.Add(MakeShared<FJsonValueString>(S));for(auto& S:SmokeFailed)Fail.Add(MakeShared<FJsonValueString>(S));
                Report->SetStringField(TEXT("scope"),TEXT("Real solo game map; synthetic keyboard/gamepad; teleport fixtures; physical hardware unverified"));
                Report->SetArrayField(TEXT("passed"),Pass);Report->SetArrayField(TEXT("failed"),Fail);FString Json;FJsonSerializer::Serialize(Report,TJsonWriterFactory<>::Create(&Json));
                const bool Saved=FFileHelper::SaveStringToFile(Json,*(Root/TEXT("dog-npcs.json")));UGameplayStatics::DeleteGameInSlot(TEXT("CheweenieOutsideAcceptance"),0);
                FPlatformMisc::RequestExitWithStatus(false,Saved&&SmokeFailed.IsEmpty()?0:1);SmokeStage=100;
            }break;
        }
        return;
    }
    if(FParse::Param(FCommandLine::Get(),TEXT("CheweenieOutsideMapAudit")))
    {
        if(SmokeStage==0&&SmokeTime>3)
        {
            SmokeCheck(N->PrivateMapLoaded&&N->RoadSegments>20,TEXT("Retained private streets validate and instantiate without publishing coordinates"));
            SmokeCheck(!N->Park.IsZero()&&!N->School.IsZero()&&N->Houses==9,TEXT("Private park, school and schematic homes are present"));
            Adventure->Start(true);N->GoOutside(P);SmokePosition=P->GetActorLocation();Key(EKeys::W,true);Next();
        }
        else if(SmokeStage==1&&SmokeTime>1.5)
        {
            Key(EKeys::W,false);SmokeCheck(N->Outside&&P->GetCharacterMovement()->IsMovingOnGround()&&FVector::Dist2D(P->GetActorLocation(),SmokePosition)>60,TEXT("Marty walks on the private-map ground"));
            P->ReturnHome();SmokeCheck(!N->Outside,TEXT("Private-map recovery returns to headquarters"));Next();
        }
        else if(SmokeStage==2&&SmokeTime>.2)
        {
            auto O=MakeShared<FJsonObject>();TArray<TSharedPtr<FJsonValue>> Pass,Fail;
            for(auto& S:SmokePassed)Pass.Add(MakeShared<FJsonValueString>(S));for(auto& S:SmokeFailed)Fail.Add(MakeShared<FJsonValueString>(S));
            O->SetArrayField(TEXT("passed"),Pass);O->SetArrayField(TEXT("failed"),Fail);FString Json;FJsonSerializer::Serialize(O,TJsonWriterFactory<>::Create(&Json));
            bool Saved=FFileHelper::SaveStringToFile(Json,*(Root/TEXT("outside-map-audit.json")));UGameplayStatics::DeleteGameInSlot(TEXT("CheweenieOutsideAcceptance"),0);
            FPlatformMisc::RequestExitWithStatus(false,Saved&&SmokeFailed.IsEmpty()?0:1);SmokeStage=100;
        }
        return;
    }
    switch(SmokeStage)
    {
    case 0:
        if(SmokeTime>(FApp::CanEverRender()?40:3))
        {
            SmokeCheck(!N->PrivateMapLoaded&&N->Houses==9&&N->Dogs.Num()==8&&N->RoadSegments>8,TEXT("Schematic exterior builds nine houses, eight dog slots and connected routes"));
            Adventure->Start(true);Adventure->Progress->SnackMask=1;Adventure->Save();PC->SetControlRotation(FRotator(-12,0,0));Key(EKeys::S,true);Next();
        }break;
    case 1:
        if(P->GetActorLocation().X< -335||SmokeTime>4){Key(EKeys::S,false);Next();}break;
    case 2:
        if(SmokeTime>.25){SmokeCheck(N->Prompt(P).Contains(TEXT("open the door")),TEXT("Actual indoor walking reaches the exit door prompt"));Key(EKeys::E,true);Next();}break;
    case 3:
        if(SmokeTime>.7)
        {
            Key(EKeys::E,false);SmokeCheck(N->Outside&&FVector::Dist2D(P->GetActorLocation(),N->Entrance)<320,TEXT("Keyboard interaction opens door and transfers Marty outside onto the front walk with camera clearance"));
            SmokeCheck(P->GetCharacterMovement()->IsMovingOnGround(),TEXT("Outside arrival lands on collision ground"));
            SmokeCheck(Adventure->Progress->SnackMask==1,TEXT("Door transition preserves the indoor mission"));
            Shot(TEXT("OutsideArrival.png"));SmokePosition=P->GetActorLocation();Key(EKeys::W,true);Next();
        }break;
    case 4:
        if(FVector::Dist2D(P->GetActorLocation(),SmokePosition)>100||SmokeTime>4)
        {
            Key(EKeys::W,false);SmokeCheck(FVector::Dist2D(P->GetActorLocation(),SmokePosition)>80&&P->GetCharacterMovement()->IsMovingOnGround(),TEXT("Real movement walks on the larger exterior ground"));
            P->GetCharacterMovement()->StopMovementImmediately();P->TeleportTo(N->Dogs[0].Position+FVector(0,0,45),FRotator::ZeroRotator);Next();
        }break;
    case 5:
        if(SmokeTime>.7){Key(EKeys::Gamepad_FaceButton_Left,true);Next();}break;
    case 6:
        if(SmokeTime>.2)
        {
            Key(EKeys::Gamepad_FaceButton_Left,false);SmokeCheck(N->MetDogs==1&&(Adventure->Progress->NeighbourDogMask&1),TEXT("Controller interaction greets a dog and saves its slot"));
            Adventure->Interact();SmokeCheck(N->MetDogs==1,TEXT("Repeated dog greeting does not duplicate friendship"));
            P->TeleportTo(N->Entrance+FVector(160,0,25),FRotator::ZeroRotator);Next();
        }break;
    case 7:
        if(SmokeTime>.5){Adventure->Interact();Next();}break;
    case 8:
        if(SmokeTime>.6)
        {
            SmokeCheck(FVector::Dist2D(P->GetActorLocation(),N->Park)<80&&P->GetCharacterMovement()->IsMovingOnGround(),TEXT("Working tour board reaches the park destination"));Shot(TEXT("OutsidePark.png"));
            Next();
        }break;
    case 9:
        if(SmokeTime>.2&&!SmokeMoving){P->TeleportTo(N->Park+FVector(300,0,40),FRotator::ZeroRotator);SmokeMoving=true;}
        if(SmokeTime>.7){Adventure->Interact();SmokeMoving=false;Next();}break;
    case 10:
        if(SmokeTime>.6)
        {
            SmokeCheck(FVector::Dist2D(P->GetActorLocation(),N->School)<80&&P->GetCharacterMovement()->IsMovingOnGround(),TEXT("Tour continues to the school exterior"));
            PC->SetControlRotation(FRotator(-8,90,0));Next();
        }break;
    case 11:
        if(SmokeTime>1){Shot(TEXT("OutsideSchool.png"));Next();}break;
    case 12:
        if(SmokeTime>.2&&!SmokeMoving){Key(EKeys::H,true);SmokeMoving=true;}
        if(SmokeTime>.8)
        {
            Key(EKeys::H,false);SmokeMoving=false;SmokeCheck(!N->Outside&&FVector::Dist2D(P->GetActorLocation(),FVector(-200,-200,0))<10,TEXT("H safely returns from the neighbourhood to headquarters"));
            P->TeleportTo(FVector(-345,-200,35),FRotator::ZeroRotator);Next();
        }break;
    case 13:
        if(SmokeTime>.4){Adventure->Interact();Next();}break;
    case 14:
        if(SmokeTime>.2&&!SmokeMoving){Key(EKeys::S,true);SmokeMoving=true;}
        if(FVector::Dist2D(P->GetActorLocation(),N->Entrance)<105||SmokeTime>4){Key(EKeys::S,false);SmokeMoving=false;Key(EKeys::Gamepad_FaceButton_Left,true);Next();}break;
    case 15:
        if(SmokeTime>.4)
        {
            Key(EKeys::Gamepad_FaceButton_Left,false);SmokeCheck(!N->Outside,TEXT("Controller opens the exterior home door after walking back to the entrance"));
            auto Report=MakeShared<FJsonObject>();auto Strings=[](const TArray<FString>& In){TArray<TSharedPtr<FJsonValue>> Out;for(auto& S:In)Out.Add(MakeShared<FJsonValueString>(S));return Out;};
            Report->SetStringField(TEXT("scope"),TEXT("Actual door and walking input; teleported fixtures for dog/landmark selection; schematic map only; physical devices unverified"));
            Report->SetArrayField(TEXT("passed"),Strings(SmokePassed));Report->SetArrayField(TEXT("failed"),Strings(SmokeFailed));FString Json;FJsonSerializer::Serialize(Report,TJsonWriterFactory<>::Create(&Json));
            bool Saved=FFileHelper::SaveStringToFile(Json,*(Root/TEXT("outside-smoke.json")));UGameplayStatics::DeleteGameInSlot(TEXT("CheweenieOutsideAcceptance"),0);
            FPlatformMisc::RequestExitWithStatus(false,Saved&&SmokeFailed.IsEmpty()?0:1);SmokeStage=100;
        }break;
    }
}
