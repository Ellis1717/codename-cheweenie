#include "HalloweenCostume.h"
#include "GameFramework/Actor.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PoseableMeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "Engine/StaticMesh.h"

UHalloweenCostume::UHalloweenCostume()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = false;
    PrimaryComponentTick.TickGroup = TG_PostUpdateWork;
}

void UHalloweenCostume::Piece(const TCHAR* Shape, const TCHAR* Material, FVector Position, FVector Size, FRotator Rotation)
{
    auto* C = NewObject<UStaticMeshComponent>(GetOwner());
    C->SetupAttachment(Anchor);
    C->SetStaticMesh(LoadObject<UStaticMesh>(nullptr, *FString::Printf(TEXT("/Engine/BasicShapes/%s.%s"), Shape, Shape)));
    C->SetMaterial(0, LoadObject<UMaterialInterface>(nullptr, *FString::Printf(TEXT("/Game/Adventure/Halloween/%s.%s"), Material, Material)));
    C->SetRelativeLocation(Position);
    C->SetRelativeRotation(Rotation);
    C->SetRelativeScale3D(Size / 100.f);
    C->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    C->SetGenerateOverlapEvents(false);
    C->SetCanEverAffectNavigation(false);
    C->SetMobility(EComponentMobility::Movable);
    C->RegisterComponent();
    Pieces.Add(C);
}

void UHalloweenCostume::Build()
{
    // Dimensions are gameplay centimetres. Faces, paws and tails remain exposed.
    if (OutfitPet == 0)
    {
        // Marty's photographed hot-dog costume: soft bun sides, red saddle, mustard.
        Piece(TEXT("Sphere"), TEXT("Bun"), FVector(-4,-10,1), FVector(41,10,16));
        Piece(TEXT("Sphere"), TEXT("Bun"), FVector(-4,10,1), FVector(41,10,16));
        Piece(TEXT("Sphere"), TEXT("Red"), FVector(-4,0,8), FVector(38,17,10));
        for (int32 I=0; I<14; ++I)
        {
            const FVector A(-18+I*2.2f,FMath::Sin(I*.7f)*3.5f,13);
            const FVector B(-18+(I+1)*2.2f,FMath::Sin((I+1)*.7f)*3.5f,13);
            Piece(TEXT("Sphere"),TEXT("Gold"),(A+B)*.5f,FVector((B-A).Size()+1.3f,2.4f,1.6f),(B-A).Rotation());
        }
    }
    else if (OutfitPet == 1)
    {
        // Proposed pumpkin saddle for Lakitu; no mask obscures his asymmetric face.
        Piece(TEXT("Sphere"), TEXT("Orange"), FVector(-3,0,2), FVector(27,26,21));
        Piece(TEXT("Cylinder"), TEXT("Green"), FVector(-3,0,15), FVector(3,3,7), FRotator(0,0,15));
        Piece(TEXT("Sphere"), TEXT("Green"), FVector(-2,5,13), FVector(7,10,1.4f), FRotator(0,30,0));
        for (float Side : {-1.f,1.f})
        {
            Piece(TEXT("Cube"), TEXT("PumpkinSeam"), FVector(2,Side*12,5), FVector(3,1,3), FRotator(0,0,20));
            Piece(TEXT("Cube"), TEXT("PumpkinSeam"), FVector(-8,Side*12,5), FVector(3,1,3), FRotator(0,0,-20));
            Piece(TEXT("Sphere"), TEXT("Black"), FVector(-3,Side*12,0), FVector(9,1.5f,3));
        }
    }
    else
    {
        // Rio's photographed bee outfit: yellow jacket, black bands and pale wings.
        Piece(TEXT("Sphere"), TEXT("Gold"), FVector(-3,0,1), FVector(31,24,19));
        for (float X : {-11.f,-2.f,7.f})
            Piece(TEXT("Sphere"), TEXT("Black"), FVector(X,0,1), FVector(3.5f,24.6f,19.6f));
        for (float Side : {-1.f,1.f})
            Piece(TEXT("Sphere"), TEXT("Wing"), FVector(-4,Side*9,10.5f), FVector(19,13,1.5f), FRotator(0,Side*25,Side*18));
    }
}

void UHalloweenCostume::SetOutfit(USceneComponent* PetMesh, int32 Pet, bool Enabled)
{
    Enabled = Enabled && IsValid(PetMesh) && Pet>=0 && Pet<3;
    if (!Enabled)
    {
        Wearing = false;
        if (Anchor) Anchor->SetVisibility(false,true);
        SetComponentTickEnabled(false);
        return;
    }
    if (Wearing && Target==PetMesh && OutfitPet==Pet) return;
    if (!Anchor)
    {
        Anchor = NewObject<USceneComponent>(GetOwner());
        Anchor->SetupAttachment(GetOwner()->GetRootComponent());
        Anchor->RegisterComponent();
    }
    if (OutfitPet != Pet)
    {
        for (const auto& C : Pieces) C->DestroyComponent();
        Pieces.Empty();
        OutfitPet = Pet;
        Build();
    }
    Target = PetMesh;
    Wearing = true;
    Anchor->SetVisibility(true,true);
    SetComponentTickEnabled(true);
    TickComponent(0,LEVELTICK_All,nullptr);
}

void UHalloweenCostume::TickComponent(float Delta, ELevelTick Type, FActorComponentTickFunction* Function)
{
    Super::TickComponent(Delta,Type,Function);
    if (!Wearing || !IsValid(Target)) return;
    if (auto* Skin=Cast<UPoseableMeshComponent>(Target.Get()))
    {
        // Follow the animated torso, including lying down, rolling and body sway.
        const FTransform Spine=Skin->GetBoneTransformByName(TEXT("spine"),EBoneSpaces::WorldSpace);
        Anchor->SetWorldTransform(FTransform(Spine.GetRotation(),Spine.TransformPosition(FVector(1,0,4)),FVector::OneVector));
    }
    else
    {
        // Retained static cats use the same authored coordinate system at 32% scale.
        const FTransform Mesh=Target->GetComponentTransform();
        const FVector Body=OutfitPet==0?FVector(-15,0,82):FVector(-8,0,73);
        Anchor->SetWorldTransform(FTransform(Mesh.GetRotation(),Mesh.TransformPosition(Body),FVector::OneVector));
    }
}

int32 UHalloweenCostume::VisiblePieces() const
{
    int32 Count=0;
    for (const auto& C : Pieces) if (C->IsVisible()) ++Count;
    return Count;
}
bool UHalloweenCostume::CollisionFree() const
{
    for (const auto& C : Pieces) if(C->GetCollisionEnabled()!=ECollisionEnabled::NoCollision || C->GetGenerateOverlapEvents() || C->CanEverAffectNavigation()) return false;
    return true;
}
bool UHalloweenCostume::MaterialsReady() const
{
    if(Pieces.IsEmpty()) return false;
    for(const auto& C : Pieces) if(!C->GetStaticMesh() || !C->GetMaterial(0) || !C->GetMaterial(0)->GetPathName().StartsWith(TEXT("/Game/Adventure/Halloween/"))) return false;
    return true;
}
