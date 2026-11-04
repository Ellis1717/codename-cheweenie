#include "DigVisuals.h"
#include "DogLife.h"
#include "MartyCharacter.h"
#include "MartyGait.h"
#include "CozyAdventure.h"
#include "CozyModeHub.h"
#include "HomeProgression.h"
#include "HouseUpstairs.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "EngineUtils.h"
#include "ProceduralMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GameFramework/PlayerController.h"

UDigVisuals::UDigVisuals(){PrimaryComponentTick.bCanEverTick=true;PrimaryComponentTick.TickGroup=TG_PostUpdateWork;}

void UDigVisuals::OpenGround()
{
    TArray<AStaticMeshActor*> Grounds;
    for(TActorIterator<AStaticMeshActor> It(GetWorld());It;++It)
        for(const TCHAR* Tag:{TEXT("Neighbourhood ground"),TEXT("Private mapped ground"),TEXT("Park lawn"),TEXT("WorldRealismBackdrop"),TEXT("WorldRealismGarden")})
            if(It->ActorHasTag(Tag)){Grounds.Add(*It);break;}
    // Use the actual lawn height, including the park's thin visual grass slab.
    for(FVector& P:Life->Patches)
        for(auto* A:Grounds)
        {
            const FBox B=A->GetStaticMeshComponent()->CalcBounds(A->GetActorTransform()).GetBox();
            if(P.X>B.Min.X&&P.X<B.Max.X&&P.Y>B.Min.Y&&P.Y<B.Max.Y&&B.Max.Z>=P.Z&&B.Max.Z<P.Z+10)P.Z=B.Max.Z;
        }
    auto* Cube=LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Cube.Cube"));
    for(auto* A:Grounds)
    {
        auto* Original=A->GetStaticMeshComponent();const FBox Bounds=Original->CalcBounds(A->GetActorTransform()).GetBox();
        TArray<FBox2D> Rectangles={FBox2D(FVector2D(Bounds.Min.X,Bounds.Min.Y),FVector2D(Bounds.Max.X,Bounds.Max.Y))};bool Cut=false;
        for(FVector P:Life->Patches)
        {
            if(Bounds.Max.Z>P.Z+3||Bounds.Max.Z<P.Z-100)continue;
            const FBox2D Hole(FVector2D(P.X-60,P.Y-55),FVector2D(P.X+60,P.Y+55));TArray<FBox2D> Next;
            for(const FBox2D& R:Rectangles)
            {
                const FVector2D Lo(FMath::Max(R.Min.X,Hole.Min.X),FMath::Max(R.Min.Y,Hole.Min.Y)),Hi(FMath::Min(R.Max.X,Hole.Max.X),FMath::Min(R.Max.Y,Hole.Max.Y));
                if(Lo.X>=Hi.X||Lo.Y>=Hi.Y){Next.Add(R);continue;}Cut=true;
                auto Add=[&](FVector2D Min,FVector2D Max){if(Max.X-Min.X>.01&&Max.Y-Min.Y>.01)Next.Add(FBox2D(Min,Max));};
                Add(R.Min,FVector2D(Lo.X,R.Max.Y));Add(FVector2D(Hi.X,R.Min.Y),R.Max);
                Add(FVector2D(Lo.X,R.Min.Y),FVector2D(Hi.X,Lo.Y));Add(FVector2D(Lo.X,Hi.Y),FVector2D(Hi.X,R.Max.Y));
            }
            Rectangles=MoveTemp(Next);
        }
        if(!Cut)continue;
        const auto Collision=Original->GetCollisionEnabled();
        for(const auto& R:Rectangles)
        {
            const auto XY=R.GetCenter(),Size=R.GetSize();auto* Tile=GetWorld()->SpawnActor<AStaticMeshActor>(FVector(XY.X,XY.Y,Bounds.GetCenter().Z),FRotator::ZeroRotator);Tile->Tags.Add(TEXT("DigGroundTile"));
            auto* C=Tile->GetStaticMeshComponent();C->SetMobility(EComponentMobility::Movable);C->SetStaticMesh(Cube);C->SetMaterial(0,Original->GetMaterial(0));Tile->SetActorScale3D(FVector(Size.X,Size.Y,Bounds.GetSize().Z)/100);
            C->SetCollisionProfileName(Collision==ECollisionEnabled::NoCollision?TEXT("NoCollision"):TEXT("BlockAll"));C->SetCastShadow(Original->CastShadow);C->SetAffectDistanceFieldLighting(Original->bAffectDistanceFieldLighting);C->SetAffectDynamicIndirectLighting(Original->bAffectDynamicIndirectLighting);C->SetCanEverAffectNavigation(false);
        }
        A->SetActorHiddenInGame(true);Original->SetCastShadow(false);Original->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
}

void UDigVisuals::RebuildPit(int32 Patch,int32 Depth,bool Filled)
{
    auto* Mesh=Pits[Patch].Get();Mesh->ClearAllMeshSections();
    const float Radius=21+Depth*9,Bottom=12,Floor=-Depth*10.f;
    auto Ring=[&](int32 Section,float Inner,float InnerZ,float Outer,float OuterZ,int32 Material,bool SquareOuter=false)
    {
        TArray<FVector> Vertices,Normals;TArray<int32> Triangles;TArray<FVector2D> UV;TArray<FLinearColor> Colors;TArray<FProcMeshTangent> Tangents;
        TArray<float> Angles;for(int32 S=0;S<=48;++S)Angles.Add(S*UE_TWO_PI/48);
        if(SquareOuter){const float Corner=FMath::Atan2(55.f,60.f);Angles.Append({Corner,UE_PI-Corner,UE_PI+Corner,UE_TWO_PI-Corner});Angles.Sort();}
        // Unreal's clockwise front faces match KismetProceduralMeshLibrary's upward grid.
        for(int32 S=0;S<Angles.Num();++S)
        {
            const float Angle=Angles[S],C=FMath::Cos(Angle),Sin=FMath::Sin(Angle);
            const float Edge=SquareOuter?FMath::Min(60.f/FMath::Max(FMath::Abs(C),.0001f),55.f/FMath::Max(FMath::Abs(Sin),.0001f)):Outer;
            const FVector N=FVector(-C*(OuterZ-InnerZ),-Sin*(OuterZ-InnerZ),(Outer-Inner)*.9f).GetSafeNormal();
            for(int32 Band=0;Band<2;++Band)
            {
                const float R=Band==0?Inner:Edge,Z=Band==0?InnerZ:OuterZ;const FVector P(C*R,Sin*R*(SquareOuter&&Band==1?1.f:.9f),Z);
                Vertices.Add(P);Normals.Add(N.IsNearlyZero()?FVector::UpVector:N);UV.Add(FVector2D(P.X,P.Y)/100);Colors.Add(FLinearColor::White);Tangents.Add(FProcMeshTangent(-Sin,C,0));
            }
            if(S+1<Angles.Num()){const int32 I=S*2;Triangles.Append({I,I+3,I+1});if(Inner>0)Triangles.Append({I,I+2,I+3});}
        }
        Mesh->CreateMeshSection_LinearColor(Section,Vertices,Triangles,Normals,UV,Colors,Tangents,true);
        auto* Grass=LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/Adventure/Scenery/Materials/Grass_World.Grass_World"));Mesh->SetMaterial(Section,Material<0&&Grass?Grass:Palette[FMath::Max(0,Material)].Get());
    };
    // The grass collar fills the rectangular cutout up to a circular soil patch.
    Ring(0,54,0,60,0,-1,true);
    Ring(1,Depth>0?Radius:0,.06f,54,.06f,3);
    if(Depth>0)
    {
        if(Filled)Ring(2,0,.06f,Radius,.06f,2);
        else
        {
            for(int32 Layer=0;Layer<3;++Layer)
            {
                const float A=Layer/3.f,B=(Layer+1)/3.f;
                Ring(2+Layer,FMath::Lerp(Bottom,Radius,A),FMath::Lerp(Floor,0.f,A),FMath::Lerp(Bottom,Radius,B),FMath::Lerp(Floor,0.f,B),Layer+1);
            }
            Ring(5,0,Floor,Bottom,Floor,0);
        }
    }
}

void UDigVisuals::Initialize(UDogLife* InLife)
{
    if(Life||!InLife)return;Life=InLife;
    auto* Base=LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/Adventure/World/NeighbourhoodSurface.NeighbourhoodSurface"));
    for(FLinearColor Color:{FLinearColor(.035f,.018f,.008f),FLinearColor(.19f,.085f,.025f),FLinearColor(.32f,.15f,.045f),FLinearColor(.46f,.25f,.085f),FLinearColor(.57f,.35f,.13f)})
    {auto* M=UMaterialInstanceDynamic::Create(Base,this);M->SetVectorParameterValue(TEXT("Color"),Color);Palette.Add(M);}
    auto* Sphere=LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    OpenGround();
    auto Batch=[&](AActor* A,UStaticMesh* Mesh,int32 Color)
    {
        auto* C=NewObject<UInstancedStaticMeshComponent>(A);A->AddInstanceComponent(C);C->SetupAttachment(A->GetRootComponent());C->SetMobility(EComponentMobility::Movable);C->SetStaticMesh(Mesh);C->SetMaterial(0,Palette[Color]);C->SetCollisionEnabled(ECollisionEnabled::NoCollision);C->SetCanEverAffectNavigation(false);C->RegisterComponent();return C;
    };
    for(FVector P:Life->Patches)
    {
        auto* A=GetWorld()->SpawnActor<AActor>(P,FRotator::ZeroRotator);A->Tags.Add(TEXT("DigVisualPatch"));
        auto* Root=NewObject<USceneComponent>(A);A->SetRootComponent(Root);Root->RegisterComponent();Root->SetWorldLocation(P);PatchActors.Add(A);
        auto* Center=NewObject<UStaticMeshComponent>(A);A->AddInstanceComponent(Center);Center->SetupAttachment(Root);Center->SetMobility(EComponentMobility::Movable);Center->SetStaticMesh(Sphere);Center->SetMaterial(0,Palette[0]);Center->SetCollisionEnabled(ECollisionEnabled::NoCollision);Center->RegisterComponent();Centers.Add(Center);
        auto* Pit=NewObject<UProceduralMeshComponent>(A);A->AddInstanceComponent(Pit);Pit->SetupAttachment(Root);Pit->bUseComplexAsSimpleCollision=true;Pit->bUseAsyncCooking=false;Pit->SetCollisionProfileName(TEXT("BlockAll"));Pit->SetCanEverAffectNavigation(false);Pit->RegisterComponent();Pits.Add(Pit);
        Rims.Add(Batch(A,Sphere,3));Covers.Add(Batch(A,Sphere,2));
        auto* T=NewObject<UTextRenderComponent>(A);A->AddInstanceComponent(T);T->SetupAttachment(Root);T->SetRelativeLocation(FVector(0,62,42));T->SetWorldSize(10);T->SetHorizontalAlignment(EHTA_Center);T->SetTextRenderColor(FColor(255,224,158));T->SetCollisionEnabled(ECollisionEnabled::NoCollision);T->RegisterComponent();Labels.Add(T);
        Depths.Add(-1);Buried.Add(-2);
    }
    auto* A=GetWorld()->SpawnActor<AActor>();auto* Root=NewObject<USceneComponent>(A);A->SetRootComponent(Root);Root->RegisterComponent();Clods=Batch(A,Sphere,3);A->Tags.Add(TEXT("DigFlyingSoil"));
    for(int32 I=0;I<18;++I)Clods->AddInstance(FTransform(FVector::ZeroVector));Clods->SetVisibility(false);
    Refresh();
}
void UDigVisuals::Refresh()
{
    auto* M=GetWorld()->GetAuthGameMode<AMartyPlayMode>();const auto* D=M&&M->Adventure?M->Adventure->Progress.Get():nullptr;if(!D||!Life)return;
    for(int32 I=0;I<PatchActors.Num();++I)
    {
        if(!D->HoleDepths.IsValidIndex(I))continue;const int32 Depth=FMath::Clamp(D->HoleDepths[I],0,3),Item=D->ItemHoles.IndexOfByKey(I);
        if(Depths[I]==Depth&&Buried[I]==Item)continue;Depths[I]=Depth;Buried[I]=Item;
        Rims[I]->ClearInstances();Covers[I]->ClearInstances();
        const float Radius=21+Depth*9,Bottom=12,Height=-Depth*10.f;
        Centers[I]->SetRelativeLocation(FVector(0,0,Height+.05f));Centers[I]->SetRelativeScale3D(FVector(Bottom*2,Bottom*1.8f,.1f)/100);Centers[I]->SetVisibility(Depth>0&&Item<0);
        RebuildPit(I,Depth,Item>=0);
        if(Depth>0)
        {
            FRandomStream Random(1717+I);
            for(int32 S=0;S<28;++S)
            {
                const float Angle=S*UE_TWO_PI/28,Spread=Radius+Random.FRandRange(1,6);
                Rims[I]->AddInstance(FTransform(FRotator(0,S*17,0),FVector(FMath::Cos(Angle)*Spread,FMath::Sin(Angle)*Spread*.9f,2),FVector(Random.FRandRange(6,10),Random.FRandRange(5,9),Random.FRandRange(3,5))/100));
            }
            for(int32 S=0;S<12;++S)
                Rims[I]->AddInstance(FTransform(FRotator(0,S*29,0),FVector(Random.FRandRange(-35,35),Radius+Random.FRandRange(6,25),Random.FRandRange(4,7)),FVector(12+Depth*4,10+Depth*3,6+Depth*2)/100));
            if(Item>=0)
            {
                Covers[I]->AddInstance(FTransform(FRotator::ZeroRotator,FVector(0,0,7),FVector(Radius*1.65f,Radius*1.45f,15)/100));
                for(int32 S=0;S<14;++S)Covers[I]->AddInstance(FTransform(FRotator(0,S*37,0),FVector(Random.FRandRange(-20,20),Random.FRandRange(-18,18),Random.FRandRange(10,14)),FVector(9,8,5)/100));
            }
        }
        Labels[I]->SetText(FText::FromString(DepthText(I)));
    }
}
int32 UDigVisuals::Nearest() const
{
    if(!Life)return -1;for(int32 I=0;I<Life->Patches.Num();++I)if(CheweenieHouse::Near(GetOwner()->GetActorLocation(),Life->Patches[I],105))return I;return -1;
}
int32 UDigVisuals::VisibleDepth(int32 Patch) const{return Depths.IsValidIndex(Patch)&&Pits.IsValidIndex(Patch)?Depths[Patch]:-1;}
bool UDigVisuals::Covered(int32 Patch) const{return Covers.IsValidIndex(Patch)&&Covers[Patch]->GetInstanceCount()>0;}
bool UDigVisuals::WorldLabelVisible(int32 Patch) const{return Labels.IsValidIndex(Patch)&&Labels[Patch]->IsVisible();}
int32 UDigVisuals::FlyingClods() const{return Clods&&Clods->IsVisible()?Clods->GetInstanceCount():0;}
float UDigVisuals::Relief(int32 Patch) const
{
    return Centers.IsValidIndex(Patch)?-Centers[Patch]->GetRelativeLocation().Z:0;
}
float UDigVisuals::BottomElevation(int32 Patch) const{return Pits.IsValidIndex(Patch)?Pits[Patch]->GetComponentLocation().Z-Relief(Patch):0;}
float UDigVisuals::StrokeProgress() const{return WasDigging?FMath::Clamp(DigElapsed/DigDuration,0.f,1.f):0;}
FString UDigVisuals::DepthText(int32 Patch) const
{
    if(!Depths.IsValidIndex(Patch))return FString();static const TCHAR* Names[]={TEXT("Untouched"),TEXT("Shallow"),TEXT("Medium"),TEXT("Deep")};const int32 Depth=FMath::Clamp(Depths[Patch],0,3);
    return FString::Printf(TEXT("Depth %d / 3 - %s%s"),Depth,Names[Depth],Buried[Patch]>=0?TEXT(" | Buried toy"):TEXT(""));
}
void UDigVisuals::TickComponent(float Delta,ELevelTick Type,FActorComponentTickFunction* Function)
{
    Super::TickComponent(Delta,Type,Function);if(!Life)return;Refresh();
    auto* M=GetWorld()->GetAuthGameMode<AMartyPlayMode>();auto* S=M?M->Adventure.Get():nullptr;if(!S||!S->Progress)return;
    const bool Playing=S->Screen==ECozyScreen::Playing,Enabled=S->Feature(ECozyFeature::Dig);const auto* PC=GetWorld()->GetFirstPlayerController();
    auto* Hub=GetWorld()->GetGameInstance()->GetSubsystem<UCozyModeHub>();
    for(int32 I=0;I<Labels.Num();++I)
    {
        const bool Show=Playing&&Enabled&&S->Feature(ECozyFeature::WorldLabels)&&Hub->OverlayVisible(ECozyOverlay::WorldLabels)&&CheweenieHouse::Near(GetOwner()->GetActorLocation(),Life->Patches[I],280);
        Labels[I]->SetVisibility(Show);if(Show&&PC){FVector Eye;FRotator R;PC->GetPlayerViewPoint(Eye,R);Labels[I]->SetWorldRotation((Eye-Labels[I]->GetComponentLocation()).Rotation());}
    }
    if(!Playing)return; // Pause freezes existing airborne soil and the stroke gauge.
    const bool Digging=Enabled&&Life->Activity==EDogActivity::Digging&&Nearest()>=0;
    if(Digging&&!WasDigging){DigElapsed=0;DigDuration=CheweenieHome(this)?CheweenieHome(this)->DigDuration():.9f;}
    if(Digging)DigElapsed+=Delta;WasDigging=Digging;Clods->SetVisibility(Digging);if(!Digging)return;
    const auto* P=Cast<AMartyCharacter>(GetOwner());const float Phase=P&&P->Gait&&P->Gait->Ready?P->Gait->DigStrokePhase():DigElapsed*18;
    for(int32 I=0;I<18;++I)
    {
        const float Side=I%2==0?-1.f:1.f,Age=FMath::Fmod(Phase+(I/2)*.7f+(Side>0?UE_PI:0),UE_TWO_PI)/18;
        FVector Point(25-85*Age,Side*(8+(35+(I%5)*12)*Age),-22+140*Age-350*Age*Age);
        const FVector World=GetOwner()->GetActorTransform().TransformPosition(Point);
        Clods->UpdateInstanceTransform(I,FTransform(FRotator(I*23+Age*300,I*17,0),World,FVector(2+(I%3)) /100),true,false,true);
    }
    Clods->MarkRenderStateDirty();
}
