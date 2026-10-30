#include "Neighbourhood.h"
#include "MartyCharacter.h"
#include "CozyAdventure.h"
#include "MartyGait.h"
#include "DogLife.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Engine/DirectionalLight.h"
#include "Components/DirectionalLightComponent.h"
#include "EngineUtils.h"
#include "Components/TextRenderComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/FileHelper.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"

void UCheweenieNeighbourhood::BeginPlay(){Super::BeginPlay();Build();}
AStaticMeshActor* UCheweenieNeighbourhood::Shape(const FString& Name,FVector P,FVector Size,int32 Color,bool Solid,const TCHAR* Mesh,float Yaw)
{
    auto* Actor=GetWorld()->SpawnActor<AStaticMeshActor>(P,FRotator(0,Yaw,0));
    auto* C=Actor->GetStaticMeshComponent();C->SetMobility(EComponentMobility::Movable);
    C->SetStaticMesh(LoadObject<UStaticMesh>(nullptr,Mesh));Actor->SetActorScale3D(Size/100);
    C->SetMaterial(0,Palette[Color%Palette.Num()]);C->SetCollisionEnabled(Solid?ECollisionEnabled::QueryAndPhysics:ECollisionEnabled::NoCollision);
    if(Solid)C->SetCollisionProfileName(TEXT("BlockAll"));Actor->Tags.Add(FName(Name));return Actor;
}
void UCheweenieNeighbourhood::Sign(const FString& Text,FVector P,float Size,float Yaw)
{
    auto* Actor=GetWorld()->SpawnActor<AActor>(P,FRotator(0,Yaw,0));auto* Label=NewObject<UTextRenderComponent>(Actor);
    Actor->SetRootComponent(Label);Label->RegisterComponent();Label->SetText(FText::FromString(Text));
    Actor->SetActorLocationAndRotation(P,FRotator(0,Yaw,0));
    Label->SetWorldSize(Size);Label->SetTextRenderColor(FColor(245,235,200));Label->SetHorizontalAlignment(EHTA_Center);
    Label->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
void UCheweenieNeighbourhood::Path(FVector A,FVector B,float Width,int32 Color)
{
    if(!Paths.Contains(Color))
    {
        auto* C=NewObject<UInstancedStaticMeshComponent>(GetOwner());
        C->SetStaticMesh(LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Cube.Cube")));C->SetMaterial(0,Palette[Color]);
        C->SetCollisionEnabled(ECollisionEnabled::NoCollision);C->RegisterComponent();Paths.Add(Color,C);
    }
    const FVector D=B-A;Paths[Color]->AddInstance(FTransform(FRotator(0,D.Rotation().Yaw,0),(A+B)*.5f+FVector(0,0,Color==2?3.f:1.5f),FVector(D.Size2D()+12,Width,2)/100),true);
    ++RoadSegments;
}
void UCheweenieNeighbourhood::House(FVector P,int32 Slot,const FString& Label)
{
    ++Houses;const float Side=P.Y<Entrance.Y?-1:1;
    Shape(TEXT("Neighbour house"),P+FVector(0,0,230),FVector(700,600,460),Slot%2?4:3);
    // Two roof planes give a recognizable pitched silhouette without a paid house pack.
    for(int32 S:{-1,1})
    {
        auto* Roof=Shape(TEXT("Pitched roof"),P+FVector(0,S*180,520),FVector(790,440,35),5,false);
        Roof->SetActorRotation(FRotator(0,0,S*28));
    }
    const FVector Door=P+FVector(0,-Side*306,110);
    Shape(TEXT("House door"),Door,FVector(110,12,220),6,false);
    for(int32 S:{-1,1})Shape(TEXT("House window"),Door+FVector(S*230,0,90),FVector(115,14,130),7,false);
    Sign(Label,P+FVector(0,-Side*335,300),32,Side>0?-90:90);
    Path(P+FVector(0,-Side*320,0),P+FVector(0,-Side*950,0),160,1);
}
void UCheweenieNeighbourhood::Dog(const FNeighbourDog& D,int32 Index)
{
    // Explicit schematic neighbours, not likeness claims or copies of Marty.
    const int32 Color=Palette.Num();auto* Mat=UMaterialInstanceDynamic::Create(LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/Adventure/World/NeighbourhoodSurface.NeighbourhoodSurface")),this);
    Mat->SetVectorParameterValue(TEXT("Color"),D.Coat);Palette.Add(Mat);
    const TCHAR* Ball=TEXT("/Engine/BasicShapes/Sphere.Sphere");
    Shape(TEXT("Neighbour dog"),D.Position+FVector(0,0,30),FVector(60,25,28),Color,false,Ball);
    Shape(TEXT("Dog head"),D.Position+FVector(-26,0,47),FVector(27,28,29),Color,false,Ball);
    for(int32 S:{-1,1})
    {
        Shape(TEXT("Dog ear"),D.Position+FVector(-25,S*16,42),FVector(15,7,27),Color,false,Ball);
        for(int32 X:{-1,1})Shape(TEXT("Dog leg"),D.Position+FVector(X*19,S*9,14),FVector(9,9,25),Color,false,Ball);
    }
    Shape(TEXT("Dog nose"),D.Position+FVector(-42,0,43),FVector(9,13,8),5,false,Ball);
    Sign(D.Name,D.Position+FVector(0,0,100),24,180);
}
void UCheweenieNeighbourhood::Home()
{
    ++Houses;const FVector P=Entrance-FVector(0,0,28);
    // Original approximation of the retained reference: red brick, blue garage, cream trim,
    // two-storey bay and a sheltered entry. No civic number, photograph or vehicle is embedded.
    Shape(TEXT("Headquarters brick main wing"),P+FVector(200,435,300),FVector(620,780,600),3);
    Shape(TEXT("Attached brick garage"),P+FVector(-550,25,165),FVector(750,650,330),3);
    Shape(TEXT("Garage cream surround"),P+FVector(-550,-307,145),FVector(660,16,290),1,false);
    Shape(TEXT("Blue double garage door"),P+FVector(-550,-318,135),FVector(610,12,260),6,false);
    for(int32 I=1;I<5;++I)Shape(TEXT("Garage door panel"),P+FVector(-550,-326,I*50),FVector(590,4,3),4,false);
    Shape(TEXT("Blue front door"),P+FVector(0,35,110),FVector(100,12,220),6,false);
    Shape(TEXT("Door glass"),P+FVector(0,26,145),FVector(55,7,80),7,false);
    Shape(TEXT("Entry canopy"),P+FVector(0,-10,250),FVector(200,200,25),5,false);
    Shape(TEXT("Two-storey bay"),P+FVector(370,15,300),FVector(270,100,570),3);
    for(int32 Z:{160,450})
    {
        Shape(TEXT("Bay cream surround"),P+FVector(370,-43,Z),FVector(240,14,175),1,false);
        Shape(TEXT("Bay glazing"),P+FVector(370,-53,Z),FVector(210,10,145),7,false);
        Shape(TEXT("Window centre mullion"),P+FVector(370,-61,Z),FVector(7,6,145),1,false);
    }
    for(int32 Side:{-1,1})
    {
        auto* R=Shape(TEXT("Garage gable roof"),P+FVector(-550+Side*195,25,390),FVector(470,750,30),5,false);R->SetActorRotation(FRotator(Side*25,0,0));
        R=Shape(TEXT("Main pitched roof"),P+FVector(200+Side*165,435,650),FVector(420,890,30),5,false);R->SetActorRotation(FRotator(Side*20,0,0));
    }
    Path(P+FVector(0,-50,0),P+FVector(0,-650,0),150,1);
    Sign(TEXT("Marty's headquarters"),P+FVector(0,-100,290),27,-90);
}
void UCheweenieNeighbourhood::LoadDogs()
{
    FString File,Json;FParse::Value(FCommandLine::Get(),TEXT("CheweenieDogs="),File);
    TSharedPtr<FJsonObject> Data;
    if(File.IsEmpty()||!FFileHelper::LoadFileToString(Json,*File)||Json.Len()>65536||!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Json),Data)||!Data)return;
    const TArray<TSharedPtr<FJsonValue>>* Items=nullptr;if(!Data->TryGetArrayField(TEXT("dogs"),Items)||Items->Num()>8)return;
    for(const auto& Value:*Items)
    {
        auto Entry=Value->AsObject();if(!Entry)continue;int32 Slot=0;if(!Entry->TryGetNumberField(TEXT("slot"),Slot)||!Dogs.IsValidIndex(Slot))continue;
        FString Name,HouseLabel;if(Entry->TryGetStringField(TEXT("name"),Name)&&!Name.IsEmpty())Dogs[Slot].Name=Name.Left(40);
        if(Entry->TryGetStringField(TEXT("house"),HouseLabel)&&!HouseLabel.IsEmpty())Dogs[Slot].House=HouseLabel.Left(40);
    }
}
bool UCheweenieNeighbourhood::LoadMap()
{
    if(FParse::Param(FCommandLine::Get(),TEXT("CheweenieOutsideSmoke"))&&!FParse::Param(FCommandLine::Get(),TEXT("CheweenieOutsideMapAudit")))return false;
    FString File,Json;FParse::Value(FCommandLine::Get(),TEXT("CheweenieNeighbourhood="),File);
    TSharedPtr<FJsonObject> Data;
    if(File.IsEmpty()||!FFileHelper::LoadFileToString(Json,*File)||Json.Len()>4000000||!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Json),Data)||!Data)return false;
    const TArray<TSharedPtr<FJsonValue>> *Roads=nullptr,*Stops=nullptr;
    if(!Data->TryGetArrayField(TEXT("roads"),Roads)||Roads->IsEmpty()||Roads->Num()>512||!Data->TryGetArrayField(TEXT("stops"),Stops))return false;
    auto Point=[&](const TSharedPtr<FJsonObject>& O,FVector& Out){double X=0,Y=0;if(!O||!O->TryGetNumberField(TEXT("x"),X)||!O->TryGetNumberField(TEXT("z"),Y)||!FMath::IsFinite(X)||!FMath::IsFinite(Y)||FMath::Abs(X)>2500||FMath::Abs(Y)>2500)return false;Out=Origin+FVector(X*100,Y*100,0);return true;};
    struct FRoad {FString Name;float Width;TArray<FVector> Points;};TArray<FRoad> Validated;FBox Bounds(ForceInit);Bounds+=Origin;int32 Count=0;
    for(const auto& Value:*Roads)
    {
        auto O=Value->AsObject();const TArray<TSharedPtr<FJsonValue>>* Points=nullptr;double Width=0;FRoad R;
        if(!O||!O->TryGetStringField(TEXT("name"),R.Name)||!O->TryGetNumberField(TEXT("width"),Width)||Width<2||Width>20||!O->TryGetArrayField(TEXT("points"),Points)||Points->Num()<2)return false;
        R.Width=Width*100;
        for(const auto& P:*Points){FVector V;if(!Point(P->AsObject(),V)||++Count>16000)return false;R.Points.Add(V);Bounds+=V;}
        Validated.Add(R);
    }
    for(const auto& Value:*Stops)
    {
        auto O=Value->AsObject();FVector P;int32 Id=0;if(!Point(O,P)||!O->TryGetNumberField(TEXT("stop"),Id))return false;
        Bounds+=P;Tour.Add(P);
        if(Id==4)Park=P;
        if(Id==16)School=P;
    }
    const TSharedPtr<FJsonObject>* Site=nullptr;
    if(Data->TryGetObjectField(TEXT("school"),Site)){FVector P;if(Point(*Site,P)){School=P;Bounds+=P;}}
    if(Tour.IsEmpty()||Park.IsZero()||School.IsZero())return false;
    Entrance=Origin+FVector(0,-1700,28);
    const FVector Size=Bounds.GetSize()+FVector(40000,40000,0);
    Shape(TEXT("Private mapped ground"),Bounds.GetCenter()+FVector(0,0,-20),FVector(Size.X,Size.Y,40),0);
    for(int32 Side:{-1,1})
    {
        Shape(TEXT("Map edge hedge"),Bounds.GetCenter()+FVector(Side*Size.X/2,0,100),FVector(60,Size.Y,200),0);
        Shape(TEXT("Map edge hedge"),Bounds.GetCenter()+FVector(0,Side*Size.Y/2,100),FVector(Size.X,60,200),0);
    }
    for(const auto& R:Validated)
    {
        for(int32 I=1;I<R.Points.Num();++I)
        {Path(R.Points[I-1],R.Points[I],R.Width+240,1);Path(R.Points[I-1],R.Points[I],R.Width,2);RoadEdges.Add({R.Points[I-1],R.Points[I]});}
        if(R.Name!=TEXT("Walking path"))Sign(R.Name.Left(60),R.Points[R.Points.Num()/2]+FVector(0,R.Width/2+120,180),32,180);
    }
    Path(Entrance,Tour[0],180,1);
    Sign(TEXT("Map data: OpenStreetMap contributors / ODbL\nRetained street layout; schematic houses and game paths"),Entrance+FVector(100,200,170),22,180);
    return true;
}
void UCheweenieNeighbourhood::Build()
{
    const FLinearColor Colors[]={FLinearColor(.16f,.34f,.12f),FLinearColor(.62f,.59f,.48f),FLinearColor(.11f,.13f,.15f),FLinearColor(.52f,.28f,.16f),FLinearColor(.30f,.43f,.54f),FLinearColor(.10f,.075f,.055f),FLinearColor(.12f,.25f,.34f),FLinearColor(.45f,.65f,.72f)};
    auto* Base=LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/Adventure/World/NeighbourhoodSurface.NeighbourhoodSurface"));
    if(!Base)Base=LoadObject<UMaterialInterface>(nullptr,TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
    for(auto C:Colors){auto* M=UMaterialInstanceDynamic::Create(Base,this);M->SetVectorParameterValue(TEXT("Color"),C);Palette.Add(M);}
    Shape(TEXT("Headquarters exit door"),FVector(-466,-200,110),FVector(12,115,220),6,false);
    Sign(TEXT("OUTSIDE\nE / X / Square"),FVector(-452,-200,225),24,0);
    PrivateMapLoaded=LoadMap();
    if(!PrivateMapLoaded)
    {
        Tour.Empty();Park=Origin+FVector(3500,2400,0);School=Origin+FVector(8200,1800,0);
        Shape(TEXT("Neighbourhood ground"),Origin+FVector(3500,0,-20),FVector(12000,8000,40),0);
        for(int32 Side:{-1,1})
        {
            Shape(TEXT("Neighbourhood edge hedge"),Origin+FVector(3500+Side*6000,0,100),FVector(60,8000,200),0);
            Shape(TEXT("Neighbourhood edge hedge"),Origin+FVector(3500,Side*4000,100),FVector(12000,60,200),0);
        }
        const FVector Points[]={Origin+FVector(500,-600,0),Origin+FVector(2300,0,0),Origin+FVector(5600,0,0),Origin+FVector(7600,600,0),Origin+FVector(8500,1800,0)};
        for(int32 I=1;I<5;++I){Path(Points[I-1],Points[I],1100,1);Path(Points[I-1],Points[I],700,2);}
        Path(Entrance,Points[0],180,1);Path(Points[1],Park,200,1);
        Tour={Park,School,Entrance};Sign(TEXT("NEIGHBOURHOOD WALK\nSchematic layout"),Origin+FVector(1500,-600,200),38,180);
    }
    // House footprints and neighbour slots remain schematic even with local street data.
    for(int32 I=0;I<8;++I)
    {
        const int32 Row=I/2;const float Side=I%2==0?-1:1;
        FNeighbourDog D;D.Name=FString::Printf(TEXT("Neighbour dog %d"),I+1);D.House=FString::Printf(TEXT("House %c (editable)"),'A'+I);
        if(auto* Mode=GetWorld()->GetAuthGameMode<AMartyPlayMode>())if(Mode->Adventure&&Mode->Adventure->Progress){D.Met=(Mode->Adventure->Progress->NeighbourDogMask&(1<<I))!=0;if(D.Met)++MetDogs;}
        D.Position=Entrance+FVector(1800+Row*1450,Side*850,-28);D.Coat=I%2?FLinearColor(.2f,.12f,.07f):FLinearColor(.64f,.43f,.18f);Dogs.Add(D);
    }
    LoadDogs();
    for(int32 I=0;I<Dogs.Num();++I)
    {
        auto& D=Dogs[I];const float Side=I%2==0?-1:1;FVector P=D.Position+FVector(0,Side*700,0);
        for(int32 Attempt=0;Attempt<12;++Attempt)
        {
            bool Clear=true;for(const auto& Edge:RoadEdges)if(FMath::PointDistToSegment(P,Edge.Key,Edge.Value)<1400){Clear=false;break;}
            if(Clear)break;P.Y+=Side*900;
        }
        D.Position=P-FVector(0,Side*700,0);House(P,I,D.House);Dog(D,I);
    }
    Home();
    Sign(TEXT("HOME\nE / X / Square to go inside"),Entrance+FVector(-120,0,150),26,0);
    // Park and school are recognizable exterior destinations with accessible approaches.
    Shape(TEXT("Park lawn"),Park+FVector(0,0,1),FVector(2300,1800,2),0,false);
    Sign(TEXT("PARK\nSniff, explore and meet your neighbours"),Park+FVector(-300,-300,180),30,90);
    for(int32 I=0;I<6;++I)
    {
        const FVector P=Park+FVector((I%3-1)*650,(I/3==0?-1:1)*650,0);
        Shape(TEXT("Tree trunk"),P+FVector(0,0,180),FVector(50,50,360),5,true,TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
        Shape(TEXT("Tree crown"),P+FVector(0,0,470),FVector(420,420,540),0,false,TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    }
    Shape(TEXT("Park bench"),Park+FVector(400,-300,50),FVector(180,50,20),3);
    Shape(TEXT("School exterior"),School+FVector(0,1000,230),FVector(2600,1200,460),3);
    Shape(TEXT("School roof"),School+FVector(0,1000,475),FVector(2700,1300,30),5,false);
    for(int32 I=-4;I<=4;++I)Shape(TEXT("School window"),School+FVector(I*240,390,250),FVector(160,12,130),7,false);
    Sign(TEXT("GLEN DHU SCHOOL\nSchematic exterior / walking destination"),School+FVector(0,370,380),32,-90);
    Sign(TEXT("WALKING TOUR\nE / X / Square: next landmark"),Entrance+FVector(160,0,100),26,180);
    Sign(TEXT("NEXT TOUR STOP\nE / X / Square"),Park+FVector(300,0,100),26,180);
    Sign(TEXT("NEXT TOUR STOP\nE / X / Square"),School+FVector(-300,0,100),26,180);
    TActorIterator<ASkyAtmosphere> Atmosphere(GetWorld());if(!Atmosphere)GetWorld()->SpawnActor<ASkyAtmosphere>();
    for(TActorIterator<ADirectionalLight> It(GetWorld());It;++It)
        if(auto* Sun=It->FindComponentByClass<UDirectionalLightComponent>())OriginalSuns.Add({Sun,Sun->Intensity});
    // Soft shadowless sky fill keeps the dog and shady front gardens readable at fixed exposure.
    // It is enabled only outdoors; the retained indoor lighting is restored on return.
    for(float Yaw:{-30.f,150.f})
    {
        auto* Actor=GetWorld()->SpawnActor<AActor>();auto* Fill=NewObject<UDirectionalLightComponent>(Actor);
        Actor->SetRootComponent(Fill);Fill->SetMobility(EComponentMobility::Movable);
        Fill->bAtmosphereSunLight=false;Fill->SetCastShadows(false);Fill->SetIntensity(.9f);
        Fill->SetLightColor(FLinearColor(.82f,.90f,1.f));Fill->SetVisibility(false);Fill->RegisterComponent();
        Actor->SetActorLocationAndRotation(Origin+FVector(0,0,1000),FRotator(-55,Yaw,0));OutdoorFill.Add(Fill);
    }
}
void UCheweenieNeighbourhood::SetOutdoorLighting(bool Active)
{
    for(auto& Fill:OutdoorFill)if(Fill)Fill->SetVisibility(Active);
    for(auto& Sun:OriginalSuns)if(Sun.Key.IsValid())Sun.Key->SetIntensity(Sun.Value*(Active?2.5f:1.f));
}
void UCheweenieNeighbourhood::Message(const FString& Text)
{
    if(auto* Mode=GetWorld()->GetAuthGameMode<AMartyPlayMode>())if(Mode->Adventure){Mode->Adventure->Message=Text;Mode->Adventure->HoldMessage(7);}
}
void UCheweenieNeighbourhood::GoOutside(AMartyCharacter* Pawn)
{
    if(Pawn->Life)Pawn->Life->CancelAction();
    Pawn->GetCharacterMovement()->StopMovementImmediately();Pawn->TeleportTo(Entrance+FVector(0,-105,0),FRotator::ZeroRotator);Outside=true;
    SetOutdoorLighting(true);
    if(Pawn->Gait)Pawn->Gait->ResetContacts();
    if(auto* PC=Cast<APlayerController>(Pawn->GetController())){PC->SetControlRotation(FRotator(-12,-90,0));PC->PlayerCameraManager->StartCameraFade(1,0,.35f,FLinearColor::Black,false);}
    Message(TEXT("Outside! Meet the dogs, explore the park and school. H always returns you home."));
}
void UCheweenieNeighbourhood::GoInside(AMartyCharacter* Pawn)
{
    Outside=false;Pawn->ReturnHome();Message(TEXT("Home sweet headquarters. Your indoor patrol progress is kept."));
}
FString UCheweenieNeighbourhood::Prompt(const AMartyCharacter* Pawn) const
{
    if(!Outside)return FVector::Dist2D(Pawn->GetActorLocation(),IndoorDoor)<130?TEXT("E / X / Square: open the door and go outside"):TEXT("Headquarters door: near the blue OUTSIDE sign");
    const FVector P=Pawn->GetActorLocation();
    if(FVector::Dist2D(P,Entrance)<120)return TEXT("E / X / Square: return inside");
    if(FVector::Dist2D(P,Entrance+FVector(160,0,0))<120||FVector::Dist2D(P,Park+FVector(300,0,0))<120||FVector::Dist2D(P,School-FVector(300,0,0))<120)return TEXT("E / X / Square: visit next landmark on walking tour");
    for(const auto& D:Dogs)if(FVector::Dist2D(P,D.Position)<135)return FString(TEXT("E / X / Square: say hello to "))+D.Name;
    return FString::Printf(TEXT("Outside | neighbours met: %d / %d | Home: H | park %.0f m | school %.0f m"),MetDogs,Dogs.Num(),FVector::Dist2D(P,Park)/100,FVector::Dist2D(P,School)/100);
}
bool UCheweenieNeighbourhood::Interact(AMartyCharacter* Pawn)
{
    const FVector P=Pawn->GetActorLocation();
    if(!Outside){if(FVector::Dist2D(P,IndoorDoor)<130){GoOutside(Pawn);return true;}return false;}
    if(FVector::Dist2D(P,Entrance)<120){GoInside(Pawn);return true;}
    if(FVector::Dist2D(P,Entrance+FVector(160,0,0))<120||FVector::Dist2D(P,Park+FVector(300,0,0))<120||FVector::Dist2D(P,School-FVector(300,0,0))<120)
    {
        const FVector Destination=Tour[NextTour++%Tour.Num()];Pawn->GetCharacterMovement()->StopMovementImmediately();Pawn->TeleportTo(Destination+FVector(0,0,40),FRotator::ZeroRotator);
        if(Pawn->Gait)Pawn->Gait->ResetContacts();Message(TEXT("Walking tour stop. Explore here, then press H to head home."));return true;
    }
    for(int32 I=0;I<Dogs.Num();++I)
    {
        auto& D=Dogs[I];if(FVector::Dist2D(P,D.Position)>=135)continue;
        if(!D.Met)
        {
            D.Met=true;++MetDogs;
            if(auto* Mode=GetWorld()->GetAuthGameMode<AMartyPlayMode>())if(Mode->Adventure&&Mode->Adventure->Progress){Mode->Adventure->Progress->NeighbourDogMask|=1<<I;Mode->Adventure->Save();}
        }
        Message(D.Name+TEXT(" greets Marty with a friendly sniff. Neighbourhood buddies!"));return true;
    }
    return false;
}
