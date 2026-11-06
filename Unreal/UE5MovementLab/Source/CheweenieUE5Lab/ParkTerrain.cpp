#include "ParkSports.h"
#include "ProceduralMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/World.h"

namespace
{
    constexpr float Step=100, MinX=-3500, MinY=800, MaxX=3500, MaxY=7800;
    float Smooth(float T){T=FMath::Clamp(T,0.f,1.f);return T*T*(3-2*T);}
    float Hill(float X,float Y,float CX,float CY,float Radius)
    {
        const float R=FVector2D(X-CX,Y-CY).Size()/Radius;
        return R<1?FMath::Square(1-R*R):0;
    }
    TArray<FVector> Clip(TArray<FVector> Polygon,int Axis,float Side,float Half)
    {
        TArray<FVector> Out;if(Polygon.IsEmpty())return Out;
        FVector Previous=Polygon.Last();float Before=Half-Side*Previous[Axis];
        for(FVector Current:Polygon)
        {
            const float After=Half-Side*Current[Axis];
            if((Before>=0)!=(After>=0))Out.Add(FMath::Lerp(Previous,Current,Before/(Before-After)));
            if(After>=0)Out.Add(Current);
            Previous=Current;Before=After;
        }
        return Out;
    }
}

float UParkSports::TerrainVertex(float X,float Y) const
{
    // Smoothly meet the old park at the perimeter; no steps or holes between layers.
    const float Edge=Smooth(FMath::Min(FMath::Min(X-MinX,MaxX-X),FMath::Min(Y-MinY,MaxY-Y))/450);
    float Z=7+14*Hill(X,Y,-1600,3500,1400)+22*Hill(X,Y,1700,3900,1500);
    Z+=36*Hill(X,Y,1700,3400,240); // A broad, walkable pitcher's mound.
    Z+=180*Hill(X,Y,-2400,5800,1100)+260*Hill(X,Y,2400,6100,1100);
    // A level court on a terrace; broad ramps blend it into the surrounding park.
    const float CourtDistance=FVector2D(FMath::Max(0.f,FMath::Abs(X)-1000),FMath::Max(0.f,FMath::Abs(Y-6400)-900)).Size();
    Z=FMath::Lerp(Z,90.f,1-Smooth(CourtDistance/650));
    return TerrainOrigin.Z+1+Z*Edge;
}

float UParkSports::GroundHeight(FVector P) const
{
    const float X=P.X-TerrainOrigin.X,Y=P.Y-TerrainOrigin.Y;
    if(X<MinX||X>MaxX||Y<MinY||Y>MaxY)return TerrainOrigin.Z+1;
    const float X0=FMath::Min(MaxX-Step,MinX+FMath::FloorToFloat((X-MinX)/Step)*Step);
    const float Y0=FMath::Min(MaxY-Step,MinY+FMath::FloorToFloat((Y-MinY)/Step)*Step);
    const float U=(X-X0)/Step,V=(Y-Y0)/Step;
    const float A=TerrainVertex(X0,Y0),B=TerrainVertex(X0+Step,Y0),C=TerrainVertex(X0,Y0+Step),D=TerrainVertex(X0+Step,Y0+Step);
    return U+V<=1?A+(B-A)*U+(C-A)*V:D+(C-D)*(1-U)+(B-D)*(1-V);
}
FVector UParkSports::GroundPoint(FVector P) const{P.Z=GroundHeight(P);return P;}

