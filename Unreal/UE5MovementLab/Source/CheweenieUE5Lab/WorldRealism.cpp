#include "WorldRealism.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/StaticMesh.h"
#include "Engine/RectLight.h"
#include "Engine/PointLight.h"
#include "Engine/PostProcessVolume.h"
#include "EngineUtils.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/RectLightComponent.h"
#include "Components/PointLightComponent.h"
#include "Materials/MaterialInterface.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"

static bool LegacyWorld(){return FParse::Param(FCommandLine::Get(),TEXT("CheweenieLegacyWorld"));}
static UStaticMesh* ArtMesh(const TCHAR* Name){return LoadObject<UStaticMesh>(nullptr,*FString::Printf(TEXT("/Game/Adventure/Scenery/Meshes/%s.%s"),Name,Name));}
static void HideSurface(UStaticMeshComponent* C)
{
    C->SetVisibility(false);C->SetCastShadow(false);C->SetAffectDistanceFieldLighting(false);
}
static AStaticMeshActor* Art(UWorld* W,UStaticMesh* Mesh,FVector P,FName Tag,FRotator R=FRotator::ZeroRotator)
{
    if(!Mesh)return nullptr;auto* A=W->SpawnActor<AStaticMeshActor>(P,R);A->Tags.Add(Tag);auto* C=A->GetStaticMeshComponent();
    // FBX converts Blender's Y into UE's opposite-handed axis; mirror the visual overlay
    // back into the retained room's centimetre coordinates. Collision is never mirrored.
    A->SetActorScale3D(FVector(1,-1,1));
    C->SetMobility(EComponentMobility::Movable);C->SetStaticMesh(Mesh);A->SetActorEnableCollision(false);C->SetCollisionEnabled(ECollisionEnabled::NoCollision);return A;
}
void UpgradeCheweenieRoom(UWorld* W)
{
    if(LegacyWorld())return;
    auto* Shell=ArtMesh(TEXT("RoomShell"));if(!Shell){UE_LOG(LogTemp,Warning,TEXT("Detailed room art unavailable; retaining the original room"));return;}
    for(TActorIterator<AStaticMeshActor> It(W);It;++It)
    {
        if(It->ActorHasTag(TEXT("WorldRealismRoom")))return;
        if(It->ActorHasTag(TEXT("BlanketFortSurface")))continue;
        auto* C=It->GetStaticMeshComponent();const auto* M=C->GetMaterial(0);const UStaticMesh* Mesh=C->GetStaticMesh();
        if(Mesh&&Mesh->GetPathName().StartsWith(TEXT("/Game/VisualLab/Pets/")))continue;
        if((M&&M->GetPathName().StartsWith(TEXT("/Game/VisualLab/UE5Materials/")))||It->ActorHasTag(TEXT("Headquarters exit door")))HideSurface(C);
    }
    for(const TCHAR* Name:{TEXT("RoomShell"),TEXT("RoomFloor"),TEXT("RoomSofa"),TEXT("RoomDetails"),TEXT("RoomSoft"),TEXT("RoomPlants"),TEXT("RoomKitchen")})Art(W,ArtMesh(Name),FVector::ZeroVector,TEXT("WorldRealismRoom"));
    // Replace the old photography fills, retaining the sun/sky and their outdoor transitions.
    for(TActorIterator<ARectLight> It(W);It;++It)It->GetLightComponent()->SetIntensity(0);
    for(TActorIterator<APointLight> It(W);It;++It)It->GetLightComponent()->SetIntensity(0);
    auto Window=[&](FVector P,FRotator R,float Intensity,float Width,float Height)
    {
        auto* A=W->SpawnActor<ARectLight>(P,R);A->Tags.Add(TEXT("WorldRealismLight"));auto* C=Cast<URectLightComponent>(A->GetLightComponent());C->SetMobility(EComponentMobility::Movable);
        C->SetIntensityUnits(ELightUnits::Candelas);C->SetIntensity(Intensity);C->SetSourceWidth(Width);C->SetSourceHeight(Height);C->SetAttenuationRadius(1400);C->SetLightColor(FLinearColor(.92f,.96f,1.f));
    };
    Window(FVector(230,450,225),FRotator(-10,-90,0),8,165,195);
    Window(FVector(0,-450,225),FRotator(-12,90,0),6,360,180);
    Window(FVector(-445,120,240),FRotator(-18,0,0),2,220,160);
    auto* Lamp=W->SpawnActor<APointLight>(FVector(435,-340,218),FRotator::ZeroRotator);Lamp->Tags.Add(TEXT("WorldRealismLight"));
    auto* L=Cast<UPointLightComponent>(Lamp->GetLightComponent());L->SetMobility(EComponentMobility::Movable);L->SetIntensityUnits(ELightUnits::Candelas);L->SetIntensity(1);L->SetSourceRadius(8);L->SetAttenuationRadius(450);L->SetLightColor(FLinearColor(1,.76f,.48f));
    auto* PP=W->SpawnActor<APostProcessVolume>();PP->bUnbound=true;PP->Settings.bOverride_ColorSaturation=true;PP->Settings.ColorSaturation=FVector4(1,1,1,.94f);PP->Settings.bOverride_BloomIntensity=true;PP->Settings.BloomIntensity=.12f;
    // A few garden trees seen through the windows give the room an actual exterior view.
    if(auto* Ground=Art(W,LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Cube.Cube")),FVector(0,0,-21),TEXT("WorldRealismGarden")))
    {Ground->SetActorScale3D(FVector(60,60,.4f));if(auto* Grass=LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/Adventure/Scenery/Materials/Grass_World.Grass_World")))Ground->GetStaticMeshComponent()->SetMaterial(0,Grass);}
    auto* Tree=ArtMesh(TEXT("LeafTree"));for(float X:{-420.f,300.f})Art(W,Tree,FVector(X,1050,-10),TEXT("WorldRealismGarden"));
    Art(W,Tree,FVector(-250,-1100,-10),TEXT("WorldRealismGarden"));
}
void UpgradeCheweenieNeighbourhood(UWorld* W,const FVector& Park)
{
    if(LegacyWorld()||!ArtMesh(TEXT("LeafTree")))return;
    TMap<FName,TObjectPtr<UMaterialInterface>> M;
    for(const TCHAR* Name:{TEXT("Grass"),TEXT("Brick"),TEXT("Concrete"),TEXT("Asphalt"),TEXT("RoofSlate"),TEXT("Bark"),TEXT("Paint"),TEXT("BluePaint")})
        M.Add(Name,LoadObject<UMaterialInterface>(nullptr,*FString::Printf(TEXT("/Game/Adventure/Scenery/Materials/%s_World.%s_World"),Name,Name)));
    TArray<TPair<FVector,FRotator>> Trees,Benches;
    TArray<FTransform> Gables,Trim;
    auto* Glass=LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/Adventure/Scenery/Materials/WindowGlass.WindowGlass"));
    for(TActorIterator<AStaticMeshActor> It(W);It;++It)
    {
        if(It->ActorHasTag(TEXT("WorldRealismOutdoor")))return;auto* C=It->GetStaticMeshComponent();const FVector P=It->GetActorLocation();if(P.Size2D()<3000)continue;
        auto Tagged=[&](const TCHAR* T){return It->ActorHasTag(FName(T));};FName Mat;
        if(Tagged(TEXT("Attached brick garage")))for(float Side:{-1.f,1.f})Gables.Add(FTransform(FRotator::ZeroRotator,P+FVector(0,Side*325,165),FVector(750,8,160)));
        if(Tagged(TEXT("Headquarters brick main wing")))for(float Side:{-1.f,1.f})Gables.Add(FTransform(FRotator::ZeroRotator,P+FVector(0,Side*390,300),FVector(620,8,122)));
        if(Tagged(TEXT("Neighbour house")))for(float Side:{-1.f,1.f})Gables.Add(FTransform(FRotator(0,90,0),P+FVector(Side*350,0,230),FVector(600,8,163)));
        if(Tagged(TEXT("House window"))||Tagged(TEXT("School window"))||Tagged(TEXT("Bay glazing")))
        {
            if(Glass)C->SetMaterial(0,Glass);const FVector Size=It->GetActorScale3D()*100;
            for(float Side:{-1.f,1.f})
            {Trim.Add(FTransform(FRotator::ZeroRotator,P+FVector(Side*(Size.X*.5f+2),0,0),FVector(5,23,Size.Z+10)/100));Trim.Add(FTransform(FRotator::ZeroRotator,P+FVector(0,0,Side*(Size.Z*.5f+2)),FVector(Size.X+10,23,5)/100));}
            Trim.Add(FTransform(FRotator::ZeroRotator,P,FVector(4,25,Size.Z)/100));
        }
        if(Tagged(TEXT("Tree trunk"))){Trees.Add({P-FVector(0,0,180),It->GetActorRotation()});HideSurface(C);}
        else if(Tagged(TEXT("Tree crown")))HideSurface(C);
        else if(Tagged(TEXT("Park bench"))){Benches.Add({P-FVector(0,0,50),It->GetActorRotation()});HideSurface(C);}
        else if(Tagged(TEXT("Neighbourhood ground"))||Tagged(TEXT("Private mapped ground"))||Tagged(TEXT("Park lawn"))||Tagged(TEXT("Map edge hedge"))||Tagged(TEXT("Neighbourhood edge hedge")))Mat=TEXT("Grass");
        else if(Tagged(TEXT("Neighbour house"))||Tagged(TEXT("Headquarters brick main wing"))||Tagged(TEXT("Attached brick garage"))||Tagged(TEXT("Two-storey bay"))||Tagged(TEXT("School exterior")))Mat=TEXT("Brick");
        else if(Tagged(TEXT("Pitched roof"))||Tagged(TEXT("Garage gable roof"))||Tagged(TEXT("Main pitched roof"))||Tagged(TEXT("School roof")))Mat=TEXT("RoofSlate");
        else if(Tagged(TEXT("Blue double garage door"))||Tagged(TEXT("Blue front door"))||Tagged(TEXT("House door")))Mat=TEXT("BluePaint");
        else if(Tagged(TEXT("Garage cream surround"))||Tagged(TEXT("Bay cream surround"))||Tagged(TEXT("Window centre mullion")))Mat=TEXT("Paint");
        if(!Mat.IsNone()&&M.FindRef(Mat))C->SetMaterial(0,M.FindRef(Mat));
    }
    for(auto T:Trees)Art(W,ArtMesh(TEXT("LeafTree")),T.Key,TEXT("WorldRealismOutdoor"),T.Value);
    if(auto* Ground=Art(W,LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Cube.Cube")),FVector(Park.X,Park.Y,-65),TEXT("WorldRealismBackdrop")))
    {Ground->SetActorScale3D(FVector(2000,2000,1));auto* C=Ground->GetStaticMeshComponent();C->SetMaterial(0,M.FindRef(TEXT("Grass")));C->SetCastShadow(false);C->SetAffectDistanceFieldLighting(false);}
    for(auto B:Benches)Art(W,ArtMesh(TEXT("ParkBench")),B.Key,TEXT("WorldRealismOutdoor"),B.Value);
    for(auto T:Gables)if(auto* A=Art(W,ArtMesh(TEXT("RoofGable")),T.GetLocation(),TEXT("WorldRealismOutdoor"),T.Rotator())){A->SetActorScale3D(T.GetScale3D());A->GetStaticMeshComponent()->SetMaterial(0,M.FindRef(TEXT("Brick")));}
    if(!Trim.IsEmpty())
    {auto* A=W->SpawnActor<AActor>();A->Tags.Add(TEXT("WorldRealismOutdoor"));auto* C=NewObject<UInstancedStaticMeshComponent>(A);A->SetRootComponent(C);C->SetStaticMesh(LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Cube.Cube")));C->SetMaterial(0,M.FindRef(TEXT("Paint")));C->SetCollisionEnabled(ECollisionEnabled::NoCollision);C->RegisterComponent();for(auto T:Trim)C->AddInstance(T,true);}
    // Shared mesh/material draw batches with a short cull range; no grass actor ticks/collision.
    auto* Grass=ArtMesh(TEXT("GrassClump"));if(Grass)
    {
        auto* A=W->SpawnActor<AActor>();A->Tags.Add(TEXT("WorldRealismOutdoor"));auto* C=NewObject<UInstancedStaticMeshComponent>(A);A->SetRootComponent(C);C->SetStaticMesh(Grass);C->SetCollisionEnabled(ECollisionEnabled::NoCollision);C->SetCullDistances(1200,2000);C->SetCastShadow(false);C->RegisterComponent();
        FRandomStream Random(1717);for(int32 I=0;I<640;++I){const FVector P=Park+FVector(Random.FRandRange(-1080,1080),Random.FRandRange(-830,830),3);C->AddInstance(FTransform(FRotator(0,Random.FRandRange(0,360),0),P,FVector(Random.FRandRange(.65f,1.1f))),true);}
    }
}
