#include "CozyNavigation.h"
#include "CozyAdventure.h"
#include "Neighbourhood.h"
#include "MartyCharacter.h"
#include "GameFramework/PlayerController.h"
#include <queue>
#include <vector>
#include <functional>

UCozyNavigation::UCozyNavigation(){PrimaryComponentTick.bCanEverTick=true;}
float UCozyNavigation::Bearing(FVector D){return FMath::Fmod(450.f+D.Rotation().Yaw,360.f);}
FString UCozyNavigation::Cardinal(float Degrees){static const TCHAR* Names[]={TEXT("N"),TEXT("NE"),TEXT("E"),TEXT("SE"),TEXT("S"),TEXT("SW"),TEXT("W"),TEXT("NW")};return Names[FMath::RoundToInt(Degrees/45.f)%8];}
float UCozyNavigation::RelativeTurn(FVector D,float Yaw){return FRotator::NormalizeAxis(D.Rotation().Yaw-Yaw);}
TArray<FCozyLandmark> UCozyNavigation::Landmarks() const
{
    const auto* M=Cast<AMartyPlayMode>(GetOwner());if(!M||!M->Adventure||!M->Adventure->Progress||!M->Neighbourhood)return {};
    const auto* S=M->Adventure.Get();const auto* N=M->Neighbourhood.Get();FCozyLandmark Objective{TEXT("Current patrol objective"),FVector(-200,-200,28),false};
    if(!S->Progress->Complete){bool Found=false;for(int32 I=0;I<S->Mission->Snacks.Num();++I)if(!(S->Progress->SnackMask&(1<<I))){Objective.Position=S->Mission->Snacks[I].Position;Objective.Label=TEXT("Next golden snack");Found=true;break;}
        if(!Found&&S->Mission->Visits.IsValidIndex(S->Progress->VisitIndex)){const auto& V=S->Mission->Visits[S->Progress->VisitIndex];Objective.Position=V.Position;Objective.Label=V.Label;}}
    TArray<FCozyLandmark> Result={Objective,{TEXT("Home / headquarters"),FVector(-200,-200,28),false},{TEXT("Park"),N->Park,true},{TEXT("Glen Dhu school"),N->School,true},{TEXT("Kitchen bowl"),FVector(-180,320,0),false}};
    for(const auto& V:S->Mission->Visits)Result.Add({V.Label,V.Position,false});
    for(const auto& D:N->Dogs)Result.Add({D.Name,D.Position,true});return Result;
}
FCozyLandmark UCozyNavigation::Goal() const
{
    const auto* M=Cast<AMartyPlayMode>(GetOwner());const auto Stops=Landmarks();if(Stops.IsEmpty())return {};
    const int32 Choice=FMath::Clamp(M->Adventure->Progress->NavTarget,0,Stops.Num()-1);auto G=Stops[Choice];
    if(G.Outdoors!=M->Neighbourhood->Outside){G.Label=TEXT("Door to ")+G.Label;G.Position=M->Neighbourhood->Outside?M->Neighbourhood->Entrance:FVector(-425,-200,28);G.Outdoors=M->Neighbourhood->Outside;}return G;
}
FString UCozyNavigation::Location() const
{
    const auto* M=Cast<AMartyPlayMode>(GetOwner());const auto* PC=GetWorld()->GetFirstPlayerController();const APawn* P=PC?PC->GetPawn():nullptr;if(!M||!P)return TEXT("Locating Marty...");
    const FVector Pos=P->GetActorLocation();
    if(!M->Neighbourhood->Outside){if(Pos.Y>240&&Pos.X<0)return TEXT("Headquarters / kitchen");if(Pos.X>130)return TEXT("Headquarters / sofa corner");return TEXT("Headquarters / cozy room");}
    auto* N=M->Neighbourhood.Get();if(FVector::Dist2D(Pos,N->Entrance)<900)return TEXT("Headquarters / front garden");
    if(FVector::Dist2D(Pos,N->Park)<1100)return TEXT("Park");if(FVector::Dist2D(Pos,N->School)<1000)return TEXT("Glen Dhu school approach");
    double Best=2000000;FString Name=TEXT("Neighbourhood walk");for(const auto& L:N->NamedStreets){FVector Flat=Pos;Flat.Z=0;const double D=FMath::PointDistToSegment(Flat,L.A,L.B);if(D<Best){Best=D;Name=L.Name;}}return Name;
}
FString UCozyNavigation::Guidance() const
{
    const auto* PC=GetWorld()->GetFirstPlayerController();const APawn* P=PC?PC->GetPawn():nullptr;if(!P)return FString();const auto G=Goal();const FVector Delta=G.Position-P->GetActorLocation();
    const float Distance=Delta.Size2D()/100;if(Distance<1.2f)return G.Label+TEXT(" | Arrived - look for the interaction prompt");
    FVector Direction=Delta;if(StreetRoute)for(int32 I=1;I<Route.Num();++I)if(FVector::Dist2D(P->GetActorLocation(),Route[I])>150){Direction=Route[I]-P->GetActorLocation();break;}
    const float Turn=RelativeTurn(Direction,P->GetActorRotation().Yaw);
    const FString TurnText=FMath::Abs(Turn)<20?TEXT("Ahead"):FMath::Abs(Turn)>150?TEXT("Turn around"):Turn>0?TEXT("Turn right"):TEXT("Turn left");
    return FString::Printf(TEXT("%s | %.0f m direct | %s | %s"),*G.Label,Distance,*TurnText,StreetRoute?TEXT("street guide"):TEXT("bearing only"));
}
void UCozyNavigation::TickComponent(float Delta,ELevelTick Type,FActorComponentTickFunction* Function)
{
    Super::TickComponent(Delta,Type,Function);const auto* M=Cast<AMartyPlayMode>(GetOwner());if(!M||!M->Adventure||!M->Adventure->Progress)return;
    RouteTime-=Delta;if(RouteTime<=0){RouteTime=1;RefreshRoute();}
}
void UCozyNavigation::RefreshRoute()
{
    Route.Empty();StreetRoute=false;const auto* M=Cast<AMartyPlayMode>(GetOwner());const auto* PC=GetWorld()->GetFirstPlayerController();const APawn* P=PC?PC->GetPawn():nullptr;
    if(!M||!P||!M->Adventure->Progress||!M->Adventure->Feature(ECozyFeature::Route))return;
    const FVector Start=P->GetActorLocation(),End=Goal().Position;Route={Start,End};if(!M->Neighbourhood->Outside)return;
    const auto& Edges=M->Neighbourhood->Streets();if(Edges.IsEmpty())return;
    TArray<FVector> Nodes;TArray<TArray<TPair<int32,double>>> Links;TMap<FIntPoint,int32> Index;
    auto Node=[&](FVector V){V.Z=0;const FIntPoint Key(FMath::RoundToInt(V.X),FMath::RoundToInt(V.Y));if(const auto* I=Index.Find(Key))return *I;const int32 I=Nodes.Add(V);Links.AddDefaulted();Index.Add(Key,I);return I;};
    int32 StartEdge=0,EndEdge=0;double SD=TNumericLimits<double>::Max(),ED=SD;
    for(int32 I=0;I<Edges.Num();++I){const auto& E=Edges[I];const int32 A=Node(E.Key),B=Node(E.Value);const double D=FVector::Dist2D(E.Key,E.Value);Links[A].Add({B,D});Links[B].Add({A,D});
        FVector S=Start,T=End;S.Z=T.Z=0;double D1=FMath::PointDistToSegment(S,Nodes[A],Nodes[B]),D2=FMath::PointDistToSegment(T,Nodes[A],Nodes[B]);if(D1<SD){SD=D1;StartEdge=I;}if(D2<ED){ED=D2;EndEdge=I;}}
    // Route on the largest connected path network. Detached decorative driveways
    // remain visible on the map but must not trap the search in a two-node island.
    TArray<int32> Component;Component.Init(-1,Nodes.Num());int32 Largest=-1,LargestCount=0,NextComponent=0;
    for(int32 I=0;I<Nodes.Num();++I)if(Component[I]<0){TArray<int32> Pending={I};Component[I]=NextComponent;int32 Count=0;while(!Pending.IsEmpty()){const int32 V=Pending.Pop(EAllowShrinking::No);++Count;for(const auto& L:Links[V])if(Component[L.Key]<0){Component[L.Key]=NextComponent;Pending.Add(L.Key);}}if(Count>LargestCount){Largest=NextComponent;LargestCount=Count;}++NextComponent;}
    SD=ED=TNumericLimits<double>::Max();
    for(int32 I=0;I<Edges.Num();++I){const auto& E=Edges[I];if(Component[Node(E.Key)]!=Largest)continue;FVector S=Start,T=End;S.Z=T.Z=0;const double D1=FMath::PointDistToSegment(S,E.Key,E.Value),D2=FMath::PointDistToSegment(T,E.Key,E.Value);if(D1<SD){SD=D1;StartEdge=I;}if(D2<ED){ED=D2;EndEdge=I;}}
    TArray<double> Cost;Cost.Init(TNumericLimits<double>::Max(),Nodes.Num());TArray<int32> Prev;Prev.Init(-1,Nodes.Num());
    using FEntry=std::pair<double,int32>;std::priority_queue<FEntry,std::vector<FEntry>,std::greater<FEntry>> Open;
    const FVector A=FMath::ClosestPointOnSegment(FVector(Start.X,Start.Y,0),Edges[StartEdge].Key,Edges[StartEdge].Value),B=FMath::ClosestPointOnSegment(FVector(End.X,End.Y,0),Edges[EndEdge].Key,Edges[EndEdge].Value);
    for(FVector V:{Edges[StartEdge].Key,Edges[StartEdge].Value}){int32 I=Node(V);Cost[I]=FVector::Dist2D(A,V);Open.push({Cost[I],I});}
    while(!Open.empty()){const auto E=Open.top();Open.pop();if(E.first>Cost[E.second])continue;for(const auto& L:Links[E.second]){const double C=E.first+L.Value;if(C<Cost[L.Key]){Cost[L.Key]=C;Prev[L.Key]=E.second;Open.push({C,L.Key});}}}
    const int32 X=Node(Edges[EndEdge].Key),Y=Node(Edges[EndEdge].Value);int32 GoalIndex=Cost[X]+FVector::Dist2D(Nodes[X],B)<Cost[Y]+FVector::Dist2D(Nodes[Y],B)?X:Y;
    if(Cost[GoalIndex]==TNumericLimits<double>::Max())return;
    TArray<FVector> Reverse;for(int32 I=GoalIndex;I>=0;I=Prev[I])Reverse.Add(Nodes[I]);
    Route={Start,A};if(StartEdge!=EndEdge)for(int32 I=Reverse.Num()-1;I>=0;--I)Route.Add(Reverse[I]);Route.Add(B);Route.Add(End);StreetRoute=true;
}
