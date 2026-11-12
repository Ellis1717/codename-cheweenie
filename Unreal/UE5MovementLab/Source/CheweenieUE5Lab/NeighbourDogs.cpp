#include "Neighbourhood.h"
#include "MartyCharacter.h"
#include "CozyAdventure.h"
#include "Engine/World.h"
#include "Engine/StaticMeshActor.h"
#include "Components/SceneComponent.h"
#include "Components/TextRenderComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"

UCheweenieNeighbourhood::UCheweenieNeighbourhood(){PrimaryComponentTick.bCanEverTick=true;}
FString UCheweenieNeighbourhood::DogAction(const FNeighbourDog& D) const
{
    if(D.Activity==ENeighbourActivity::Follow)return TEXT("say goodbye to ")+D.Name;
    if(D.Visit==0)return TEXT("say hello to ")+D.Name;
    if(D.Visit==1)return TEXT("play bow with ")+D.Name;
    return TEXT("invite ")+D.Name+TEXT(" for a walk");
}
void UCheweenieNeighbourhood::Dog(FNeighbourDog& D,int32 Index)
{
    // Original schematic characters. Unconfirmed potcake coats remain neutral placeholders.
    const int32 Color=Palette.Num();
    auto* Mat=UMaterialInstanceDynamic::Create(LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/Adventure/World/NeighbourhoodSurface.NeighbourhoodSurface")),this);
    Mat->SetVectorParameterValue(TEXT("Color"),D.Coat);Palette.Add(Mat);
    auto* Root=GetWorld()->SpawnActor<AActor>();auto* Scene=NewObject<USceneComponent>(Root);
    Root->SetRootComponent(Scene);Scene->SetMobility(EComponentMobility::Movable);Scene->RegisterComponent();Root->SetActorLocation(D.Position);D.Body=Root;
    const float Scale=Index==1?1.55f:Index==0?1.35f:1.f;
    auto Part=[&](const TCHAR* Name,FVector P,FVector Size,int32 C)
    {
        auto* A=Shape(Name,D.Position+P,Size,C,false,TEXT("/Engine/BasicShapes/Sphere.Sphere"));
        A->AttachToActor(Root,FAttachmentTransformRules::KeepWorldTransform);return A;
    };
    Part(TEXT("Neighbour dog"),FVector(0,0,43),FVector(72,33,40),Color);
    D.Head=Part(TEXT("Dog head"),FVector(-36,0,62),FVector(34,34,38),Color);
    auto Face=[&](const TCHAR* Name,FVector P,FVector Size,int32 C){auto* A=Part(Name,P,Size,C);A->AttachToActor(D.Head.Get(),FAttachmentTransformRules::KeepWorldTransform);return A;};
    Face(TEXT("Dog muzzle"),FVector(-53,0,56),FVector(28,23,20),Index==1?1:Color);
    Face(TEXT("Dog nose"),FVector(-66,0,59),FVector(9,14,10),5);
    for(int32 Side:{-1,1})
    {
        Face(TEXT("Dog ear"),FVector(-28,Side*21,55),FVector(17,10,32),Color);
        Face(TEXT("Dog eye"),FVector(-48,Side*14,70),FVector(7,6,8),5);
        Face(TEXT("Dog eye glint"),FVector(-50,Side*16,72),FVector(2,2,2),1);
        for(int32 X:{-1,1})
        {
            auto* Leg=Part(TEXT("Dog leg"),FVector(X*24,Side*13,21),FVector(12,12,39),Color);D.Legs.Add(Leg);
            auto* Paw=Part(TEXT("Dog paw"),FVector(X*24-4,Side*13,5),FVector(19,15,10),Index==1?1:Color);Paw->AttachToActor(Leg,FAttachmentTransformRules::KeepWorldTransform);
        }
    }
    Part(TEXT("Dog collar"),FVector(-25,0,48),FVector(10,37,37),Index%2?6:4);
    D.Tail=Part(TEXT("Dog tail"),FVector(43,0,55),FVector(35,12,16),Color);
    if(Index<2)
    {
        // Tufts provide a fluffy silhouette without claiming Hank's exact breed.
        for(int32 I=0;I<12;++I)
        {
            const float Angle=I*PI/6;Part(TEXT("Dog fluffy coat"),FVector(FMath::Cos(Angle)*28,FMath::Sin(Angle)*17,48+(I%3)*4),FVector(29,23,30),Index==1&&I%3==0?1:Color);
        }
        if(Index==1)Part(TEXT("Hank white chest"),FVector(-24,0,35),FVector(24,34,35),1);
    }
    Root->SetActorScale3D(FVector(Scale));
    auto* LabelActor=GetWorld()->SpawnActor<AActor>();auto* T=NewObject<UTextRenderComponent>(LabelActor);
    LabelActor->SetRootComponent(T);T->SetMobility(EComponentMobility::Movable);T->RegisterComponent();
    T->SetWorldSize(21);T->SetTextRenderColor(FColor(245,235,200));T->SetHorizontalAlignment(EHTA_Center);T->SetCollisionEnabled(ECollisionEnabled::NoCollision);D.Label=T;
}
void UCheweenieNeighbourhood::ResetDogVisits()
{
    for(auto& D:Dogs)
    {
        D.Position=D.Home;D.Target=D.Home;D.Activity=ENeighbourActivity::Roam;D.Timer=1;D.Visit=0;D.Cooldown=0;D.Blocked=0;
        if(D.Body.IsValid())D.Body->SetActorLocation(D.Home);
    }
}
bool UCheweenieNeighbourhood::MoveDog(FNeighbourDog& D,FVector Target,float Delta)
{
    FVector Direction=Target-D.Position;Direction.Z=0;if(Direction.Size2D()<15)return false;
    const float Step=FMath::Min(float(Direction.Size2D()),D.Speed*Delta*(D.Activity==ENeighbourActivity::Play?1.7f:1.f));
    const FVector Forward=Direction.GetSafeNormal2D();
    // Sweep the full body against real geometry; probe ground before accepting a step.
    FCollisionQueryParams Q;FHitResult Hit;bool Moved=false;
    for(float Angle:{0.f,55.f,-55.f,100.f,-100.f})
    {
        const FVector Heading=Forward.RotateAngleAxis(Angle,FVector::UpVector),Next=D.Position+Heading*Step;
        const float Radius=75.f*(D.Body.IsValid()?float(D.Body->GetActorScale3D().X):1.f);
        if(GetWorld()->SweepSingleByObjectType(Hit,D.Position+FVector(0,0,Radius+2),Next+FVector(0,0,Radius+2),FQuat::Identity,FCollisionObjectQueryParams(ECC_WorldStatic),FCollisionShape::MakeSphere(Radius),Q))continue;
        FHitResult Ground;if(!GetWorld()->LineTraceSingleByObjectType(Ground,Next+FVector(0,0,70),Next-FVector(0,0,90),FCollisionObjectQueryParams(ECC_WorldStatic),Q)||Ground.ImpactNormal.Z<.75f||FMath::Abs(Ground.ImpactPoint.Z-D.Position.Z)>25)continue;
        D.Position=FVector(Next.X,Next.Y,Ground.ImpactPoint.Z);D.Yaw=FMath::FixedTurn(D.Yaw,Heading.Rotation().Yaw+180,Delta*220);Moved=true;break;
    }
    D.Blocked=Moved?0:D.Blocked+Delta;return Moved;
}
void UCheweenieNeighbourhood::TickComponent(float Delta,ELevelTick Type,FActorComponentTickFunction* Function)
{
    Super::TickComponent(Delta,Type,Function);auto* M=GetWorld()->GetAuthGameMode<AMartyPlayMode>();
    if(!M||!M->Adventure)return;
    const bool Active=Outside&&M->Adventure->Screen==ECozyScreen::Playing;
    for(auto& D:Dogs)if(D.Label.IsValid())D.Label->SetVisibility(Active&&M->Adventure->Feature(ECozyFeature::WorldLabels));
    if(M->Adventure->Screen!=ECozyScreen::Playing)return;
    if(!Outside)
    {
        for(const auto& D:Dogs)if(D.Activity==ENeighbourActivity::Follow||D.Visit>0){ResetDogVisits();break;}
        return;
    }
    UpdateDogs(FMath::Min(Delta,.05f));
}
void UCheweenieNeighbourhood::UpdateDogs(float Delta)
{
    auto* PC=GetWorld()->GetFirstPlayerController();auto* P=PC?Cast<AMartyCharacter>(PC->GetPawn()):nullptr;if(!P)return;
    const FVector Player=P->GetActorLocation();
    for(int32 I=0;I<Dogs.Num();++I)
    {
        auto& D=Dogs[I];D.Timer-=Delta;D.Cooldown=FMath::Max(0.f,D.Cooldown-Delta);D.Phase+=Delta;
        const float Distance=FVector::Dist2D(Player,D.Position);bool Moving=false;
        if(D.Activity==ENeighbourActivity::Follow)
        {
            if(Distance>1600||FMath::Abs(Player.Z-D.Position.Z)>140){D.Activity=ENeighbourActivity::Return;D.Visit=0;}
            else if(Distance>140)Moving=MoveDog(D,Player-P->GetActorForwardVector()*100,Delta);
        }
        else if(D.Activity==ENeighbourActivity::Return)
        {
            Moving=MoveDog(D,D.Home,Delta);
            if(FVector::Dist2D(D.Position,D.Home)<25){D.Activity=ENeighbourActivity::Sniff;D.Timer=3;}
        }
        else if(D.Activity==ENeighbourActivity::Greet)
        {D.Yaw=FMath::FixedTurn(D.Yaw,(Player-D.Position).Rotation().Yaw+180,Delta*150);if(D.Timer<=0){D.Activity=ENeighbourActivity::Sniff;D.Timer=3;}}
        else if(D.Activity==ENeighbourActivity::Play)
        {
            // A short local bounce/chase loop never takes or modifies Marty's toys.
            const FVector Centre=FVector::Dist2D(Player,D.Home)<500?FVector(Player.X,Player.Y,D.Home.Z):D.Home;
            D.Target=Centre+FVector(FMath::Cos(D.Phase*1.8f)*130,FMath::Sin(D.Phase*1.8f)*130,0);
            Moving=MoveDog(D,D.Target,Delta);
            if(D.Timer<=0){D.Activity=ENeighbourActivity::Sniff;D.Timer=3;}
        }
        else if(Distance<210&&FMath::Abs(Player.Z-D.Position.Z)<100)
        {D.Yaw=FMath::FixedTurn(D.Yaw,(Player-D.Position).Rotation().Yaw+180,Delta*130);}
        else if(D.Activity==ENeighbourActivity::Sniff)
        {if(D.Timer<=0){D.Activity=ENeighbourActivity::Roam;D.Timer=6;}}
        else
        {
            if(D.Timer<=0||FVector::Dist2D(D.Position,D.Target)<20||D.Blocked>1)
            {
                ++D.RoamStep;const float Angle=I*1.4f+D.RoamStep*2.4f;
                D.Target=D.Home+FVector(FMath::Cos(Angle)*180,FMath::Sin(Angle)*150,0);D.Timer=5;D.Blocked=0;
                D.Activity=ENeighbourActivity::Sniff;
            }
            else Moving=MoveDog(D,D.Target,Delta);
        }
        if(D.Body.IsValid())D.Body->SetActorLocationAndRotation(D.Position+FVector(0,0,Moving?FMath::Abs(FMath::Sin(D.Phase*9))*2:0),FRotator(0,D.Yaw,0));
        for(int32 L=0;L<D.Legs.Num();++L)if(D.Legs[L].IsValid())D.Legs[L]->SetActorRelativeRotation(FRotator(Moving?FMath::Sin(D.Phase*10+(L==0||L==3?0:PI))*23:0,0,0));
        if(D.Tail.IsValid())D.Tail->SetActorRelativeRotation(FRotator(-15,FMath::Sin(D.Phase*(Distance<210?12:5))*(Distance<210?35:12),0));
        if(D.Head.IsValid())D.Head->SetActorRelativeRotation(FRotator(D.Activity==ENeighbourActivity::Sniff&&Distance>210?-20:D.Activity==ENeighbourActivity::Play?-12:0,0,0));
        if(D.Label.IsValid())
        {
            const TCHAR* State=D.Activity==ENeighbourActivity::Follow?TEXT("Walking buddy"):D.Activity==ENeighbourActivity::Return?TEXT("Heading home"):D.Activity==ENeighbourActivity::Play?TEXT("Play time!"):D.Activity==ENeighbourActivity::Sniff?TEXT("Sniffing"):TEXT("Exploring");
            D.Label->SetText(FText::FromString(D.Name+TEXT(" | ")+State+FString::Printf(TEXT("\nFriendship %d / 3"),D.Bond)));
            D.Label->SetWorldLocation(D.Position+FVector(0,0,I==1?175:I==0?150:120));
            D.Label->SetWorldRotation(FRotator(0,PC->PlayerCameraManager->GetCameraRotation().Yaw+180,0));
            if(auto* M=GetWorld()->GetAuthGameMode<AMartyPlayMode>())D.Label->SetVisibility(Distance<1100&&M->Adventure->Feature(ECozyFeature::WorldLabels));
        }
    }
}
