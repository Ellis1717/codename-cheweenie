#include "MartyCharacter.h"
#include "MartyStance.h"
#include "MartyGait.h"
#include "DogLife.h"
#include "DogJackets.h"
#include "CozyAdventure.h"
#include "Components/CapsuleComponent.h"
#include "Components/PoseableMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "InputKeyEventArgs.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "Misc/App.h"
#include "UnrealClient.h"

void AMartyPlayMode::TickStanceSmoke(float Delta)
{
#if !UE_BUILD_SHIPPING
    SmokeTime+=Delta;auto* PC=GetWorld()->GetFirstPlayerController();auto* P=PC?Cast<AMartyCharacter>(PC->GetPawn()):nullptr;
    if(!P||!Adventure||!Adventure->Progress||!P->Gait->Ready)return;
    auto* S=Adventure.Get();auto* D=S->Progress.Get();auto* Move=P->GetCharacterMovement();auto* Stance=P->Stances.Get();
    static FVector4 Axes(0,0,0,0);static FVector Start;static float Spine=0,Feet=0,Energy=0;static int32 Barks=0,Camera=0;
    static TWeakObjectPtr<AStaticMeshActor> Roof;static TWeakObjectPtr<ACameraActor> View;
    FString Root;FParse::Value(FCommandLine::Get(),TEXT("CheweenieTestOutput="),Root);
    const auto A=EKeys::Gamepad_FaceButton_Bottom,Y=EKeys::Gamepad_FaceButton_Top,LB=EKeys::Gamepad_LeftShoulder,L3=EKeys::Gamepad_LeftThumbstick,R3=EKeys::Gamepad_RightThumbstick;
    auto Key=[&](FKey K,bool Down){PC->InputKey(FInputKeyEventArgs(nullptr,INPUTDEVICEID_NONE,K,Down?IE_Pressed:IE_Released,FPlatformTime::Cycles64()));};
    const FKey StickKeys[]={EKeys::Gamepad_LeftX,EKeys::Gamepad_LeftY,EKeys::Gamepad_RightX,EKeys::Gamepad_RightY};
    for(int32 I=0;I<4;++I)PC->InputKey(FInputKeyEventArgs(nullptr,INPUTDEVICEID_NONE,StickKeys[I],Axes[I],Delta,1,FPlatformTime::Cycles64()));
    TArray<TPair<float,TFunction<void()>>> Steps;auto Add=[&](float T,TFunction<void()> F){Steps.Emplace(T,MoveTemp(F));};
    auto Check=[&](bool Good,const TCHAR* Name){SmokeCheck(Good,Name);};
    auto Tap=[&](FKey K,TFunction<void()> F){Add(.12f,[&,K](){Key(K,true);});Add(.14f,[&,K,F=MoveTemp(F)](){F();Key(K,false);});Add(.12f,[](){});};
    auto Chord=[&](FKey K,TFunction<void()> F){Add(.12f,[&](){Key(LB,true);});Tap(K,MoveTemp(F));Add(.12f,[&](){Key(LB,false);});Add(.12f,[](){});};
    auto Place=[&](float X=-230.f){Axes=FVector4(0,0,0,0);P->ReturnHome();Stance->Set(EMartyStance::FourPaws,true);Move->StopMovementImmediately();P->TeleportTo(FVector(X,1100,45),FRotator::ZeroRotator);PC->SetControlRotation(FRotator(-12,0,0));PC->SetViewTarget(P);D->PetEnergy=100;D->PetPace=0;};
    auto Shot=[&](const TCHAR* Name,bool Low=false){if(!FApp::CanEverRender())return;if(!View.IsValid())View=GetWorld()->SpawnActor<ACameraActor>();const FVector Focus=P->GetActorLocation()+FVector(0,0,Stance->Active==EMartyStance::Upright?3:-7);const FVector Eye=Focus+FVector(Low?135:175,-210,Low?7:60);View->GetCameraComponent()->FieldOfView=55;View->SetActorLocationAndRotation(Eye,(Focus-Eye).Rotation());PC->SetViewTarget(View.Get());FScreenshotRequest::RequestScreenshot(Root/Name,true,false);};
    auto Box=[&](FVector Position,FVector Size){auto* B=GetWorld()->SpawnActor<AStaticMeshActor>(Position,FRotator::ZeroRotator);auto* C=B->GetStaticMeshComponent();C->SetMobility(EComponentMobility::Movable);C->SetStaticMesh(LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Cube.Cube")));C->SetCollisionProfileName(TEXT("BlockAll"));B->SetActorScale3D(Size/100.f);return B;};
    Add(FApp::CanEverRender()?20.f:3.f,[&](){Check(S->Slot==TEXT("CheweenieStanceAcceptance"),TEXT("Movement audit uses an isolated adventure slot"));});
    Tap(A,[&](){Check(S->Screen==ECozyScreen::Playing,TEXT("Production controller confirmation enters adventure"));Box(FVector(0,1100,-5),FVector(800,400,10));Roof=Box(FVector(0,1100,80),FVector(100,240,60));Roof->SetActorEnableCollision(false);Roof->SetActorHiddenInGame(true);Place();});
    Add(.5f,[&](){Check(Move->IsMovingOnGround()&&Stance->Supported(),TEXT("Marty is grounded and supports skeletal movement stances"));Check(FMath::IsNearlyEqual(P->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight(),28.f),TEXT("Four-paw collision retains the original height"));Spine=P->Skin->GetBoneLocationByName(TEXT("spine"),EBoneSpaces::ComponentSpace).Z;Feet=P->GetActorLocation().Z-P->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();Start=P->GetActorLocation();Axes=FVector4(0,.9f,0,0);Key(A,true);});
    Add(.5f,[&](){Check(P->GetVelocity().Size2D()>160,TEXT("Holding A while steering reaches running speed"));Key(Y,true);});
    Add(.13f,[&](){Check(Move->IsFalling()&&Move->Velocity.Z>100,TEXT("Y jumps immediately while A-run remains held"));Check(P->GetVelocity().Size2D()>150&&P->GetActorLocation().X>Start.X+50,TEXT("Running jump preserves forward momentum"));Shot(TEXT("MartyRunningJump.png"));Key(Y,false);});
    Add(1.f,[&](){Check(Move->IsMovingOnGround()&&P->GetVelocity().Size2D()>150,TEXT("Held steering and run continue on landing"));Axes=FVector4(0,0,0,0);Key(A,false);});
    Add(.4f,[&](){Check(P->GetVelocity().Size2D()<1,TEXT("Releasing run and movement stops without creeping"));Place();});
    Add(.4f,[&](){Axes=FVector4(.3f,.8f,0,0);});
    Tap(L3,[&](){Check(P->Life->SprintRequested,TEXT("L3 run remains available before a moving jump"));});
    Tap(Y,[&](){Check(Move->IsFalling()&&P->GetVelocity().Size2D()>100&&FMath::Abs(P->GetVelocity().Y)>10,TEXT("L3-run jump supports diagonal controller movement"));});
    Add(1.f,[&](){Place();});
    Add(.4f,[&](){Key(EKeys::W,true);Key(EKeys::LeftShift,true);});
    Add(.4f,[&](){Key(EKeys::SpaceBar,true);});
    Add(.13f,[&](){Check(Move->IsFalling()&&P->GetVelocity().Size2D()>140,TEXT("Keyboard W Shift Space also jumps while running"));Key(EKeys::W,false);Key(EKeys::LeftShift,false);Key(EKeys::SpaceBar,false);});
    Add(1.f,[&](){Place();});
    Add(.4f,[&](){P->TeleportTo(FVector(-200,1100,45),FRotator::ZeroRotator);Move->SetMovementMode(MOVE_Falling);Move->Velocity=FVector(60,0,-150);Key(Y,true);});
    Add(.025f,[&](){Check(Stance->JumpPending(),TEXT("A just-before-landing jump tap is buffered"));Key(Y,false);});
    Add(.18f,[&](){Check(Move->IsFalling()&&Move->Velocity.Z>100,TEXT("Buffered jump fires after actual ground contact"));});
    Add(1.f,[&](){Place();});
    Add(.4f,[&](){P->TeleportTo(FVector(-200,1100,70),FRotator::ZeroRotator);Move->SetMovementMode(MOVE_Falling);Move->Velocity=FVector(0,0,-20);Key(Y,true);});
    Add(.025f,[&](){Key(EKeys::Gamepad_Special_Right,true);});
    Add(.1f,[&](){Check(S->Screen==ECozyScreen::Pause&&!Stance->JumpPending(),TEXT("Opening a menu cancels queued jumps"));Key(Y,false);Key(EKeys::Gamepad_Special_Right,false);});
    Chord(L3,[&](){Check(Stance->Active==EMartyStance::FourPaws&&!P->Life->SprintRequested,TEXT("Crawl chord in a menu cannot queue movement or hurry"));});
    Tap(EKeys::Gamepad_FaceButton_Right,[](){});
    Add(.4f,[&](){Place();});
    Add(.3f,[](){}); // Finish the fixture's short setup fall before requesting a grounded stance.
    Tap(EKeys::Z,[&](){Check(Stance->Active==EMartyStance::Crawl,TEXT("Keyboard Z toggles movable crawling"));});
    Add(.4f,[&](){Check(P->Life->Activity==EDogActivity::Idle&&P->Skin->GetBoneLocationByName(TEXT("spine"),EBoneSpaces::ComponentSpace).Z<Spine-3,TEXT("Crawl lowers the torso without entering the stationary lie-down trick"));Check(FMath::IsNearlyEqual(P->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight(),22.f)&&FMath::IsNearlyEqual(P->GetActorLocation().Z-22,Feet,.3f),TEXT("Crawl lowers real collision while keeping feet on the floor"));Start=P->GetActorLocation();Axes=FVector4(0,1,0,0);});
    Add(.6f,[&](){Check(FVector::Dist2D(Start,P->GetActorLocation())>10&&P->GetVelocity().Size2D()<=46,TEXT("Crawling moves with an independent slow speed"));Axes=FVector4(0,0,0,0);Barks=P->Life->BarkCount;});
    Chord(L3,[&](){Check(Stance->Active==EMartyStance::FourPaws&&!P->Life->SprintRequested,TEXT("LB plus L3 returns from crawl without toggling run"));});
    Add(.2f,[&](){Check(P->Life->BarkCount==Barks,TEXT("A stance chord consumes the shoulder tap instead of barking"));Camera=P->CameraPreset;});
    Chord(R3,[&](){Check(Stance->Active==EMartyStance::Upright&&P->CameraPreset==Camera,TEXT("LB plus R3 selects two legs without cycling the camera"));});
    Add(.6f,[&](){Check(FMath::IsNearlyEqual(P->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight(),48.f)&&FMath::IsNearlyEqual(P->GetActorLocation().Z-48,Feet,.3f),TEXT("Two-leg collision grows upward from planted feet"));Check(P->Skin->GetBoneLocationByName(TEXT("head"),EBoneSpaces::ComponentSpace).Z>50&&P->Skin->GetBoneLocationByName(TEXT("front_l_paw"),EBoneSpaces::ComponentSpace).Z>30,TEXT("Two-leg pose visibly raises the head and folds the front paws"));Check(P->Gait->PlantedPaws<=2&&P->Skin->GetBoneLocationByName(TEXT("hind_l_paw"),EBoneSpaces::ComponentSpace).Z<8,TEXT("Only the hind paws support upright standing"));const auto* Boom=P->FindComponentByClass<USpringArmComponent>();Check(FMath::IsNearlyEqual(float(P->GetActorLocation().Z+Boom->SocketOffset.Z),Feet+63,.3f),TEXT("Default follow camera keeps its focus height while Marty rises"));if(FApp::CanEverRender())FScreenshotRequest::RequestScreenshot(Root/TEXT("MartyTwoLegFollow.png"),true,false);Start=P->GetActorLocation();Key(EKeys::W,true);});
    Add(.5f,[&](){Check(Move->IsMovingOnGround()&&P->GetVelocity().Size2D()>40&&P->GetVelocity().Size2D()<=81&&FVector::Dist2D(Start,P->GetActorLocation())>15,TEXT("Marty actually walks forward on his hind legs"));Shot(TEXT("MartyTwoLegWalk.png"));Key(EKeys::W,false);});
    Tap(A,[&](){Check(Stance->Active==EMartyStance::FourPaws&&P->Life->SprintRequested,TEXT("Requesting run safely returns upright Marty to four paws"));});
    Tap(EKeys::U,[&](){Check(Stance->Active==EMartyStance::Upright,TEXT("Keyboard U also selects two-leg walking"));});
    Add(.2f,[&](){D->MartyJacket=4;});
    Add(.4f,[&](){const FVector Front=(P->Skin->GetBoneLocationByName(TEXT("front_l_upper"),EBoneSpaces::WorldSpace)+P->Skin->GetBoneLocationByName(TEXT("front_r_upper"),EBoneSpaces::WorldSpace))*.5;const FVector Hips=(P->Skin->GetBoneLocationByName(TEXT("hind_l_upper"),EBoneSpaces::WorldSpace)+P->Skin->GetBoneLocationByName(TEXT("hind_r_upper"),EBoneSpaces::WorldSpace))*.5;const auto Jacket=P->Jackets->JacketTransform();Check(P->Jackets->VisiblePieces()>0&&FVector::DotProduct(Jacket.GetRotation().GetAxisX(),(Front-Hips).GetSafeNormal())>.95,TEXT("Equipped jacket follows the raised torso"));Check(P->Jackets->CollisionFree(),TEXT("Upright clothing adds no movement collision"));Shot(TEXT("MartyTwoLegJacket.png"));});
    Add(.2f,[&](){D->MartyJacket=0;});
    Tap(Y,[&](){Check(Stance->Active==EMartyStance::Upright&&Move->IsFalling(),TEXT("Two-leg stance can also jump"));Check(P->Skin->GetBoneLocationByName(TEXT("hind_l_paw"),EBoneSpaces::WorldSpace).Z>20,TEXT("Upright jump lifts the hind paws away from the floor"));});
    Add(1.f,[](){});
    Tap(EKeys::H,[&](){Check(Stance->Active==EMartyStance::FourPaws&&FMath::IsNearlyEqual(P->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight(),28.f),TEXT("Return home recovers normal stance and collision"));Place(-130);Roof->SetActorEnableCollision(true);Roof->SetActorHiddenInGame(false);});
    Add(.4f,[&](){Axes=FVector4(0,1,0,0);});
    Add(1.f,[&](){Check(P->GetActorLocation().X< -65&&P->GetVelocity().Size2D()<1,TEXT("A low beam blocks the ordinary standing capsule"));});
    Tap(EKeys::Z,[](){});
    Add(1.25f,[&](){Check(P->GetActorLocation().X> -30&&P->GetActorLocation().X<45&&Stance->Active==EMartyStance::Crawl,TEXT("Crawl collision passes under the same real low beam"));Axes=FVector4(0,0,0,0);});
    Tap(EKeys::Z,[&](){Check(Stance->Active==EMartyStance::Crawl&&S->Message.Contains(TEXT("headroom")),TEXT("Blocked stand-up keeps crawl and explains headroom"));const auto* Boom=P->FindComponentByClass<USpringArmComponent>();Check(FVector::Dist2D(Boom->GetSocketLocation(USpringArmComponent::SocketName),P->GetActorLocation())>150,TEXT("Crawl follow camera stays usable under the low beam"));if(FApp::CanEverRender())FScreenshotRequest::RequestScreenshot(Root/TEXT("MartyCrawlFollow.png"),true,false);});
    Add(.2f,[&](){Energy=D->PetEnergy;});
    Tap(Y,[&](){Check(Stance->Active==EMartyStance::Crawl&&!Move->IsFalling()&&!Stance->JumpPending(),TEXT("Jump cannot force Marty through a low ceiling"));Check(D->PetEnergy>Energy-1,TEXT("Blocked crawl jump does not spend jump energy"));Shot(TEXT("MartyCrawlingUnderBeam.png"),true);});
    Tap(EKeys::U,[&](){Check(Stance->Active==EMartyStance::Crawl,TEXT("Two-leg stance also respects overhead collision"));});
    Tap(A,[&](){Check(Stance->Active==EMartyStance::Crawl&&!P->Life->SprintRequested&&Move->MaxWalkSpeed<=45,TEXT("Run cannot bypass crawl headroom restrictions"));});
    Add(.2f,[&](){Axes=FVector4(0,1,0,0);});
    Add(2.7f,[&](){Check(P->GetActorLocation().X>80,TEXT("Marty can crawl clear of the low passage"));Axes=FVector4(0,0,0,0);});
    Tap(L3,[&](){Check(Stance->Active==EMartyStance::FourPaws&&P->Life->SprintRequested,TEXT("L3 resumes normal running once headroom is clear"));});
    Tap(EKeys::Z,[](){});
    Tap(Y,[&](){Check(Stance->Active==EMartyStance::FourPaws&&Move->IsFalling(),TEXT("A crawl jump in open space stands and jumps safely"));});
    Add(1.f,[&](){Place();Roof->SetActorEnableCollision(false);Roof->SetActorHiddenInGame(true);});
    Tap(EKeys::Gamepad_FaceButton_Right,[&](){Check(S->Screen==ECozyScreen::Tricks,TEXT("Existing tricks menu exposes movement options"));S->Selection=7;});
    Tap(A,[&](){Check(S->Screen==ECozyScreen::Playing&&Stance->Active==EMartyStance::Upright,TEXT("Controller menu confirmation selects two-leg walking"));});
    Tap(EKeys::U,[&](){Check(Stance->Active==EMartyStance::FourPaws,TEXT("Repeating the stance control returns to normal walking"));});
    Tap(R3,[&](){Check(P->CameraPreset!=Camera,TEXT("Unmodified R3 still cycles camera views"));});
    Add(.3f,[&](){auto J=MakeShared<FJsonObject>();auto Strings=[](const TArray<FString>& V){TArray<TSharedPtr<FJsonValue>> R;for(const auto& T:V)R.Add(MakeShared<FJsonValueString>(T));return R;};J->SetArrayField(TEXT("passed"),Strings(SmokePassed));J->SetArrayField(TEXT("failed"),Strings(SmokeFailed));J->SetStringField(TEXT("scope"),TEXT("Real game map with synthetic production keyboard/controller input, an isolated save/profile and spawned collision fixtures. Actual movement, momentum, ground contacts and skeletal poses checked. Human physical gameplay and subjective animation assessment unverified. New stances apply to solo skeletal Marty; room controls retain their existing scope."));FString Json;FJsonSerializer::Serialize(J,TJsonWriterFactory<>::Create(&Json));const bool Saved=FFileHelper::SaveStringToFile(Json,*(Root/TEXT("movement-stances.json")));SmokeStage=10000;FPlatformMisc::RequestExitWithStatus(false,Saved&&SmokeFailed.IsEmpty()?0:1);});
    if(Steps.IsValidIndex(SmokeStage)&&SmokeTime>=Steps[SmokeStage].Key){const int32 Before=SmokeStage;Steps[SmokeStage].Value();if(SmokeStage==Before)++SmokeStage;SmokeTime=0;}
#endif
}
