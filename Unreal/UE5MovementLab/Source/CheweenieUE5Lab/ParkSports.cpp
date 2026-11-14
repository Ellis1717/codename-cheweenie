#include "ParkSports.h"
#include "Neighbourhood.h"
#include "MartyCharacter.h"
#include "CozyAdventure.h"
#include "Engine/World.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GameFramework/PlayerController.h"

namespace { const FLinearColor White(.94f,.91f,.78f),Grass(.12f,.32f,.10f),Clay(.45f,.23f,.10f),Blue(.08f,.23f,.36f); }
UParkSports::UParkSports(){PrimaryComponentTick.bCanEverTick=true;}
AStaticMeshActor* UParkSports::Shape(FString Name,FVector P,FVector Size,FLinearColor Color,bool Solid,bool Ball)
{
    auto* A=GetWorld()->SpawnActor<AStaticMeshActor>(P,FRotator::ZeroRotator);A->Tags.Add(FName(*Name));
    auto* C=A->GetStaticMeshComponent();C->SetMobility(EComponentMobility::Movable);
    C->SetStaticMesh(LoadObject<UStaticMesh>(nullptr,Ball?TEXT("/Engine/BasicShapes/Sphere.Sphere"):TEXT("/Engine/BasicShapes/Cube.Cube")));
    auto* Material=LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/Adventure/World/NeighbourhoodSurface.NeighbourhoodSurface"));
    if(!Material)Material=LoadObject<UMaterialInterface>(nullptr,TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
    auto* M=UMaterialInstanceDynamic::Create(Material,A);M->SetVectorParameterValue(TEXT("Color"),Color);C->SetMaterial(0,M);
    A->SetActorScale3D(Size/100);C->SetCollisionProfileName(Solid?TEXT("BlockAll"):TEXT("NoCollision"));return A;
}
UTextRenderComponent* UParkSports::Label(FString Text,FVector P,float Size)
{
    auto* A=GetWorld()->SpawnActor<AActor>();auto* T=NewObject<UTextRenderComponent>(A);A->SetRootComponent(T);T->RegisterComponent();
    T->SetText(FText::FromString(Text));T->SetWorldSize(Size);T->SetTextRenderColor(FColor(255,235,181));T->SetHorizontalAlignment(EHTA_Center);
    A->SetActorLocationAndRotation(P,FRotator(0,-90,0));return T;
}
void UParkSports::Line(FVector A,FVector B,float Width)
{
    if(!FieldMarkings)
    {
        auto* Actor=GetWorld()->SpawnActor<AActor>();Actor->Tags.Add(TEXT("Sports field markings"));
        FieldMarkings=NewObject<UInstancedStaticMeshComponent>(Actor);Actor->SetRootComponent(FieldMarkings);
        FieldMarkings->SetStaticMesh(LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Cube.Cube")));
        auto* M=UMaterialInstanceDynamic::Create(LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/Adventure/World/NeighbourhoodSurface.NeighbourhoodSurface")),Actor);
        M->SetVectorParameterValue(TEXT("Color"),White);FieldMarkings->SetMaterial(0,M);
        FieldMarkings->SetCollisionEnabled(ECollisionEnabled::NoCollision);FieldMarkings->SetCastShadow(false);
        FieldMarkings->RegisterComponent();Actor->SetActorLocation(TerrainOrigin);
    }
    const int Segments=FMath::Max(1,FMath::CeilToInt(FVector::Dist2D(A,B)/40));
    for(int I=0;I<Segments;++I)
    {
        const FVector Start=GroundPoint(FMath::Lerp(A,B,float(I)/Segments)),End=GroundPoint(FMath::Lerp(A,B,float(I+1)/Segments));
        FieldMarkings->AddInstance(FTransform((End-Start).Rotation(),(Start+End)*.5+FVector(0,0,2.5)-TerrainOrigin,FVector(FVector::Dist(Start,End)+.4f,Width,1)/100));
    }
}
FVector UParkSports::Base(int32 I) const
{
    const FVector Bases[]={FVector(0,-750,0),FVector(-650,-100,0),FVector(0,550,0),FVector(650,-100,0)};
    return GroundPoint(Diamond+Bases[I%4]);
}
FVector UParkSports::ResetPoint(int32 Sport) const
{
    return GroundPoint(Sport==0?Soccer+FVector(0,-1100,0):Sport==1?Diamond+FVector(-350,-900,0):Court+FVector(-500,-650,0));
}
void UParkSports::Build(FVector Park)
{
    TerrainOrigin=Park;
    Soccer=GroundPoint(Park+FVector(-1600,3500,0));Diamond=GroundPoint(Park+FVector(1700,3500,0));Court=GroundPoint(Park+FVector(0,6400,0));
    BuildTerrain();
    Label(TEXT("COMMUNITY SPORTS PARK\nSoccer / Baseball / Basketball\nFollow the path to the fields"),Park+FVector(0,850,190),32);
    // Dog-sized practice pitch, two goals and continuous touchlines.
    for(int S:{-1,1})
    {
        Line(Soccer+FVector(S*1100,-900,0),Soccer+FVector(S*1100,900,0));
        Line(Soccer+FVector(-1100,S*900,0),Soccer+FVector(1100,S*900,0));
        const FVector Goal=GroundPoint(Soccer+FVector(S*1130,0,0));
        for(int Y:{-1,1})Shape(TEXT("Soccer goal post"),GroundPoint(Goal+FVector(0,Y*240,0))+FVector(0,0,100),FVector(10,10,200),White,true);
        Shape(TEXT("Soccer crossbar"),Goal+FVector(0,0,200),FVector(10,490,10),White,true);
        for(int I=-4;I<=4;++I)Shape(TEXT("Soccer net"),GroundPoint(Goal+FVector(S*120,I*60,0))+FVector(0,0,100),FVector(4,3,200),White);
        for(int I=0;I<=4;++I)Shape(TEXT("Soccer net"),GroundPoint(Goal+FVector(S*120,0,0))+FVector(0,0,I*50),FVector(4,480,3),White);
    }
    Line(Soccer+FVector(0,-900,0),Soccer+FVector(0,900,0));
    for(int I=0;I<32;++I){const float A=I*2*PI/32,B=(I+1)*2*PI/32;Line(Soccer+FVector(220*FMath::Cos(A),220*FMath::Sin(A),0),Soccer+FVector(220*FMath::Cos(B),220*FMath::Sin(B),0));}
    Label(TEXT("SOCCER\nFace either goal. E / X / Square: kick"),Soccer+FVector(0,1100,180));
    SoccerBall=Shape(TEXT("Park soccer ball"),Soccer+FVector(0,0,16),FVector(32),White,false,true);
    // Diamond, four visible bases, mound and open outfield.
    for(int I=0;I<4;++I){Line(Base(I),Base(I+1));Shape(TEXT("Baseball base"),Base(I)+FVector(0,0,4),FVector(55,55,8),White);Label(I==0?TEXT("HOME"):FString::FromInt(I),Base(I)+FVector(0,40,35),21);}
    Line(Base(0),Diamond+FVector(1300,550,0));Line(Base(0),Diamond+FVector(-1300,550,0));
    Shape(TEXT("Batting tee"),Base(0)+FVector(0,0,18),FVector(9,9,36),Blue);
    Baseball=Shape(TEXT("Park baseball"),Base(0)+FVector(0,0,42),FVector(18),White,false,true);
    Label(TEXT("BASEBALL\nE / X / Square at tee: bat\nRun 1 > 2 > 3 > HOME in 25 seconds\nHurry: Shift / L3"),Diamond+FVector(0,1200,210));
    // A half-court with a backboard, rim and a marked shooting spot.
    for(int S:{-1,1}){Line(Court+FVector(S*900,-800,0),Court+FVector(S*900,800,0));Line(Court+FVector(-900,S*800,0),Court+FVector(900,S*800,0));}
    Shape(TEXT("Basketball post"),Court+FVector(0,730,180),FVector(18,18,360),White,true);
    Shape(TEXT("Basketball backboard"),Court+FVector(0,680,320),FVector(180,12,115),White,true);
    for(int I=0;I<20;++I){float A=I*2*PI/20,B=(I+1)*2*PI/20;auto* Rim=Shape(TEXT("Basketball rim"),Court+FVector(38*(FMath::Cos(A)+FMath::Cos(B))*.5,620+38*(FMath::Sin(A)+FMath::Sin(B))*.5,290),FVector(13,5,5),FLinearColor(.9f,.25f,.02f));Rim->SetActorRotation(FRotator(0,(A+B)*.5*180/PI+90,0));}
    Line(Court+FVector(-220,0,0),Court+FVector(220,0,0),10);
    Basketball=Shape(TEXT("Park basketball"),Court+FVector(0,-450,22),FVector(44),FLinearColor(.8f,.25f,.03f),false,true);
    Label(TEXT("BASKETBALL\nE / X / Square: shoot toward the hoop\nFace the hoop before shooting"),Court+FVector(600,850,190));
    for(int I=0;I<3;++I){auto P=ResetPoint(I);Shape(TEXT("Sports reset bollard"),P+FVector(0,0,45),FVector(24,24,90),White,true);Label(TEXT("E / X / Square\nReset ball"),P+FVector(0,0,115),22);}
    Score=Label(TEXT(""),Park+FVector(0,1900,230),34);UpdateScore();
}
void UParkSports::UpdateScore(){if(Score)Score->SetText(FText::FromString(FString::Printf(TEXT("PARK SCOREBOARD\nSoccer %d : %d | Baseball runs %d | Baskets %d"),Goals[0],Goals[1],Runs,Baskets)));}
void UParkSports::Tell(FString Text){if(auto* M=GetWorld()->GetAuthGameMode<AMartyPlayMode>())if(M->Adventure){M->Adventure->Message=Text;M->Adventure->HoldMessage(5);}}
void UParkSports::Reset(int32 I)
{
    if(I==0){SoccerVelocity=FVector::ZeroVector;SoccerBall->SetActorLocation(Soccer+FVector(0,0,16));}
    if(I==1){NextBase=-1;BatTime=0;Baseball->SetActorLocation(Base(0)+FVector(0,0,42));}
    if(I==2){ShotTime=-1;Basketball->SetActorLocation(Court+FVector(0,-450,22));}
}
FString UParkSports::Location(FVector P) const
{
    if(FVector::Dist2D(P,Soccer)<1450)return TEXT("Park / soccer pitch");
    if(FVector::Dist2D(P,Diamond)<1500)return TEXT("Park / baseball diamond");
    if(FVector::Dist2D(P,Court)<1350)return TEXT("Park / basketball court");return FString();
}
FString UParkSports::Prompt(const AMartyCharacter* P) const
{
    if(!P)return FString();if(auto* M=GetWorld()->GetAuthGameMode<AMartyPlayMode>())if(M->Adventure&&!M->Adventure->Feature(ECozyFeature::ParkSports))return FString();const FVector Pos=P->GetActorLocation();
    auto Near=[&](FVector Target,float Range){return FVector::Dist2D(Pos,Target)<Range&&FMath::Abs(Pos.Z-Target.Z)<110;};
    for(int I=0;I<3;++I)if(Near(ResetPoint(I),130))return TEXT("E / X / Square: reset this sport's ball (scores kept)");
    if(Near(SoccerBall->GetActorLocation(),145))return TEXT("E / X / Square: kick soccer ball where Marty faces");
    if(NextBase<0&&Near(Base(0),150))return TEXT("E / X / Square: bat, then run 1 > 2 > 3 > HOME");
    if(ShotTime<0&&Near(Basketball->GetActorLocation(),145))return TEXT("E / X / Square: shoot basketball where Marty faces");
    if(NextBase>=0&&FVector::Dist2D(Pos,Diamond)<1800)return FString::Printf(TEXT("Baseball: run to %s | %.0f seconds"),NextBase==0?TEXT("HOME"):*FString::FromInt(NextBase),25-BatTime);
    return FString();
}
bool UParkSports::Interact(AMartyCharacter* P)
{
    if(!P||Prompt(P).IsEmpty())return false;FVector Pos=P->GetActorLocation();
    for(int I=0;I<3;++I)if(FVector::Dist2D(Pos,ResetPoint(I))<130){Reset(I);Tell(TEXT("Ball reset. Keep playing!"));return true;}
    FVector Aim=P->GetActorForwardVector().GetSafeNormal2D();
    if(FVector::Dist2D(Pos,SoccerBall->GetActorLocation())<145){if(KickCooldown<=0){SoccerVelocity=Aim*1000;KickCooldown=.25;Tell(TEXT("Soccer! Chase the ball and line up your next kick."));}return true;}
    if(NextBase<0&&FVector::Dist2D(Pos,Base(0))<150){NextBase=1;BatTime=0;BatVelocity=FVector(Aim.X*.6,FMath::Max(.4,Aim.Y),0).GetSafeNormal()*800+FVector(0,0,600);Tell(TEXT("Hit! Run first, second, third, then home!"));return true;}
    if(ShotTime<0&&FVector::Dist2D(Pos,Basketball->GetActorLocation())<145){ShotStart=Basketball->GetActorLocation();ShotTime=0;
        // Flight retains the player's aim. Only a close, well-directed shot crosses the rim.
        Basketball->SetActorRotation(FRotator(0,Aim.Rotation().Yaw,0));Tell(TEXT("Basketball shot! Face the hoop for a clean basket."));return true;}
    return true;
}
void UParkSports::TickComponent(float Delta,ELevelTick Type,FActorComponentTickFunction* Function)
{
    Super::TickComponent(Delta,Type,Function);auto* M=GetWorld()->GetAuthGameMode<AMartyPlayMode>();
    if(!SoccerBall||!M||!M->Adventure||!M->Adventure->Feature(ECozyFeature::ParkSports)||!M->Neighbourhood->Outside||M->Adventure->Screen!=ECozyScreen::Playing)return;
    auto* PC=GetWorld()->GetFirstPlayerController();auto* P=PC?Cast<AMartyCharacter>(PC->GetPawn()):nullptr;if(!P)return;
    const float Dt=Delta;KickCooldown=FMath::Max(0.f,KickCooldown-Dt);
    FVector Previous=SoccerBall->GetActorLocation()-Soccer,Next=Previous+SoccerVelocity*Dt;
    if(FMath::Abs(Previous.X)<1130&&FMath::Abs(Next.X)>=1130)
    {
        const float Crossing=FMath::Lerp(Previous.Y,Next.Y,(FMath::Sign(Next.X)*1130-Previous.X)/(Next.X-Previous.X));
        if(FMath::Abs(Crossing)<224){++Goals[Next.X>0?0:1];if(M&&M->Adventure)M->Adventure->RecordMission(TEXT("soccer"));Reset(0);UpdateScore();Tell(FString::Printf(TEXT("GOAL! Soccer %d : %d. Ball back at centre."),Goals[0],Goals[1]));Next=SoccerBall->GetActorLocation()-Soccer;}
    }
    SoccerBall->SetActorLocation(GroundPoint(Soccer+Next)+FVector(0,0,16));
    // Gravity projected along the local slope, with rolling resistance. Deadband prevents jitter.
    const FVector At=SoccerBall->GetActorLocation();
    FVector Slope((GroundHeight(At+FVector(20,0,0))-GroundHeight(At-FVector(20,0,0)))/40,
                  (GroundHeight(At+FVector(0,20,0))-GroundHeight(At-FVector(0,20,0)))/40,0);
    if(SoccerVelocity.Size2D()>3)SoccerVelocity-=Slope*980*Dt;
    SoccerVelocity*=FMath::Exp(-.38f*Dt);
    if(FMath::Abs(Next.X)>1380||FMath::Abs(Next.Y)>1100){Reset(0);Tell(TEXT("Soccer ball out of bounds. Restart at centre."));}
    if(NextBase>=0)
    {
        BatTime+=Dt;if(BatTime<2){BatVelocity.Z-=700*Dt;FVector B=Baseball->GetActorLocation()+BatVelocity*Dt;const float Floor=GroundHeight(B)+10;B.Z=FMath::Max(double(Floor),B.Z);if(B.Z<=Floor)BatVelocity.Z=0;Baseball->SetActorLocation(B);}
        if(FVector::Dist2D(P->GetActorLocation(),Base(NextBase))<115&&FMath::Abs(P->GetActorLocation().Z-Base(NextBase).Z)<110)
        {if(NextBase==0){++Runs;if(M&&M->Adventure)M->Adventure->RecordMission(TEXT("baseball"));Reset(1);UpdateScore();Tell(FString::Printf(TEXT("Safe at home! Baseball runs: %d."),Runs));}else{NextBase=(NextBase+1)%4;Tell(NextBase==0?TEXT("Third base! Head HOME!"):TEXT("Safe! Keep running to the next numbered base."));}}
        if(BatTime>=25){Reset(1);Tell(TEXT("Time's up! Try another hit and run the bases."));}
    }
    if(ShotTime>=0)
    {
        ShotTime+=Dt;const float T=FMath::Min(ShotTime/1.2f,1.f);FVector Aim=Basketball->GetActorForwardVector();FVector End=ShotStart+Aim*1070;End.Z=Court.Z+290;
        Basketball->SetActorLocation(FMath::Lerp(ShotStart,End,T)+FVector(0,0,460*FMath::Sin(PI*T)));
        if(T>=1){const bool Scored=FVector::Dist2D(End,Court+FVector(0,620,290))<32;if(Scored){++Baskets;if(M&&M->Adventure)M->Adventure->RecordMission(TEXT("basketball"));UpdateScore();}Reset(2);Tell(Scored?FString::Printf(TEXT("Swish! Baskets: %d. Shoot again!"),Baskets):TEXT("Miss! Face the hoop from the ball and try again."));}
    }
}
