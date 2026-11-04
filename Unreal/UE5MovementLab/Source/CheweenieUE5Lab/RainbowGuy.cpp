#include "RainbowGuy.h"
#include "Engine/World.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

AStaticMeshActor* SpawnRainbowGuy(UWorld* World, FVector Location)
{
    auto* Toy = World->SpawnActor<AStaticMeshActor>(Location, FRotator::ZeroRotator);
    Toy->Tags.Add(TEXT("Rainbow Guy"));
    auto* Root = Toy->GetStaticMeshComponent();
    Root->SetMobility(EComponentMobility::Movable);
    Root->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    auto* Sphere = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    auto* Base = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Adventure/World/NeighbourhoodSurface.NeighbourhoodSurface"));
    TArray<UMaterialInstanceDynamic*> Colors;
    for (FLinearColor Color : {FLinearColor(.85f,.015f,.035f), FLinearColor(1,.22f,.015f), FLinearColor(1,.82f,.025f), FLinearColor(.18f,.7f,.08f), FLinearColor(.015f,.57f,.85f), FLinearColor(.96f,.96f,.9f), FLinearColor(.008f,.012f,.02f), FLinearColor(.02f,.09f,.5f)})
    {
        auto* Material = UMaterialInstanceDynamic::Create(Base, Toy);
        Material->SetVectorParameterValue(TEXT("Color"), Color);
        Colors.Add(Material);
    }
    auto Part = [&](FVector P, FVector Size, int32 Color, FRotator Rotation = FRotator::ZeroRotator)
    {
        auto* Mesh = NewObject<UStaticMeshComponent>(Toy);
        Toy->AddInstanceComponent(Mesh);
        Mesh->SetupAttachment(Root);
        Mesh->SetMobility(EComponentMobility::Movable);
        Mesh->SetStaticMesh(Sphere);
        Mesh->SetMaterial(0, Colors[Color]);
        Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        Mesh->SetRelativeLocation(P);
        Mesh->SetRelativeRotation(Rotation);
        Mesh->SetRelativeScale3D(Size / 100);
        Mesh->RegisterComponent();
    };
    // A flat, cuddly monster: face upward so it reads while lying on the floor.
    Part(FVector::ZeroVector, FVector(23,27,8), 0);
    for (int32 Side : {-1, 1})
    {
        Part(FVector(0,Side*17,0), FVector(12,17,7), 0, FRotator(0,Side*25,0));
        Part(FVector(0,Side*25,0), FVector(10,9,5), 4);
        Part(FVector(-15,Side*7,0), FVector(12,10,5), 4);
        Part(FVector(15,Side*8,0), FVector(12,4,4), 4, FRotator(0,Side*20,0));
    }
    // Overlapping tapered locks give the red/orange/yellow/green bands a shaggy edge.
    FRandomStream Random(179371);
    for (int32 Row=0; Row<12; ++Row)
    {
        const float X=-11.f+Row*2.f;
        const float Width=Row>=4&&Row<=8?22.f:12.f;
        for (float Y=-Width; Y<=Width; Y+=2.5f)
            Part(FVector(X+Random.FRandRange(-.6f,.6f),Y,3.f+Random.FRandRange(0,.6f)), FVector(5.5f,3.2f,1.8f), (Row/2)%4, FRotator(0,Random.FRandRange(-22,22),0));
    }
    Part(FVector(1,0,5), FVector(6,12,1.5f), 6);
    for (int32 Side : {-1,1})
    {
        Part(FVector(6,Side*3.3f,5.6f), FVector(6,4.7f,2), 5);
        Part(FVector(6.4f,Side*3.3f,6.3f), FVector(2.8f,2.8f,1), 6);
        Part(FVector(7.1f,Side*3.3f-.5f,6.8f), FVector(.9f,.9f,.5f), 5);
        Part(FVector(8.2f,Side*3.3f,6), FVector(2,4.7f,1), 7);
        Part(FVector(1.8f,Side*3.2f,5.9f), FVector(2,1.8f,.7f), 5);
    }
    Part(FVector(-.2f,0,5.9f), FVector(2,2,.7f), 5);
    return Toy;
}
