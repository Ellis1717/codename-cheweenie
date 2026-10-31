#include "DogLife.h"
#include "MartyCharacter.h"
#include "MartyGait.h"
#include "CozyAdventure.h"
#include "Neighbourhood.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceDynamic.h"

UDogLife::UDogLife(){PrimaryComponentTick.bCanEverTick=true;}
AMartyCharacter* UDogLife::Pawn() const{return Cast<AMartyCharacter>(GetOwner());}
UCozyAdventure* UDogLife::Session() const{auto* M=GetWorld()->GetAuthGameMode<AMartyPlayMode>();return M?M->Adventure.Get():nullptr;}
UCozyAdventureSave* UDogLife::Data() const{auto* S=Session();return S?S->Progress.Get():nullptr;}
bool UDogLife::Playing() const{auto* S=Session();return S&&S->Enabled&&S->Screen==ECozyScreen::Playing&&Data();}
float UDogLife::Energy() const{return Data()?Data()->PetEnergy:80;}
bool UDogLife::Busy() const{return Activity==EDogActivity::Digging||Activity==EDogActivity::Eating||Activity==EDogActivity::CouchHop||Activity==EDogActivity::Rolling||Activity==EDogActivity::Shaking;}
bool UDogLife::OnCouch() const
{
    const FVector P=Pawn()->GetActorLocation();return P.X>250&&P.X<385&&P.Y> -220&&P.Y<205&&P.Z>90&&Pawn()->GetCharacterMovement()->IsMovingOnGround();
}
FString UDogLife::Mood() const{return Energy()<25?TEXT("Tired"):Excitement>0?TEXT("Excited"):TEXT("Normal");}
FString UDogLife::Status() const
{
    const auto* D=Data();FString Carry=D&&D->CarryItem>=0?(D->CarryItem==0?TEXT("tennis ball"):TEXT("treasure pebble")):TEXT("nothing");
    return FString::Printf(TEXT("Energy %d / 100 | %s | %s | Mouth: %s"),FMath::RoundToInt(Energy()),*Mood(),*PaceName(),*Carry);
}
void UDogLife::Tell(const FString& Text,const FString& Sound){if(auto* S=Session()){S->Message=Text;S->HoldMessage(5);S->ActionCue(Sound);}}
void UDogLife::Save(){if(auto* S=Session())S->Save();SaveTime=0;}
void UDogLife::BeginPlay()
{
    Super::BeginPlay();Initialize();
}
void UDogLife::Initialize()
{
    auto* D=Data();auto* M=GetWorld()->GetAuthGameMode<AMartyPlayMode>();
    if(Initialized||!D||!M||!M->Neighbourhood||M->Neighbourhood->Houses<9)return;
    Initialized=true;
    D->PetEnergy=FMath::IsFinite(D->PetEnergy)?FMath::Clamp(D->PetEnergy,0.f,100.f):80.f;
    D->PetHealth=FMath::IsFinite(D->PetHealth)?FMath::Clamp(D->PetHealth,0.f,100.f):100.f;
    D->PetPace=FMath::Clamp(D->PetPace,0,2);
    D->CarryItem=D->CarryItem>=0&&D->CarryItem<2?D->CarryItem:-1;
    if(D->HoleDepths.Num()!=3)D->HoleDepths={0,0,0};
    if(D->ItemHoles.Num()!=2)D->ItemHoles={-1,-1};
    for(auto& Depth:D->HoleDepths)Depth=FMath::Clamp(Depth,0,3);
    int32 Used=0;for(int32 I=0;I<2;++I)
    {
        int32& H=D->ItemHoles[I];if(H<0||H>2||I==D->CarryItem||(H>=0&&(Used&(1<<H))))H=-1;
        if(H>=0){Used|=1<<H;D->HoleDepths[H]=3;}
    }
    TArray<int32> CleanBag;for(int32 I:D->BagItems)if(I>=0&&I<2&&I!=D->CarryItem&&D->ItemHoles[I]<0&&!CleanBag.Contains(I)&&CleanBag.Num()<4)CleanBag.Add(I);D->BagItems=CleanBag;
    Build();SyncItems();SyncHoles();
}
AStaticMeshActor* UDogLife::Prop(FString Name,FVector P,FVector Size,int32 Color,const TCHAR* Shape,bool Solid)
{
    auto* A=GetWorld()->SpawnActor<AStaticMeshActor>(P,FRotator::ZeroRotator);auto* C=A->GetStaticMeshComponent();C->SetMobility(EComponentMobility::Movable);
    C->SetStaticMesh(LoadObject<UStaticMesh>(nullptr,Shape));A->SetActorScale3D(Size/100);C->SetMaterial(0,Materials[Color]);
    C->SetCollisionEnabled(Solid?ECollisionEnabled::QueryAndPhysics:ECollisionEnabled::NoCollision);if(Solid)C->SetCollisionProfileName(TEXT("BlockAll"));A->Tags.Add(FName(Name));return A;
}
void UDogLife::Label(FString Text,FVector P,float Yaw)
{
    auto* A=GetWorld()->SpawnActor<AActor>();auto* C=NewObject<UTextRenderComponent>(A);A->SetRootComponent(C);C->RegisterComponent();
    C->SetText(FText::FromString(Text));C->SetWorldSize(18);C->SetTextRenderColor(FColor(255,235,190));C->SetHorizontalAlignment(EHTA_Center);
    C->SetCollisionEnabled(ECollisionEnabled::NoCollision);A->SetActorLocationAndRotation(P,FRotator(0,Yaw,0));
}
void UDogLife::Build()
{
    auto* Base=LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/Adventure/World/NeighbourhoodSurface.NeighbourhoodSurface"));
    for(auto C:{FLinearColor(.12f,.065f,.025f),FLinearColor(.38f,.21f,.08f),FLinearColor(.8f,.65f,.25f),FLinearColor(.04f,.09f,.14f),FLinearColor(.6f,.74f,.7f),FLinearColor(.2f,.65f,.75f)})
    {auto* M=UMaterialInstanceDynamic::Create(Base,this);M->SetVectorParameterValue(TEXT("Color"),C);Materials.Add(M);}
    auto* Mode=GetWorld()->GetAuthGameMode<AMartyPlayMode>();if(!Mode||!Mode->Neighbourhood)return;auto* N=Mode->Neighbourhood.Get();
    Patches={N->Entrance+FVector(-400,-600,-28),N->Park+FVector(-300,200,0),N->Park+FVector(300,200,0)};
    for(auto P:Patches){Prop(TEXT("Dig patch rim"),P+FVector(0,0,2),FVector(110,95,4),1,TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));Holes.Add(Prop(TEXT("Dig hole"),P+FVector(0,0,4),FVector(20,20,2),0,TEXT("/Engine/BasicShapes/Cylinder.Cylinder")));}
    LooseLocations={FVector(0,-230,12),N->Entrance+FVector(450,-300,-16)};
    Items.Add(Prop(TEXT("Carryable tennis ball"),LooseLocations[0],FVector(13,13,13),2));
    Items.Add(Prop(TEXT("Carryable treasure pebble"),LooseLocations[1],FVector(15,10,8),4));
    Prop(TEXT("Kitchen food bowl"),Bowl+FVector(0,0,5),FVector(32,32,8),5,TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
    Prop(TEXT("Dog food"),Bowl+FVector(0,0,10),FVector(25,25,3),1,TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
    Prop(TEXT("Kitchen counter"),FVector(-250,425,42),FVector(145,65,84),4,TEXT("/Engine/BasicShapes/Cube.Cube"),true);
    Prop(TEXT("Kitchen fridge"),FVector(-100,425,80),FVector(70,65,160),4,TEXT("/Engine/BasicShapes/Cube.Cube"),true);
    Prop(TEXT("TV screen"),FVector(-458,100,175),FVector(10,240,135),3,TEXT("/Engine/BasicShapes/Cube.Cube"));
    TVBall=Prop(TEXT("Original TV ball animation"),FVector(-450,100,175),FVector(5,22,22),2);
    Label(TEXT("PAW TV\nOriginal ball bounce"),FVector(-446,100,270),0);
    const FVector View(0,-460,250);
    TVCamera=GetWorld()->SpawnActor<ACameraActor>(View,(FVector(0,20,140)-View).Rotation());TVCamera->GetCameraComponent()->FieldOfView=95;
}
void UDogLife::SyncItems()
{
    auto* D=Data();if(!D||Items.Num()!=2)return;
    for(int32 I=0;I<2;++I){Items[I]->SetActorHiddenInGame(D->ItemHoles[I]>=0||D->BagItems.Contains(I));if(D->CarryItem!=I)Items[I]->SetActorLocation(LooseLocations[I]);}
}
void UDogLife::SyncHoles()
{
    auto* D=Data();if(!D||Holes.Num()!=3)return;
    for(int32 I=0;I<3;++I){Holes[I]->SetActorHiddenInGame(D->HoleDepths[I]==0);Holes[I]->SetActorScale3D(FVector((25+D->HoleDepths[I]*19)/100.f,(25+D->HoleDepths[I]*17)/100.f,.025f));}
}
int32 UDogLife::NearPatch() const{for(int32 I=0;I<Patches.Num();++I)if(FVector::Dist2D(Pawn()->GetActorLocation(),Patches[I])<105)return I;return INDEX_NONE;}
FString UDogLife::Prompt() const
{
    if(!Playing())return FString();if(Busy())return Activity==EDogActivity::Rolling?TEXT("Rolling over..."):Activity==EDogActivity::Shaking?TEXT("Shake a paw!"):Activity==EDogActivity::Digging?TEXT("Digging with both front paws..."):Activity==EDogActivity::Eating?TEXT("Crunch crunch..."):TEXT("Hopping onto the couch...");
    if(Activity==EDogActivity::Lying)return TEXT("E / X-Square: stand up | movement also gets Marty up");
    if(Activity==EDogActivity::WatchingTV)return TEXT("Watching Paw TV and resting | E to stop, or walk away");
    auto* D=Data();const int32 H=NearPatch();
    if(H>=0&&D->CarryItem>=0&&D->HoleDepths[H]==3)return TEXT("E / X-Square: bury carried item | F / RT: dig");
    if(D->CarryItem<0)for(int32 I=0;I<Items.Num();++I)if(D->ItemHoles[I]<0&&!D->BagItems.Contains(I)&&FVector::Dist2D(Pawn()->GetActorLocation(),Items[I]->GetActorLocation())<70)return TEXT("E / X-Square: pick up this toy");
    if(H>=0)return TEXT("F / RT: dig here (three digs open a hole; dig again to recover treasure)");
    if(FVector::Dist2D(Pawn()->GetActorLocation(),Bowl)<80)return TEXT("E / X-Square: eat kitchen food and restore energy");
    if(OnCouch())return TEXT("E / X-Square: watch TV and rest");
    const FVector P=Pawn()->GetActorLocation();if(P.X>135&&P.X<250&&P.Y> -205&&P.Y<180)return TEXT("E / X-Square: hop onto couch | Space / Y-Triangle: jump");
    if(D->CarryItem<0)for(int32 I=0;I<Items.Num();++I)if(D->ItemHoles[I]<0&&!D->BagItems.Contains(I)&&FVector::Dist2D(P,Items[I]->GetActorLocation())<70)return TEXT("E / X-Square: pick up this toy");
    return FString();
}
void UDogLife::Dig()
{
    if(!Playing()||Busy())return;const int32 H=NearPatch();if(H<0){Tell(TEXT("Find a brown DIG PATCH in the front garden or park."));return;}
    if(Energy()<6){Tell(TEXT("Marty is tired. Rest or visit his kitchen bowl first."));return;}
    Data()->PetEnergy-=6;PendingPatch=H;Activity=EDogActivity::Digging;ActionTime=.9f;Save();Tell(TEXT("Marty's front paws go scritch, scritch!"),TEXT("Dig"));
}
void UDogLife::Bark()
{
    if(!Playing()||Busy()||BarkCooldown>0)return;
    BarkCooldown=1.2f;++BarkCount;Excitement=FMath::Max(Excitement,4.f);Tell(TEXT("Marty: Woof woof! Very important dog announcement."),TEXT("Bark"));
}
void UDogLife::Drop()
{
    if(!Playing()||Busy()||Data()->CarryItem<0)return;
    const int32 I=Data()->CarryItem;FVector P=Pawn()->GetActorLocation()+Pawn()->GetActorForwardVector()*55;P.Z-=20;
    FHitResult Hit;FCollisionQueryParams Q(SCENE_QUERY_STAT(ToyDrop),false,Pawn());
    if(GetWorld()->LineTraceSingleByChannel(Hit,P+FVector(0,0,80),P-FVector(0,0,160),ECC_Visibility,Q))P.Z=Hit.ImpactPoint.Z+9;
    LooseLocations[I]=P;Data()->CarryItem=-1;SyncItems();Save();Tell(TEXT("Toy dropped. Pick it up again with E / X-Square."),TEXT("Toy"));
}
bool UDogLife::PayJump()
{
    if(!Playing()||Busy()||Pawn()->GetCharacterMovement()->IsFalling())return false;
    if(Energy()<8){Tell(TEXT("Not enough energy to jump. Rest or eat some food."));return false;}
    StopWatching();StandUp();Data()->PetEnergy-=8;Excitement=6;Save();Session()->ActionCue(TEXT("Jump"));return true;
}
void UDogLife::StopWatching()
{
    if(Activity==EDogActivity::WatchingTV){Activity=EDogActivity::Idle;if(auto* PC=Cast<APlayerController>(Pawn()->GetController()))PC->SetViewTargetWithBlend(Pawn(),.2f);}
}
void UDogLife::CancelAction(){ClearBodyStick();BodyRoll=BodyPitch=BodyYawSpeed=0;BodyVelocity=FVector2D::ZeroVector;StopWatching();if(Activity==EDogActivity::Eating||Activity==EDogActivity::Lying||Activity==EDogActivity::Rolling||Activity==EDogActivity::Shaking)if(auto* PC=Cast<APlayerController>(Pawn()->GetController()))PC->SetViewTargetWithBlend(Pawn(),.2f);Activity=EDogActivity::Idle;ActionTime=0;HopDelay=0;PendingPatch=INDEX_NONE;SprintRequested=false;}
bool UDogLife::Interact()
{
    if(!Playing())return false;if(Busy())return true;if(Activity==EDogActivity::Lying){StandUp();return true;}auto* D=Data();const int32 H=NearPatch();
    if(H>=0&&D->CarryItem>=0&&D->HoleDepths[H]==3)
    {
        if(D->ItemHoles.Contains(H)){Tell(TEXT("There is already a treasure here. Dig it up first."));return true;}
        D->ItemHoles[D->CarryItem]=H;D->CarryItem=-1;Excitement=15;SyncItems();Save();Tell(TEXT("Buried treasure! Marty looks extremely pleased with himself."),TEXT("Dig"));return true;
    }
    if(D->CarryItem<0)for(int32 I=0;I<Items.Num();++I)
        if(D->ItemHoles[I]<0&&!D->BagItems.Contains(I)&&FVector::Dist2D(Pawn()->GetActorLocation(),Items[I]->GetActorLocation())<70)
        {D->CarryItem=I;Excitement=12;Save();Tell(TEXT("Marty carries his treasure in his mouth. Q / LT drops it; E buries it at an open hole."),TEXT("Toy"));return true;}
    if(FVector::Dist2D(Pawn()->GetActorLocation(),Bowl)<80)
    {
        if(FoodCooldown>0){Tell(TEXT("The bowl will refill soon. Marty can rest in the meantime."));return true;}
        Activity=EDogActivity::Eating;ActionTime=1.5f;Pawn()->GetCharacterMovement()->StopMovementImmediately();Pawn()->SetActorRotation(FRotator(0,(Bowl-Pawn()->GetActorLocation()).Rotation().Yaw,0));
        if(TVCamera){TVCamera->GetCameraComponent()->FieldOfView=95;const FVector View=Bowl+FVector(180,-220,135);TVCamera->SetActorLocationAndRotation(View,(Bowl+FVector(0,0,20)-View).Rotation());if(auto* PC=Cast<APlayerController>(Pawn()->GetController()))PC->SetViewTargetWithBlend(TVCamera,.2f);}
        Tell(TEXT("Kitchen snack time. Crunch, crunch!"),TEXT("Crunch"));return true;
    }
    if(OnCouch())
    {
        if(Activity==EDogActivity::WatchingTV){StopWatching();Tell(TEXT("Marty is ready to explore again."));}
        else {Activity=EDogActivity::WatchingTV;D->PetTVOn=true;Pawn()->GetCharacterMovement()->StopMovementImmediately();Pawn()->SetActorRotation(FRotator(0,180,0));if(auto* PC=Cast<APlayerController>(Pawn()->GetController()))if(TVCamera){TVCamera->GetCameraComponent()->FieldOfView=95;const FVector View(0,-460,250);TVCamera->SetActorLocationAndRotation(View,(FVector(0,20,140)-View).Rotation());PC->SetViewTargetWithBlend(TVCamera,.3f);}Tell(TEXT("Marty watches Paw TV and recharges on the couch."));Save();}
        return true;
    }
    const FVector P=Pawn()->GetActorLocation();
    if(P.X>135&&P.X<250&&P.Y> -205&&P.Y<180&&!Pawn()->GetCharacterMovement()->IsFalling())
    {
        if(!PayJump())return true;Activity=EDogActivity::CouchHop;ActionTime=1.7f;HopDelay=.22f;
        Pawn()->SetActorRotation(FRotator::ZeroRotator);Pawn()->LaunchCharacter(FVector(0,0,520),true,true);Tell(TEXT("Up we go! Marty hops onto the couch."));return true;
    }
    return false;
}
void UDogLife::FinishAction()
{
    auto* D=Data();if(!D)return;
    if(Activity==EDogActivity::Digging&&D->HoleDepths.IsValidIndex(PendingPatch))
    {
        int32 Found=D->ItemHoles.IndexOfByKey(PendingPatch);
        if(Found>=0){D->ItemHoles[Found]=-1;LooseLocations[Found]=Patches[PendingPatch]+FVector(65,0,10);Tell(TEXT("Treasure recovered! Pick it up again with E / X-Square."));}
        else {D->HoleDepths[PendingPatch]=FMath::Min(3,D->HoleDepths[PendingPatch]+1);Tell(D->HoleDepths[PendingPatch]==3?TEXT("A proper little hole! Bring a toy here and press E to bury it."):TEXT("The hole is getting deeper. Dig again with F / RT."));}
        SyncHoles();SyncItems();
    }
    else if(Activity==EDogActivity::Eating){if(auto* PC=Cast<APlayerController>(Pawn()->GetController()))PC->SetViewTargetWithBlend(Pawn(),.2f);D->PetEnergy=FMath::Min(100.f,D->PetEnergy+30);D->PetHealth=FMath::Min(100.f,D->PetHealth+20);FoodCooldown=25;Excitement=10;Tell(TEXT("A full bowl and a happier Marty. Energy restored!"),TEXT("Cozy"));}
    if(Activity==EDogActivity::Rolling||Activity==EDogActivity::Shaking)if(auto* PC=Cast<APlayerController>(Pawn()->GetController()))PC->SetViewTargetWithBlend(Pawn(),.2f);
    Activity=EDogActivity::Idle;PendingPatch=INDEX_NONE;Save();
}
void UDogLife::TickComponent(float Delta,ELevelTick Type,FActorComponentTickFunction* Function)
{
    Super::TickComponent(Delta,Type,Function);Initialize();if(!Playing()||!Initialized)return;auto* D=Data();auto* P=Pawn();
    BarkCooldown=FMath::Max(0.f,BarkCooldown-Delta);
    TickBodyControls(Delta);
    const bool Airborne=P->GetCharacterMovement()->IsFalling();
    if(WasAirborne&&!Airborne)Session()->ActionCue(TEXT("Land"));WasAirborne=Airborne;
    if(!Airborne&&!Busy()&&P->GetVelocity().Size2D()>8){StepClock-=Delta;if(StepClock<=0){Session()->ActionCue(TEXT("Step"));StepClock=P->GetVelocity().Size2D()>130?.24f:.4f;}}else StepClock=0;
    FoodCooldown=FMath::Max(0.f,FoodCooldown-Delta);Excitement=FMath::Max(0.f,Excitement-Delta);
    const float Speed=P->GetVelocity().Size2D();if(Activity==EDogActivity::WatchingTV&&(!OnCouch()||Speed>8))StopWatching();
    if(Activity==EDogActivity::CouchHop&&HopDelay>0)
    {
        HopDelay-=Delta;if(HopDelay<=0&&P->GetCharacterMovement()->IsFalling())P->LaunchCharacter(FVector(FMath::Clamp((300-P->GetActorLocation().X)/.65,70.,220.),0,0),true,false);
    }
    if(Busy()){ActionTime-=Delta;if(ActionTime<=0)FinishAction();}
    const float Rate=(Activity==EDogActivity::WatchingTV||Activity==EDogActivity::Lying)?TVRecovery:Speed>190?-HurryDrain:Speed>110?-HurryDrain*.5f:Speed>8?-WalkingDrain:Busy()?0:IdleRecovery;
    D->PetEnergy=FMath::Clamp(D->PetEnergy+Rate*Delta,0.f,100.f);
    // Keep legacy movement fixtures independent of a needs save that is not present there.
    P->GetCharacterMovement()->MaxWalkSpeed=WalkSpeed();
    P->GetCharacterMovement()->MaxAcceleration=WalkSpeed()>180?650:WalkSpeed()>110?500:350;
    P->GetCharacterMovement()->BrakingDecelerationWalking=600;
    if(Speed<8&&!Busy())D->PetHealth=FMath::Min(100.f,D->PetHealth+Delta*(Activity==EDogActivity::Lying||Activity==EDogActivity::WatchingTV?.3f:.1f));
    if(D->CarryItem>=0&&Items.IsValidIndex(D->CarryItem))Items[D->CarryItem]->SetActorLocation(P->Gait?P->Gait->MouthWorld():P->GetActorTransform().TransformPosition(FVector(40,0,5)));
    TVTime+=Delta;if(TVBall){TVBall->SetActorHiddenInGame(!D->PetTVOn);TVBall->SetActorLocation(FVector(-450,100+FMath::Sin(TVTime*1.7f)*90,175+FMath::Abs(FMath::Sin(TVTime*2.4f))*40-25));}
    SaveTime+=Delta;if(SaveTime>=10)Save();
}
