#include "MartyCharacter.h"
#include "CatCompanions.h"
#include "AnimalGait.h"
#include "Components/PoseableMeshComponent.h"
#include "CozyAdventure.h"
#include "CozyNavigation.h"
#include "Neighbourhood.h"
#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
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
#include "Components/SceneComponent.h"

void AMartyPlayMode::TickCompanionSmoke(float Delta)
{
    SmokeTime+=Delta;auto* PC=GetWorld()->GetFirstPlayerController();auto* P=PC?Cast<AMartyCharacter>(PC->GetPawn()):nullptr;
    if(!P||!Adventure->Progress||!Companions||!Navigation)return;auto* S=Adventure.Get();auto* D=S->Progress.Get();auto* C=Companions.Get();auto* N=Navigation.Get();
    FString Root=FPaths::ProjectSavedDir();FParse::Value(FCommandLine::Get(),TEXT("CheweenieTestOutput="),Root);
    auto Key=[PC](FKey K,bool Down){PC->InputKey(FInputKeyEventArgs(nullptr,INPUTDEVICEID_NONE,K,Down?IE_Pressed:IE_Released,FPlatformTime::Cycles64()));};
    auto Next=[this](){++SmokeStage;SmokeTime=0;};auto Shot=[&](const TCHAR* Name){if(FApp::CanEverRender())FScreenshotRequest::RequestScreenshot(Root/Name,true,false);};
    auto Finish=[&](bool Resume){auto O=MakeShared<FJsonObject>();TArray<TSharedPtr<FJsonValue>> Pass,Fail;for(auto& V:SmokePassed)Pass.Add(MakeShared<FJsonValueString>(V));for(auto& V:SmokeFailed)Fail.Add(MakeShared<FJsonValueString>(V));O->SetStringField(TEXT("scope"),FParse::Param(FCommandLine::Get(),TEXT("CheweenieSpeciesGaitSmoke"))?TEXT("Real-map matched-speed transform fixtures compare feline and canine animation. Separate companion tests cover keyboard/controller input. Not a measured biomechanics or performance claim."):TEXT("Real map; synthetic keyboard/controller with menu, position and mission/bond fixtures. Articulated feline skins with retained actor identity. Physical devices unverified."));O->SetBoolField(TEXT("privateMapLoaded"),Neighbourhood->PrivateMapLoaded);O->SetArrayField(TEXT("passed"),Pass);O->SetArrayField(TEXT("failed"),Fail);FString Json;FJsonSerializer::Serialize(O,TJsonWriterFactory<>::Create(&Json));const bool Saved=FFileHelper::SaveStringToFile(Json,*(Root/(Resume?TEXT("companions-resume.json"):TEXT("companions-smoke.json"))));if(Resume)UGameplayStatics::DeleteGameInSlot(TEXT("CheweenieCompanionAcceptance"),0);FPlatformMisc::RequestExitWithStatus(false,Saved&&SmokeFailed.IsEmpty()?0:1);SmokeStage=1000;};
    if(FParse::Param(FCommandLine::Get(),TEXT("CheweenieSpeciesGaitSmoke")))
    {
        static TWeakObjectPtr<AActor> Bodies[2];static TWeakObjectPtr<UAnimalGait> Gaits[2];
        static float StartTravel=0,MinBody[2]={1e9f,1e9f},MaxBody[2]={-1e9f,-1e9f},MinHead[2]={1e9f,1e9f},MaxHead[2]={-1e9f,-1e9f};
        static float MinTail[2]={1e9f,1e9f},MaxTail[2]={-1e9f,-1e9f},MaxRegisterError=0,MaxBalance=0,StopCycle=0;
        static int32 LandingSamples=0,Pairs[2]={0,0},Samples=0,Resets=0;static bool Finite=true;
        auto Move=[&](float Speed,float Yaw){const FVector Step=FRotator(0,Yaw,0).Vector()*Speed*FMath::Min(Delta,.05f);for(auto& A:Bodies)A->SetActorLocationAndRotation(A->GetActorLocation()+Step,FRotator(0,Yaw,0));};
        if(SmokeStage==0&&SmokeTime>(FApp::CanEverRender()?25:3))
        {
            S->Start(true);Neighbourhood->GoOutside(P);C->SetComponentTickEnabled(false);
            for(int32 I=0;I<2;++I)
            {
                auto* A=GetWorld()->SpawnActor<AActor>();auto* R=NewObject<USceneComponent>(A);A->SetRootComponent(R);R->RegisterComponent();A->SetActorLocation(Neighbourhood->Park+FVector(-700,I==0?-70:70,0));Bodies[I]=A;
                auto* G=NewObject<UAnimalGait>(A);G->RegisterComponent();Gaits[I]=G;
                SmokeCheck(G->Initialize(I==0?TEXT("Lakitu"):TEXT("Bauer"),I==0,R),I==0?TEXT("Feline comparison rig initializes on the real ground"):TEXT("Canine comparison rig initializes on the same real ground"));
            }
            if(!Gaits[0]->Ready||!Gaits[1]->Ready){Finish(false);return;}
            Next();return;
        }
        if(!Gaits[0].IsValid()||!Gaits[1].IsValid())return;auto* Cat=Gaits[0].Get();auto* Dog=Gaits[1].Get();
        for(auto& G:Gaits)for(const auto& L:G->LegStates())Finite&=!G->Skin->GetBoneTransformByName(L.Lower,EBoneSpaces::ComponentSpace).ContainsNaN();
        if(SmokeStage==1)
        {
            if(Cat->Speed>50&&Dog->Speed>50)
            {
                for(int32 I=0;I<2;++I)
                {
                    auto* G=Gaits[I].Get();const float H=G->Skin->GetBoneTransformByName(TEXT("head"),EBoneSpaces::ComponentSpace).GetLocation().Z;
                    const float T=G->Skin->GetBoneTransformByName(TEXT("tail_tip"),EBoneSpaces::ComponentSpace).GetLocation().Y;
                    MinBody[I]=FMath::Min(MinBody[I],G->BodyMotion);MaxBody[I]=FMath::Max(MaxBody[I],G->BodyMotion);MinHead[I]=FMath::Min(MinHead[I],H);MaxHead[I]=FMath::Max(MaxHead[I],H);MinTail[I]=FMath::Min(MinTail[I],T);MaxTail[I]=FMath::Max(MaxTail[I],T);
                }
                const auto& Legs=Cat->LegStates();for(int32 I=2;I<4;++I)if(Legs[I].RegisteredSwing&&Legs[I].Planted)
                {
                    const auto& F=Legs[I-2];const FVector Paw=Cat->Skin->GetBoneTransformByName(Legs[I].Paw,EBoneSpaces::WorldSpace).GetLocation();
                    float E=F.PrintValid?FVector::Dist2D(Paw,F.LastPlant):1e6f;if(F.PreviousPrintValid)E=FMath::Min(E,float(FVector::Dist2D(Paw,F.PreviousPlant)));MaxRegisterError=FMath::Max(MaxRegisterError,E);++LandingSamples;
                }
            }
            if(Cat->Travel<200){Move(70,0);return;}
            SmokeCheck(Cat->RegisteredHindSteps>=4&&LandingSamples>5,TEXT("Walking cat repeatedly follows real same-side front paw prints"));
            SmokeCheck(MaxRegisterError<2.5f,TEXT("Registered hind paw bones land within overlapping front paw prints"));
            SmokeCheck(Dog->RegisteredHindSteps==0,TEXT("Dog retains its independent paw placement policy"));
            SmokeCheck(MaxBody[0]-MinBody[0]<(MaxBody[1]-MinBody[1])*.8f,TEXT("Cat torso has less vertical bounce than the dog at matched world speed"));
            SmokeCheck(MaxHead[0]-MinHead[0]<(MaxHead[1]-MinHead[1])*.75f,TEXT("Cat head remains steadier than the canine head during matched-speed walking"));
            SmokeCheck(MaxTail[0]-MinTail[0]<(MaxTail[1]-MinTail[1])*.8f,TEXT("Straight-walking feline tail is quieter than the canine wag"));
            StartTravel=Cat->Travel;Next();return;
        }
        if(SmokeStage==2)
        {
            MaxBalance=FMath::Max(MaxBalance,FMath::Abs(Cat->TurnBalance));
            if(Cat->Travel-StartTravel<110){Move(70,FMath::Min(SmokeTime*35,90.f));return;}
            SmokeCheck(MaxBalance>.1f&&FMath::Abs(Dog->TurnBalance)<.001f,TEXT("Feline turning drives tail counterbalance rather than the dog wag policy"));StartTravel=Cat->Travel;Next();return;
        }
        if(SmokeStage==3)
        {
            if(Cat->Speed>225&&Dog->Speed>225){for(int32 I=0;I<2;++I)Pairs[I]+=Gaits[I]->LegStates()[0].Planted==Gaits[I]->LegStates()[1].Planted?1:0;++Samples;}
            if(Cat->Travel-StartTravel<250){Move(250,90);return;}
            SmokeCheck(Samples>10&&float(Pairs[0])/Samples>float(Pairs[1])/Samples+.3f,TEXT("Fast cat uses paired catch-up timing distinct from the canine diagonal trot"));
            SmokeCheck(Finite,TEXT("Feline and canine joints remain finite through walking, turning and faster movement"));StopCycle=Cat->Cycle;Resets=Cat->ContactResets;Next();return;
        }
        if(SmokeStage==4&&SmokeTime>1)
        {
            SmokeCheck(Cat->Cycle==StopCycle&&Cat->PlantedPaws==4,TEXT("Stationary cat finishes with planted paws and no treadmill stepping"));
            Bodies[0]->AddActorWorldOffset(FVector(500,0,0));Next();return;
        }
        if(SmokeStage==5&&SmokeTime>.3f)
        {
            SmokeCheck(Cat->ContactResets>Resets,TEXT("Cat teleport discards stale registered footprints"));
            bool OldPrint=false;for(const auto& L:Cat->LegStates())OldPrint|=L.PreviousPrintValid;
            SmokeCheck(!OldPrint,TEXT("No pre-teleport footprint history survives a recall"));
            auto Metrics=MakeShared<FJsonObject>();Metrics->SetNumberField(TEXT("registeredHindSwings"),Cat->RegisteredHindSteps);Metrics->SetNumberField(TEXT("registeredLandingSamples"),LandingSamples);Metrics->SetNumberField(TEXT("maxPrintErrorCm"),MaxRegisterError);Metrics->SetNumberField(TEXT("maxTurnBalance"),MaxBalance);
            Metrics->SetNumberField(TEXT("catBodySpan"),MaxBody[0]-MinBody[0]);Metrics->SetNumberField(TEXT("dogBodySpan"),MaxBody[1]-MinBody[1]);Metrics->SetNumberField(TEXT("catHeadSpan"),MaxHead[0]-MinHead[0]);Metrics->SetNumberField(TEXT("dogHeadSpan"),MaxHead[1]-MinHead[1]);Metrics->SetNumberField(TEXT("catTailSpan"),MaxTail[0]-MinTail[0]);Metrics->SetNumberField(TEXT("dogTailSpan"),MaxTail[1]-MinTail[1]);Metrics->SetNumberField(TEXT("fastCatFrontPairFraction"),Samples?float(Pairs[0])/Samples:0);Metrics->SetNumberField(TEXT("fastDogFrontPairFraction"),Samples?float(Pairs[1])/Samples:0);
            FString Json;FJsonSerializer::Serialize(Metrics,TJsonWriterFactory<>::Create(&Json));FFileHelper::SaveStringToFile(Json,*(Root/TEXT("species-gait-metrics.json")));Finish(false);return;
        }
        return;
    }
    if(FParse::Param(FCommandLine::Get(),TEXT("CheweenieCompanionResume"))){if(SmokeTime>3&&SmokeStage==0){SmokeCheck(D->CatFollowMask==1,TEXT("Independent follow/stay choices reload"));SmokeCheck(D->LakituBond==3&&D->RioBond==3&&C->Status().Contains(TEXT("Best buddies")),TEXT("Friendship and crew badge survive a second process"));SmokeCheck(!C->Cats[0].Outdoors&&!C->Cats[1].Outdoors&&C->Cats[0].Speed==0&&S->Screen==ECozyScreen::Title,TEXT("Reload resets transient cat travel and locomotion safely"));SmokeCheck(!S->Feature(ECozyFeature::Companions),TEXT("Companion feature switch persists independently of bonds"));Finish(true);}return;}
    switch(SmokeStage)
    {
    case 0:if(SmokeTime>(FApp::CanEverRender()?40:3)){S->Start(true);C->Initialize();SmokeCheck(C->Cats[0].Actor.IsValid()&&C->Cats[1].Actor.IsValid(),TEXT("Retained Lakitu and Rio meshes are found in the real map"));SmokeCheck(C->Cats[0].Gait.IsValid()&&C->Cats[1].Gait.IsValid()&&C->Cats[0].Gait->Ready&&C->Cats[1].Gait->Ready&&C->Cats[0].Gait->Feline,TEXT("Both cats have complete weighted feline rigs"));SmokeCheck(!C->Following(0)&&!C->Following(1),TEXT("Cats default to staying without changing patrol progress"));Key(EKeys::K,true);Next();}break;
    case 1:if(SmokeTime>.2){Key(EKeys::K,false);SmokeCheck(S->Screen==ECozyScreen::Companions,TEXT("Keyboard K opens crew commands"));Key(EKeys::Enter,true);Next();}break;
    case 2:if(SmokeTime>.2){Key(EKeys::Enter,false);SmokeCheck(C->Following(0)&&!C->Following(1),TEXT("Crew confirm enables only Lakitu following"));SmokeCheck(C->Cats[0].Speed>0&&C->Cats[0].Speed<C->LakituSpeed,TEXT("Lakitu accelerates smoothly instead of snapping to full speed"));SmokePosition=C->Position(0);Next();}break;
    case 3:if(SmokeTime>.5){SmokeCheck(FVector::Dist2D(SmokePosition,C->Position(0))>10,TEXT("Following moves the actual Lakitu actor"));SmokeCheck(C->Cats[0].Gait->Travel>0&&C->Cats[0].Gait->Cycle!=SmokeYaw&&C->Cats[0].Gait->MaxJointBend>15,TEXT("Feline distance-driven stride bends elbows, knees and hocks"));Key(EKeys::W,true);Next();}break;
    case 4:if(SmokeTime>.8){Key(EKeys::W,false);SmokeCheck(C->Cats[0].Gait->LegStates().Num()==4&&C->Cats[0].Gait->PlantedPaws>=2&&!C->Cats[0].Gait->Skin->GetBoneTransformByName(TEXT("spine"),EBoneSpaces::ComponentSpace).ContainsNaN(),TEXT("Cat shoulders and flexible spine retain a finite supported gait"));SmokeCheck(C->Cats[0].Speed<=C->CatchupSpeed&&FVector::Dist2D(P->GetActorLocation(),C->Position(0))<600,TEXT("Follow speed is bounded while Marty walks"));Key(EKeys::Gamepad_DPad_Down,true);Next();}break;
    case 5:if(SmokeTime>.2){Key(EKeys::Gamepad_DPad_Down,false);SmokeCheck(S->Screen==ECozyScreen::Companions,TEXT("Controller D-pad Down opens the crew menu"));Key(EKeys::Enter,true);Next();}break;
    case 6:if(SmokeTime>.2){Key(EKeys::Enter,false);SmokeCheck(!C->Following(0),TEXT("The same menu can ask Lakitu to stay"));SmokePosition=C->Position(0);Next();}break;
    case 7:if(SmokeTime>.5){SmokeCheck(FVector::Dist2D(SmokePosition,C->Position(0))<1&&C->Cats[0].Speed==0,TEXT("Stay stops the cat without position drift"));S->OpenCompanions();S->Selection=2;S->Confirm();SmokeCheck(D->CatFollowMask==3&&FVector::Dist2D(C->Position(0),C->Position(1))>50,TEXT("Call both places cats at separate nearby floor positions"));P->CycleCamera();PC->SetControlRotation(FRotator(-18,-30,0));Next();}break;
    case 8:if(SmokeTime>.3){Shot(TEXT("CompanionCrew.png"));Key(EKeys::E,true);Next();}break;
    case 9:if(SmokeTime>.2){Key(EKeys::E,false);SmokeCheck(D->LakituBond==1&&C->Cats[0].Happy>0,TEXT("Real interact input gives Lakitu a head bump and friendship"));C->Pet(0);SmokeCheck(D->LakituBond==1,TEXT("Head-bump cooldown prevents repeated bond farming"));C->Pet(1);SmokeCheck(D->RioBond==1,TEXT("Rio has independent friendship and reaction state"));Neighbourhood->GoOutside(P);Next();}break;
    case 10:if(SmokeTime>.4){SmokeCheck(C->Outdoors(0)&&C->Outdoors(1)&&FVector::Dist2D(C->Position(0),P->GetActorLocation())<250,TEXT("Following cats accompany Marty through the real exterior door"));D->SnackMask=(1<<S->Mission->Snacks.Num())-1;D->VisitIndex=0;const FVector Original=S->Mission->Visits[0].Position;S->Interact();SmokeCheck(D->VisitIndex==1,TEXT("Patrol interaction reaches a following cat at its natural distance"));SmokeCheck(S->Mission->Visits[0].Position.Equals(Original),TEXT("Moving cats do not mutate authored mission data assets"));S->Pause();SmokePosition=C->Position(0);Next();}break;
    case 11:if(SmokeTime>.5){SmokeCheck(FVector::Dist2D(SmokePosition,C->Position(0))<.01f,TEXT("Pause freezes cat movement and reactions"));S->Back();S->ToggleFeature(ECozyFeature::Companions);SmokeCheck(D->CatFollowMask==3&&D->LakituBond==1&&!C->Outdoors(0),TEXT("Disabling companions restores home without erasing choices or bonds"));S->ToggleFeature(ECozyFeature::Companions);P->ReturnHome();SmokeCheck(!C->Outdoors(0)&&!C->Outdoors(1),TEXT("Home recovery brings following cats safely indoors"));C->ToggleFollow(1);Neighbourhood->GoOutside(P);SmokeCheck(C->Outdoors(0)&&!C->Outdoors(1),TEXT("A staying cat remains indoors when the other follows outside"));D->NavTarget=5;N->RefreshRoute();SmokeCheck(N->Goal().Outdoors&&FVector::Dist2D(N->Goal().Position,C->Position(0))<2,TEXT("Map tracks the cat's live position and area"));D->NavTarget=2;N->RefreshRoute();SmokeYaw=N->GraphBuilds;SmokePitch=N->RouteSearches;Next();}break;
    case 12:if(SmokeTime>.2){TArray<double> Samples;for(int32 I=0;I<64;++I){N->RefreshRoute();Samples.Add(N->LastSearchMs);}Samples.Sort();SmokeCheck(N->GraphBuilds==int32(SmokeYaw)&&N->RouteSearches>=int32(SmokePitch)+64,TEXT("Repeated route searches reuse the compiled street graph"));SmokeCheck(N->StreetRoute&&FVector::Dist2D(N->Route.Last(),Neighbourhood->Park)<1,TEXT("Cached routing preserves the park destination"));auto O=MakeShared<FJsonObject>();O->SetStringField(TEXT("scope"),TEXT("64 synchronous CPU route refresh samples; this is not a frame-rate benchmark. No coordinates exported."));O->SetNumberField(TEXT("samples"),64);O->SetBoolField(TEXT("privateMapLoaded"),Neighbourhood->PrivateMapLoaded);O->SetNumberField(TEXT("graphBuilds"),N->GraphBuilds);O->SetNumberField(TEXT("graphBuildMs"),N->LastGraphMs);O->SetNumberField(TEXT("medianRouteMs"),Samples[32]);O->SetNumberField(TEXT("p95RouteMs"),Samples[60]);FString Json;FJsonSerializer::Serialize(O,TJsonWriterFactory<>::Create(&Json));FFileHelper::SaveStringToFile(Json,*(Root/TEXT("route-performance.json")));SmokePitch=N->RouteSearches;Next();}break;
    case 13:if(SmokeTime>1.3){SmokeCheck(N->RouteSearches==int32(SmokePitch),TEXT("Stationary player and destination avoid redundant route searches"));S->Pause();SmokePitch=N->RouteSearches;Next();}break;
    case 14:if(SmokeTime>.5){SmokeCheck(N->RouteSearches==int32(SmokePitch),TEXT("Paused menus do not run periodic routing work"));S->Back();N->InvalidateNetwork();N->RefreshRoute();SmokeCheck(N->GraphBuilds==int32(SmokeYaw)+1&&N->StreetRoute,TEXT("Explicit topology invalidation rebuilds a valid route"));C->SendHome();SmokeCheck(D->CatFollowMask==0&&!C->Outdoors(0)&&FVector::Dist2D(C->Position(0),C->Cats[0].Home.GetLocation())<1,TEXT("Send home resets both cats to their retained home spots"));P->ReturnHome();D->CatFollowMask=1;C->Cats[0].Position=FVector(-440,-200,6);C->Cats[0].Actor->SetActorLocation(C->Cats[0].Position);P->GetCharacterMovement()->SetMovementMode(MOVE_Flying);P->TeleportTo(FVector(-650,-200,28),FRotator::ZeroRotator);SmokeStage=60;SmokeTime=0;}break;
    case 60:if(SmokeTime>1.2){SmokeCheck(C->Position(0).X> -478,TEXT("Cat sweeps prevent following through the room wall"));C->SendHome();P->GetCharacterMovement()->SetMovementMode(MOVE_Walking);P->ReturnHome();SmokePosition=P->GetActorLocation();Key(EKeys::Down,true);SmokeStage=15;SmokeTime=0;}break;
    case 15:if(SmokeTime>.4){Key(EKeys::Down,false);SmokeCheck(S->Screen==ECozyScreen::Playing&&FVector::Dist2D(P->GetActorLocation(),SmokePosition)>5,TEXT("Keyboard Down still walks instead of opening companions"));D->CatFollowMask=1;D->LakituBond=D->RioBond=3;S->ToggleFeature(ECozyFeature::Companions);S->Save();S->OpenCompanions();Shot(TEXT("CompanionMenu.png"));Next();}break;
    case 16:if(SmokeTime>.4){S->Back();S->Pause();S->Selection=2;S->Confirm();S->Selection=2;S->Confirm();S->Selection=11;Shot(TEXT("CompanionSwitch.png"));Next();}break;
    case 17:if(SmokeTime>.4)Finish(false);break;
    }
}
