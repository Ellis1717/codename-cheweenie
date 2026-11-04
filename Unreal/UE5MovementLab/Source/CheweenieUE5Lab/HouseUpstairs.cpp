#include "HouseUpstairs.h"
#include "Engine/World.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/StaticMesh.h"
#include "Engine/PointLight.h"
#include "EngineUtils.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

void CheweenieHouse::Build(UWorld* World)
{
    for(TActorIterator<AStaticMeshActor> It(World);It;++It)if(It->ActorHasTag(TEXT("UpstairsFloor")))return;
    // RoomShell combines walls and ceiling in one retained imported mesh. Replace its
    // presentation locally with matching walls and a split ceiling around the stairwell.
    for(TActorIterator<AStaticMeshActor> It(World);It;++It)
        if(const UStaticMesh* Mesh=It->GetStaticMeshComponent()->GetStaticMesh();Mesh&&Mesh->GetName()==TEXT("RoomShell"))
        {It->SetActorHiddenInGame(true);It->GetStaticMeshComponent()->SetCastShadow(false);}
    auto* Base=LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/Adventure/World/NeighbourhoodSurface.NeighbourhoodSurface"));
    auto* Cube=LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Cube.Cube"));
    TArray<UMaterialInstanceDynamic*> Palette;
    auto* MaterialOwner=World->SpawnActor<AActor>();
    for(FLinearColor Color:{FLinearColor(.83f,.77f,.64f),FLinearColor(.32f,.17f,.075f),FLinearColor(.94f,.91f,.8f),FLinearColor(.12f,.36f,.38f),FLinearColor(.42f,.25f,.4f),FLinearColor(.78f,.87f,.87f),FLinearColor(.06f,.1f,.12f),FLinearColor(.85f,.59f,.18f)})
    {auto* M=UMaterialInstanceDynamic::Create(Base,MaterialOwner);M->SetVectorParameterValue(TEXT("Color"),Color);Palette.Add(M);}
    auto Box=[&](const TCHAR* Tag,FVector P,FVector Size,int32 Color,bool Solid=true)
    {
        auto* A=World->SpawnActor<AStaticMeshActor>(P,FRotator::ZeroRotator);A->Tags.Add(Tag);
        auto* C=A->GetStaticMeshComponent();C->SetMobility(EComponentMobility::Movable);C->SetStaticMesh(Cube);C->SetMaterial(0,Palette[Color]);A->SetActorScale3D(Size/100);
        C->SetCollisionProfileName(Solid?TEXT("BlockAll"):TEXT("NoCollision"));C->SetCanEverAffectNavigation(false);return A;
    };
    // Reproduce original window and front-door openings; original downstairs floor/art stay.
    Box(TEXT("HouseGroundWall"),FVector(500,0,180),FVector(20,1020,360),0);
    for(auto Pair:TArray<TPair<FVector,FVector>>{
        {FVector(-180,500,180),FVector(640,20,360)},{FVector(420,500,180),FVector(160,20,360)},
        {FVector(230,500,49),FVector(180,20,98)},{FVector(230,500,334),FVector(180,20,52)},
        {FVector(-500,-385,180),FVector(20,230,360)},{FVector(-500,180,180),FVector(20,640,360)},{FVector(-500,-200,310),FVector(20,140,100)},
        {FVector(-355,-500,180),FVector(290,20,360)},{FVector(355,-500,180),FVector(290,20,360)},
        {FVector(0,-500,44),FVector(420,20,88)},{FVector(0,-500,337),FVector(420,20,46)}})
        Box(TEXT("HouseGroundWall"),Pair.Key,Pair.Value,0,false);
    // The upper slabs double as the downstairs ceiling. Stair opening: x -350..410,
    // y -480..-340. All 38 risers are 10 cm, below Marty's 12 cm step limit.
    Box(TEXT("UpstairsFloor"),FVector(0,70,370),FVector(960,820,20),1);
    Box(TEXT("UpstairsFloor"),FVector(-420,-410,370),FVector(120,140,20),1);
    Box(TEXT("UpstairsFloor"),FVector(445,-410,370),FVector(70,140,20),1);
    for(int32 I=0;I<38;++I)
    {
        const float X=-340+I*20,Height=(I+1)*10;
        Box(TEXT("HouseStair"),FVector(X,-420,Height/2),FVector(20,110,Height),1);
        Box(TEXT("HouseStairTread"),FVector(X,-420,Height+.25f),FVector(19,109,.5f),2,false);
        if(I%3==0)Box(TEXT("HouseStairRail"),FVector(X,-352,Height+45),FVector(5,5,90),2);
    }
    auto* Handrail=Box(TEXT("HouseStairRail"),FVector(30,-352,280),FVector(850,7,7),1);
    Handrail->SetActorRotation(FRotator(26.565f,0,0));
    // Guard the full stairwell edge; only the east landing is open for entry/exit.
    Box(TEXT("HouseLandingRail"),FVector(-30,-338,425),FVector(800,10,90),2);
    Box(TEXT("HouseLandingRail"),FVector(-355,-410,425),FVector(10,140,90),2);
    // Solid exterior, with bright inset glazing and timber window trim.
    for(float X:{-490.f,490.f})Box(TEXT("HouseUpperWall"),FVector(X,0,520),FVector(20,1000,280),0);
    for(float Y:{-490.f,490.f})Box(TEXT("HouseUpperWall"),FVector(0,Y,520),FVector(1000,20,280),0);
    Box(TEXT("HouseUpperCeiling"),FVector(0,0,670),FVector(1000,1000,20),2);
    for(float X:{-300.f,300.f})
    {
        Box(TEXT("HouseUpperWindow"),FVector(X,478,535),FVector(150,3,130),5,false);
        for(float Side:{-1.f,1.f})Box(TEXT("HouseWindowTrim"),FVector(X+Side*80,474,535),FVector(8,10,150),2,false);
        for(float Z:{465.f,605.f})Box(TEXT("HouseWindowTrim"),FVector(X,474,Z),FVector(168,10,8),2,false);
        Box(TEXT("HouseWindowTrim"),FVector(X,472,535),FVector(5,10,140),2,false);
        for(float Side:{-1.f,1.f})for(int32 Fold=0;Fold<4;++Fold)Box(TEXT("HouseCurtain"),FVector(X+Side*(90+Fold*7),468+Fold%2*3,528),FVector(10,8,170),3,false);
    }
    // Central hall. Bedroom doors are permanently open, 130 cm wide with headers.
    for(float X:{-90.f,90.f})
    {
        Box(TEXT("HouseBedroomPartition"),FVector(X,-85,520),FVector(14,70,280),0);
        Box(TEXT("HouseBedroomPartition"),FVector(X,295,520),FVector(14,370,280),0);
        Box(TEXT("HouseDoorHeader"),FVector(X,30,640),FVector(14,160,40),2);
        for(float Y:{-50.f,110.f})Box(TEXT("HouseDoorTrim"),FVector(X,Y,500),FVector(20,7,240),2);
    }
    // Each bedroom has a full-size bed, pillows, blanket, rug, nightstands and wardrobe.
    for(int32 Room=0;Room<2;++Room)
    {
        const float X=Room==0?-290.f:290.f;const int32 Cloth=Room==0?3:4;
        Box(TEXT("HouseBedroomRug"),FVector(X,60,381),FVector(300,300,2),Cloth,false);
        Box(TEXT("HouseBedFrame"),FVector(X,120,400),FVector(170,220,40),1);
        Box(TEXT("HouseMattress"),FVector(X,120,427),FVector(164,214,18),2);
        Box(TEXT("HouseBlanket"),FVector(X,72,439),FVector(166,124,8),Cloth,false);
        for(float Side:{-1.f,1.f})
        {
            Box(TEXT("HousePillow"),FVector(X+Side*40,190,442),FVector(65,42,12),2,false);
            Box(TEXT("HouseNightstand"),FVector(X+Side*115,200,405),FVector(42,42,50),1);
            Box(TEXT("HouseLampStem"),FVector(X+Side*115,200,449),FVector(5,5,38),7,false);
            Box(TEXT("HouseLampShade"),FVector(X+Side*115,200,470),FVector(28,28,25),2,false);
        }
        Box(TEXT("HouseHeadboard"),FVector(X,234,442),FVector(180,12,124),1);
        Box(TEXT("HouseWardrobe"),FVector(Room==0?-410:410,370,475),FVector(96,140,190),1);
        for(float Y:{340.f,400.f})Box(TEXT("HouseWardrobeHandle"),FVector(Room==0?-359:359,Y,475),FVector(5,5,20),7,false);
        Box(TEXT("HouseWallArt"),FVector(Room==0?-477:477,120,550),FVector(3,110,70),Cloth,false);
    }
    // Bathroom at the north end of the hall, reached through an open doorway.
    for(float X:{-170.f,170.f})Box(TEXT("HouseBathroomWall"),FVector(X,300,520),FVector(160,14,280),0);
    Box(TEXT("HouseBathroomHeader"),FVector(0,300,640),FVector(180,14,40),2);
    Box(TEXT("HouseBathroomTiles"),FVector(0,390,382),FVector(170,170,4),5);
    Box(TEXT("HouseBathVanity"),FVector(-48,440,411),FVector(60,55,62),3);
    Box(TEXT("HouseBathSink"),FVector(-48,438,445),FVector(64,59,8),2);
    Box(TEXT("HouseBathMirror"),FVector(-48,475,525),FVector(65,3,90),5,false);
    Box(TEXT("HouseBathToilet"),FVector(48,443,403),FVector(44,50,46),2);
    Box(TEXT("HouseBathCistern"),FVector(48,465,435),FVector(44,18,65),2);
    Box(TEXT("HouseBathMat"),FVector(0,350,385),FVector(100,60,2),3,false);
    // A pet nook at the south landing leaves the main walking lane clear.
    Box(TEXT("HousePetBlanket"),FVector(-270,-245,384),FVector(140,80,8),3,false);
    Box(TEXT("HousePetCushion"),FVector(-310,-245,394),FVector(45,65,16),7,false);
    for(FVector P:{FVector(-270,60,610),FVector(270,60,610),FVector(0,-210,610),FVector(0,400,605)})
    {
        auto* Light=World->SpawnActor<APointLight>(P,FRotator::ZeroRotator);Light->Tags.Add(TEXT("HouseUpperLight"));auto* C=Cast<UPointLightComponent>(Light->GetLightComponent());
        C->SetMobility(EComponentMobility::Movable);C->SetIntensityUnits(ELightUnits::Candelas);C->SetIntensity(3);C->SetAttenuationRadius(650);C->SetSourceRadius(20);C->SetCastShadows(false);C->SetLightColor(FLinearColor(1,.85f,.68f));
    }
}

FString CheweenieHouse::Location(FVector P)
{
    if(!Upstairs(P))return TEXT("Headquarters / staircase");
    if(P.Y>310&&FMath::Abs(P.X)<90)return TEXT("Upstairs / bathroom");
    if(P.Y> -120&&P.X< -90)return TEXT("Upstairs / main bedroom");
    if(P.Y> -120&&P.X>90)return TEXT("Upstairs / guest bedroom");
    return TEXT("Upstairs / landing + pet nook");
}