void UParkSports::Surface(FString Name,FVector Center,FVector2D Size,FLinearColor Color,float Yaw,float Lift)
{
    auto* A=GetWorld()->SpawnActor<AActor>();A->Tags.Add(FName(*Name));
    auto* Mesh=NewObject<UProceduralMeshComponent>(A);A->SetRootComponent(Mesh);Mesh->RegisterComponent();A->SetActorLocation(Center);
    const int NX=FMath::CeilToInt(Size.X/Step),NY=FMath::CeilToInt(Size.Y/Step);
    TArray<FVector> V,Normals;TArray<int32> Indices;TArray<FVector2D> UV;TArray<FProcMeshTangent> Tangents;
    const FRotator Rotation(0,Yaw,0);
    if(Lift>0)
    {
        // Clip the actual ground triangles to each field/path rectangle. Resampling a rotated
        // patch can cut beneath a convex mound even with a fine grid, exposing grass through clay.
        auto Point=[&](float X,float Y){return Rotation.UnrotateVector(FVector(TerrainOrigin.X+X,TerrainOrigin.Y+Y,TerrainVertex(X,Y))-Center);};
        auto Triangle=[&](FVector A0,FVector B,FVector C)
        {
            TArray<FVector> Polygon={A0,B,C};
            for(int Axis=0;Axis<2;++Axis)for(float Side:{-1.f,1.f})Polygon=Clip(MoveTemp(Polygon),Axis,Side,Size[Axis]*.5f);
            if(Polygon.Num()<3)return;
            const int First=V.Num();
            for(FVector P:Polygon){V.Add(Rotation.RotateVector(P)+FVector(0,0,Lift));UV.Add(FVector2D(P.X/Size.X+.5f,P.Y/Size.Y+.5f));}
            for(int I=1;I+1<Polygon.Num();++I)Indices.Append({First,First+I,First+I+1});
        };
        for(float Y=MinY;Y<MaxY;Y+=Step)for(float X=MinX;X<MaxX;X+=Step)
        {
            const FVector A0=Point(X,Y),B=Point(X+Step,Y),C=Point(X,Y+Step),D=Point(X+Step,Y+Step);
            Triangle(A0,C,B);Triangle(B,C,D);
        }
    }
    else
    {
        for(int J=0;J<=NY;++J)for(int I=0;I<=NX;++I)
        {
            const FVector P=Center+FVector((float(I)/NX-.5f)*Size.X,(float(J)/NY-.5f)*Size.Y,0);
            // Local vertices preserve precision at the outdoor world's distant anchor.
            V.Add(GroundPoint(P)-Center);UV.Add(FVector2D(float(I)/NX,float(J)/NY));
        }
        for(int J=0;J<NY;++J)for(int I=0;I<NX;++I)
        {
            const int A0=J*(NX+1)+I,B=A0+1,C=A0+NX+1,D=C+1;
            Indices.Append({A0,C,B,B,C,D}); // Unreal clockwise faces; normals point up.
        }
    }
    UKismetProceduralMeshLibrary::CalculateTangentsForMesh(V,Indices,UV,Normals,Tangents);
    const bool Solid=Lift==0;
    Mesh->bUseComplexAsSimpleCollision=true;Mesh->bUseAsyncCooking=false;
    Mesh->CreateMeshSection_LinearColor(0,V,Indices,Normals,UV,TArray<FLinearColor>(),Tangents,Solid);
    Mesh->SetCollisionProfileName(Solid?TEXT("BlockAll"):TEXT("NoCollision"));
    auto* Material=LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/Adventure/World/NeighbourhoodSurface.NeighbourhoodSurface"));
    if(!Material)Material=LoadObject<UMaterialInterface>(nullptr,TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
    if(Name==TEXT("Contoured park terrain")||Name==TEXT("Soccer pitch"))
        if(auto* Grass=LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/Adventure/Scenery/Materials/Grass_World.Grass_World")))Material=Grass;
    auto* M=UMaterialInstanceDynamic::Create(Material,A);M->SetVectorParameterValue(TEXT("Color"),Color);Mesh->SetMaterial(0,M);
}
void UParkSports::BuildTerrain()
{
    Surface(TEXT("Contoured park terrain"),TerrainOrigin+FVector(0,4300,0),FVector2D(7000,7000),FLinearColor(.12f,.32f,.10f));
    Surface(TEXT("Park sports path"),TerrainOrigin+FVector(0,3300,0),FVector2D(260,4600),FLinearColor(.48f,.44f,.34f),0,.8f);
    Surface(TEXT("Soccer pitch"),Soccer,FVector2D(2200,1800),FLinearColor(.09f,.25f,.08f),0,.8f);
    Surface(TEXT("Baseball infield"),Diamond+FVector(0,-100,0),FVector2D(1500,1500),FLinearColor(.45f,.23f,.10f),45,.8f);
    // The mound is part of the terrain and infield, without a second overlapping clay skin.
    Surface(TEXT("Basketball half court"),Court,FVector2D(1800,1600),FLinearColor(.08f,.23f,.36f),0,.8f);
    for(int Side:{-1,1})
    {
        const FVector Crest=GroundPoint(TerrainOrigin+FVector(Side*2400,Side<0?5800:6100,0));
        Label(Side<0?TEXT("PARK VIEW\nWalk up the grassy slope"):TEXT("OUTFIELD HILL\nFollow the gentle rise"),Crest+FVector(0,0,130),25);
        Shape(TEXT("Hilltop bench seat"),Crest+FVector(0,0,40),FVector(210,50,12),FLinearColor(.35f,.20f,.09f),true);
        for(int S:{-1,1})Shape(TEXT("Hilltop bench leg"),GroundPoint(Crest+FVector(S*75,0,0))+FVector(0,0,18),FVector(12,35,36),FLinearColor(.22f,.23f,.22f),true);
    }
}
