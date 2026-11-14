#include "AnimalGait.h"
#include "HockeySurprise.h"
#include "MartyCharacter.h"
#include "CozyAdventure.h"
#include "CatCompanions.h"
#include "HalloweenSpecial.h"
#include "DogLife.h"
#include "Engine/World.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Materials/MaterialInstanceDynamic.h"

namespace { const FVector Origin(0,0,3000); }
UHockeySurprise::UHockeySurprise(){PrimaryComponentTick.bCanEverTick=true;}
FVector UHockeySurprise::Target() const{return Origin+FVector(140,0,Stage==0?65:0);}
void UHockeySurprise::Tell(const FString& Text,const FString& Cue)
{
    auto* M=Cast<AMartyPlayMode>(GetOwner());M->Adventure->Message=Text;M->Adventure->HoldMessage(12);M->Adventure->ActionCue(Cue);
}
void UHockeySurprise::Clear(){for(auto A:Props)if(IsValid(A))A->Destroy();Props.Empty();Bag=nullptr;Cat=nullptr;}
void UHockeySurprise::Build(bool DressingRoom)
{
    Clear();
    auto* Cube=LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Cube.Cube"));
    auto* Base=LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/Adventure/World/NeighbourhoodSurface.NeighbourhoodSurface"));
    auto Box=[&](FVector P,FVector Size,FLinearColor Color,bool Solid=true)
    {
        auto* A=GetWorld()->SpawnActor<AStaticMeshActor>(Origin+P,FRotator::ZeroRotator);Props.Add(A);
        auto* C=A->GetStaticMeshComponent();C->SetMobility(EComponentMobility::Movable);C->SetStaticMesh(Cube);
        auto* Mat=UMaterialInstanceDynamic::Create(Base,A);Mat->SetVectorParameterValue(TEXT("Color"),Color);C->SetMaterial(0,Mat);
        A->SetActorScale3D(Size/100);C->SetCollisionProfileName(Solid?TEXT("BlockAll"):TEXT("NoCollision"));return A;
    };
    const FLinearColor Cream(.78,.72,.57),Blue(.04,.17,.28),Wood(.36,.17,.06),Gold(.9,.57,.12);
    Box(FVector(0,0,-10),FVector(1000,800,20),Cream);
    for(float Y:{-400.f,400.f})Box(FVector(0,Y,130),FVector(1020,20,260),Blue);
    for(float X:{-500.f,500.f})Box(FVector(X,0,130),FVector(20,800,260),Blue);
    auto Label=[&](FString Text,FVector P,float Size)
    {
        auto* A=GetWorld()->SpawnActor<AActor>();Props.Add(A);auto* T=NewObject<UTextRenderComponent>(A);A->SetRootComponent(T);T->RegisterComponent();T->SetText(FText::FromString(Text));T->SetWorldSize(Size);T->SetTextRenderColor(FColor(255,224,150));T->SetHorizontalAlignment(EHTA_Center);A->SetActorLocationAndRotation(Origin+P,FRotator(0,180,0));
    };
    Label(DressingRoom?TEXT("LEO'S LEARN TO SKATE\nDRESSING ROOM"):TEXT("HOCKEY MORNING\nONE VERY SUSPICIOUS BAG"),FVector(475,0,210),26);
    if(DressingRoom)
    {
        for(float Y:{-290.f,290.f})
        {
            Box(FVector(110,Y,35),FVector(520,60,16),Wood);
            for(float X:{-110.f,330.f})Box(FVector(X,Y,14),FVector(16,50,28),Wood);
            for(int32 I=0;I<5;++I){Box(FVector(-130+I*110,Y*1.22,115),FVector(95,35,190),Blue);Box(FVector(-130+I*110,Y*1.14,155),FVector(25,4,8),Gold,false);}
        }
        // Original little hockey sticks and a puck; no team branding.
        Box(FVector(325,150,80),FVector(8,8,155),Wood,false);
        Box(FVector(300,150,5),FVector(60,10,10),Wood,false);
        Box(FVector(280,100,3),FVector(16,16,6),FLinearColor(.015,.015,.015),false);
    }
    else
    {
        // Open pickup bed, cab and wheels; Marty stands on a loading platform beside it.
        Box(FVector(160,0,25),FVector(300,225,50),Blue);
        for(float Y:{-115.f,115.f})Box(FVector(160,Y,80),FVector(310,12,65),Blue);
        Box(FVector(365,0,100),FVector(105,240,175),Blue);
        Box(FVector(307,0,125),FVector(5,180,65),FLinearColor(.3,.6,.7),false);
        for(float X:{50.f,330.f})for(float Y:{-125.f,125.f})Box(FVector(X,Y,10),FVector(65,25,60),FLinearColor(.025,.025,.025));
        for(int32 I=0;I<5;++I)Box(FVector(-160+I*36,0,(I+1)*5),FVector(36,130,(I+1)*10),Wood);
    }
    Bag=Box(FVector(140,0,DressingRoom?22:72),FVector(110,65,44),FLinearColor(.16,.055,.23),false);
    for(float X:{105.f,175.f})Box(FVector(X,0,DressingRoom?45:95),FVector(7,68,6),Gold,false);
    Box(FVector(140,0,DressingRoom?54:104),FVector(45,8,8),Gold,false);
    Cat=GetWorld()->SpawnActor<AStaticMeshActor>(Origin+FVector(140,0,0),FRotator(0,180,0));Props.Add(Cat);
    auto* C=Cat->GetStaticMeshComponent();C->SetMobility(EComponentMobility::Movable);C->SetStaticMesh(LoadObject<UStaticMesh>(nullptr,TEXT("/Game/VisualLab/Pets/Lakitu.Lakitu")));C->SetCollisionEnabled(ECollisionEnabled::NoCollision);Cat->SetActorScale3D(FVector(.32));
    auto* Gait=NewObject<UAnimalGait>(Cat);Gait->RegisterComponent();if(Gait->Initialize(TEXT("Lakitu"),true,C,FTransform(FQuat::Identity,FVector::ZeroVector,FVector(3.125))))C->SetVisibility(false);
    Cat->SetActorHiddenInGame(true);
    auto* P=Cast<AMartyCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());P->GetCharacterMovement()->StopMovementImmediately();P->TeleportTo(Origin+FVector(-230,0,40),FRotator::ZeroRotator);GetWorld()->GetFirstPlayerController()->SetControlRotation(FRotator(-12,0,0));
}
void UHockeySurprise::Start()
{
    auto* M=Cast<AMartyPlayMode>(GetOwner());if(!M||!M->Adventure->Progress)return;
    if(!M->Adventure->Feature(ECozyFeature::HockeyStory)){Tell(TEXT("Enable Hockey bag story in Pet feature switches to play."));return;}
    if(Active()){auto* PC=GetWorld()->GetFirstPlayerController();M->Adventure->Screen=ECozyScreen::Playing;PC->bShowMouseCursor=false;PC->bEnableClickEvents=false;PC->SetInputMode(FInputModeGameOnly());return;}
    if(M->Festival&&M->Festival->Active())M->Festival->Toggle();
    auto* P=Cast<AMartyCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());if(!P)return;
    M->Adventure->CloseModes();M->Adventure->Start(false);
    M->Companions->Initialize();Stage=0;
    if(auto* A=M->Companions->Cats[0].Actor.Get())A->SetActorHiddenInGame(true);
    Build(false);Tell(TEXT("Leo's hockey bag is in the back of the truck. Nobody noticed Keetz snuggling inside..."));
}
void UHockeySurprise::Finish()
{
    if(!Active())return;Stage=-1;Clear();auto* M=Cast<AMartyPlayMode>(GetOwner());
    if(auto* A=M->Companions->Cats[0].Actor.Get())A->SetActorHiddenInGame(false);
    Cast<AMartyCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn())->ReturnHome();
    Tell(TEXT("Everybody home! Replay Lakitu's Hockey Bag Surprise from Game & versions."));
}
FString UHockeySurprise::Objective() const
{
    if(Stage==0)return TEXT("Hockey Bag Surprise | Walk to the truck bed and check Leo's bag.");
    if(Stage==1)return TEXT("Leo's Learn to Skate | The hockey bag just moved. Open it!");
    if(Stage==2)return TEXT("Surprise, Leo! Keeterman came to hockey. Say hello to the stowaway.");
    return TEXT("Honorary dressing-room mascot! Check the bag to take Keetz home.");
}
FString UHockeySurprise::Prompt() const
{
    const auto* PC=GetWorld()->GetFirstPlayerController();const APawn* P=PC?PC->GetPawn():nullptr;
    if(!P||FVector::Dist2D(P->GetActorLocation(),Target())>175||FMath::Abs(P->GetActorLocation().Z-Target().Z)>130)return FString();
    return Stage==0?TEXT("E / X-Square: bring the bag to Leo's dressing room"):Stage==1?TEXT("E / X-Square: unzip the wiggling hockey bag"):Stage==2?TEXT("E / X-Square: Keetz?! You came to hockey?!"):TEXT("E / X-Square: everybody home");
}
void UHockeySurprise::Interact()
{
    if(Prompt().IsEmpty())return;
    if(Stage==0){Stage=1;Build(true);Tell(TEXT("At Leo's Learn to Skate, the bag lands in the dressing room. Was that a meow?"));}
    else if(Stage==1){Stage=2;RevealTime=0;Cat->SetActorHiddenInGame(false);Tell(TEXT("SURPRISE! Lakitu pops out! Leo: Keetz?! You're not my skates!"),TEXT("Lakitu"));}
    else if(Stage==2){Stage=3;Tell(TEXT("Keeterman: I thought this was Cheeween Cozy, rink edition. Leo has a very fluffy new mascot!"),TEXT("Lakitu"));}
    else Finish();
}
void UHockeySurprise::TickComponent(float Delta,ELevelTick Type,FActorComponentTickFunction* Function)
{
    Super::TickComponent(Delta,Type,Function);if(!Active())return;
    auto* M=Cast<AMartyPlayMode>(GetOwner());if(M->Adventure->Screen!=ECozyScreen::Playing)return;
    if(Stage<2&&Bag)Bag->SetActorRotation(FRotator(0,4*FMath::Sin(GetWorld()->GetTimeSeconds()*5),0));
    if(Stage>=2&&Cat){RevealTime=FMath::Min(1.f,RevealTime+Delta);const float T=RevealTime;Cat->SetActorLocation(Origin+FVector(140-90*T,0,50*FMath::Sin(PI*T)));if(T<1)Cat->Tags.AddUnique(TEXT("AnimalAirborne"));else Cat->Tags.Remove(TEXT("AnimalAirborne"));}
}
