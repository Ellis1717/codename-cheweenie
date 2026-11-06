#include "CozyNavigation.h"
#include "HouseUpstairs.h"
#include "HomeProgression.h"
#include "HalloweenSpecial.h"
#include "CozyAdventure.h"
#include "Neighbourhood.h"
#include "ParkSports.h"
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
        if(!Found&&S->Mission->Visits.IsValidIndex(S->Progress->VisitIndex)){const auto& V=S->Mission->Visits[S->Progress->VisitIndex];Objective.Position=S->StopPosition(V);Objective.Outdoors=S->StopOutdoors(V);Objective.Label=V.Label;}}
    if(M->Festival&&M->Festival->Active())Objective={M->Festival->Objective(),M->Festival->GoalPosition(),M->Festival->NextPorch()>=0||N->Outside};
    TArray<FCozyLandmark> Result={Objective,{TEXT("Home / headquarters"),FVector(-200,-200,28),false},{TEXT("Park"),N->Park,true},{TEXT("Glen Dhu school"),N->School,true},{TEXT("Kitchen bowl"),FVector(-180,320,0),false}};
    for(const auto& V:S->Mission->Visits)Result.Add({V.Label,S->StopPosition(V),S->StopOutdoors(V)});
    for(const auto& D:N->Dogs)Result.Add({D.Name,D.Position,true});if(M->HomeProgression){const auto* H=M->HomeProgression.Get();Result.Add({TEXT("Marty bed"),H->Bed,false});Result.Add({TEXT("Garage + gym door"),H->Portal,false});Result.Add({TEXT("Padded harness workshop"),H->ArmorBench,false});Result.Add({TEXT("Strength training"),H->Gym[0],false});Result.Add({TEXT("Agility training"),H->Gym[1],false});Result.Add({TEXT("Speed training"),H->Gym[2],false});Result.Add({TEXT("Annex return door"),H->Exit,false});}
    Result.Add({TEXT("Stairs / upstairs"),CheweenieHouse::StairBottom(),false});
    Result.Add({TEXT("Main bedroom"),FVector(-220,0,408),false});
    Result.Add({TEXT("Guest bedroom"),FVector(220,0,408),false});
    Result.Add({TEXT("Upstairs bathroom"),FVector(0,365,408),false});
    Result.Add({TEXT("Upstairs pet nook"),FVector(-270,-245,408),false});
    if(N->Sports){Result.Add({TEXT("Park soccer pitch"),N->Sports->Soccer,true});Result.Add({TEXT("Park baseball diamond"),N->Sports->Base(0),true});Result.Add({TEXT("Park basketball court"),N->Sports->Court,true});}return Result;
}
FCozyLandmark UCozyNavigation::Goal() const
{
    const auto* M=Cast<AMartyPlayMode>(GetOwner());const auto Stops=Landmarks();if(Stops.IsEmpty())return {};
    if(M->HomeProgression&&M->HomeProgression->Training>=0)return {M->HomeProgression->TrainingObjective(),M->HomeProgression->TrainingGoal(),false};
    const int32 Choice=FMath::Clamp(M->Adventure->Progress->NavTarget,0,Stops.Num()-1);auto G=Stops[Choice];
    if(G.Outdoors!=M->Neighbourhood->Outside){G.Label=TEXT("Door to ")+G.Label;G.Position=M->Neighbourhood->Outside?M->Neighbourhood->Entrance:FVector(-425,-200,28);G.Outdoors=M->Neighbourhood->Outside;}if(M->HomeProgression&&!G.Outdoors){const auto* H=M->HomeProgression.Get();if(H->InAnnex()&&!UHomeProgression::PointInAnnex(G.Position)){G.Label=TEXT("Return via headquarters: ")+Stops[Choice].Label;G.Position=H->Exit;}else if(!H->InAnnex()&&!M->Neighbourhood->Outside&&UHomeProgression::PointInAnnex(G.Position)){G.Label=TEXT("Garage door to ")+G.Label;G.Position=H->Portal;}}if(!G.Outdoors&&!M->Neighbourhood->Outside&&!UHomeProgression::PointInAnnex(G.Position))
    {
        const auto* PC=GetWorld()->GetFirstPlayerController();const FVector P=PC&&PC->GetPawn()?PC->GetPawn()->GetActorLocation():FVector::ZeroVector;
        if(G.Position.Z>=CheweenieHouse::UpperFloor&&!CheweenieHouse::Upstairs(P))
        {G.Label=TEXT("Walk upstairs to ")+G.Label;G.Position=P.Y< -330?CheweenieHouse::StairTop():CheweenieHouse::StairBottom();}
        else if(G.Position.Z<CheweenieHouse::UpperFloor&&P.Z>150&&FMath::Abs(P.X)<510&&FMath::Abs(P.Y)<510)
        {G.Label=TEXT("Walk downstairs to ")+G.Label;G.Position=P.Y< -330?CheweenieHouse::StairBottom():CheweenieHouse::StairTop();}
    }
    return G;
}
FString UCozyNavigation::Location() const
{
    const auto* M=Cast<AMartyPlayMode>(GetOwner());const auto* PC=GetWorld()->GetFirstPlayerController();const APawn* P=PC?PC->GetPawn():nullptr;if(!M||!P)return TEXT("Locating Marty...");
    const FVector Pos=P->GetActorLocation();if(M->HomeProgression&&M->HomeProgression->InAnnex())return Pos.X<8300?TEXT("Headquarters / garage workshop"):TEXT("Headquarters / gym");
    if(!M->Neighbourhood->Outside){if(CheweenieHouse::Upstairs(Pos)||(Pos.Y< -330&&Pos.Z>80))return CheweenieHouse::Location(Pos);if(Pos.Y>240&&Pos.X<0)return TEXT("Headquarters / kitchen");if(Pos.X>130)return TEXT("Headquarters / sofa corner");return TEXT("Headquarters / cozy room");}
    auto* N=M->Neighbourhood.Get();if(FVector::Dist2D(Pos,N->Entrance)<900)return TEXT("Headquarters / front garden");
    if(N->Sports){const FString Sport=N->Sports->Location(Pos);if(!Sport.IsEmpty())return Sport;}
    if(FVector::Dist2D(Pos,N->Park)<1100)return TEXT("Park");if(FVector::Dist2D(Pos,N->School)<1000)return TEXT("Glen Dhu school approach");
    double Best=2000000;FString Name=TEXT("Neighbourhood walk");for(const auto& L:N->NamedStreets){FVector Flat=Pos;Flat.Z=0;const double D=FMath::PointDistToSegment(Flat,L.A,L.B);if(D<Best){Best=D;Name=L.Name;}}return Name;
}
FString UCozyNavigation::Guidance() const
{
    const auto* PC=GetWorld()->GetFirstPlayerController();const APawn* P=PC?PC->GetPawn():nullptr;if(!P)return FString();const auto G=Goal();const FVector Delta=G.Position-P->GetActorLocation();
    const float Distance=Delta.Size2D()/100;if(Distance<1.2f&&FMath::Abs(Delta.Z)<120)return G.Label+TEXT(" | Arrived - look for the interaction prompt");
    FVector Direction=Delta;if(StreetRoute)for(int32 I=1;I<Route.Num();++I)if(FVector::Dist2D(P->GetActorLocation(),Route[I])>150){Direction=Route[I]-P->GetActorLocation();break;}
    const float Turn=RelativeTurn(Direction,P->GetActorRotation().Yaw);
    const FString TurnText=FMath::Abs(Turn)<20?TEXT("Ahead"):FMath::Abs(Turn)>150?TEXT("Turn around"):Turn>0?TEXT("Turn right"):TEXT("Turn left");
    return FString::Printf(TEXT("%s | %.0f m direct | %s | %s"),*G.Label,Distance,*TurnText,StreetRoute?TEXT("street guide"):TEXT("bearing only"));
}
void UCozyNavigation::BuildNetwork(const TArray<TPair<FVector,FVector>>& Edges)
{
    const double Started=FPlatformTime::Seconds();Graph=FCozyStreetGraph();Graph.EdgeCount=Edges.Num();++GraphBuilds;
    TMap<FIntPoint,int32> Index;
    auto Node=[&](FVector V){V.Z=0;const FIntPoint Key(FMath::RoundToInt(V.X),FMath::RoundToInt(V.Y));if(const auto* I=Index.Find(Key))return *I;const int32 I=Graph.Nodes.Add(V);Graph.Links.AddDefaulted();Index.Add(Key,I);return I;};
    for(const auto& E:Edges){const int32 A=Node(E.Key),B=Node(E.Value);const double D=FVector::Dist2D(E.Key,E.Value);Graph.Links[A].Add({B,D});Graph.Links[B].Add({A,D});Graph.Ends.Add(FIntPoint(A,B));}
    TArray<int32> Component;Component.Init(-1,Graph.Nodes.Num());int32 Largest=-1,LargestCount=0,NextComponent=0;
    for(int32 I=0;I<Graph.Nodes.Num();++I)if(Component[I]<0){TArray<int32> Pending={I};Component[I]=NextComponent;int32 Count=0;while(!Pending.IsEmpty()){const int32 V=Pending.Pop(EAllowShrinking::No);++Count;for(const auto& L:Graph.Links[V])if(Component[L.Key]<0){Component[L.Key]=NextComponent;Pending.Add(L.Key);}}if(Count>LargestCount){Largest=NextComponent;LargestCount=Count;}++NextComponent;}
    for(int32 I=0;I<Graph.Ends.Num();++I)if(Component[Graph.Ends[I].X]==Largest)Graph.ConnectedEdges.Add(I);
    LastGraphMs=(FPlatformTime::Seconds()-Started)*1000;
}
void UCozyNavigation::TickComponent(float Delta,ELevelTick Type,FActorComponentTickFunction* Function)
{
    Super::TickComponent(Delta,Type,Function);const auto* M=Cast<AMartyPlayMode>(GetOwner());const auto* PC=GetWorld()->GetFirstPlayerController();const APawn* P=PC?PC->GetPawn():nullptr;
    if(!M||!P||!M->Adventure->Progress||M->Adventure->Screen!=ECozyScreen::Playing||!M->Adventure->Feature(ECozyFeature::Route))return;
    RouteTime-=Delta;if(RouteTime>0)return;RouteTime=1;
    if((M->Neighbourhood->Outside&&Graph.EdgeCount<0)||M->Neighbourhood->Outside!=LastOutside||FVector::DistSquared2D(P->GetActorLocation(),LastStart)>10000||FVector::DistSquared2D(Goal().Position,LastGoal)>1600)RefreshRoute();
}
void UCozyNavigation::RefreshRoute()
{
    const double Started=FPlatformTime::Seconds();Route.Empty();StreetRoute=false;const auto* M=Cast<AMartyPlayMode>(GetOwner());const auto* PC=GetWorld()->GetFirstPlayerController();const APawn* P=PC?PC->GetPawn():nullptr;
    if(!M||!P||!M->Adventure->Progress)return;
    const FVector Start=P->GetActorLocation(),End=Goal().Position;LastStart=Start;LastGoal=End;LastOutside=M->Neighbourhood->Outside;
    if(!M->Adventure->Feature(ECozyFeature::Route))return;Route={Start,End};if(!LastOutside)return;
    const auto& Edges=M->Neighbourhood->Streets();if(Graph.EdgeCount!=Edges.Num())BuildNetwork(Edges);if(Graph.ConnectedEdges.IsEmpty())return;++RouteSearches;
    int32 StartEdge=0,EndEdge=0;double SD=TNumericLimits<double>::Max(),ED=SD;
    for(int32 I:Graph.ConnectedEdges){const auto Ends=Graph.Ends[I];const FVector A=Graph.Nodes[Ends.X],B=Graph.Nodes[Ends.Y];FVector S=Start,T=End;S.Z=T.Z=0;const double D1=FMath::PointDistToSegment(S,A,B),D2=FMath::PointDistToSegment(T,A,B);if(D1<SD){SD=D1;StartEdge=I;}if(D2<ED){ED=D2;EndEdge=I;}}
    TArray<double> Cost;Cost.Init(TNumericLimits<double>::Max(),Graph.Nodes.Num());TArray<int32> Prev;Prev.Init(-1,Graph.Nodes.Num());
    using FEntry=std::pair<double,int32>;std::priority_queue<FEntry,std::vector<FEntry>,std::greater<FEntry>> Open;
    const auto S=Graph.Ends[StartEdge],E=Graph.Ends[EndEdge];const FVector A=FMath::ClosestPointOnSegment(FVector(Start.X,Start.Y,0),Graph.Nodes[S.X],Graph.Nodes[S.Y]),B=FMath::ClosestPointOnSegment(FVector(End.X,End.Y,0),Graph.Nodes[E.X],Graph.Nodes[E.Y]);
    for(int32 I:{S.X,S.Y}){Cost[I]=FVector::Dist2D(A,Graph.Nodes[I]);Open.push({Cost[I],I});}
    while(!Open.empty()){const auto Entry=Open.top();Open.pop();if(Entry.first>Cost[Entry.second])continue;for(const auto& L:Graph.Links[Entry.second]){const double C=Entry.first+L.Value;if(C<Cost[L.Key]){Cost[L.Key]=C;Prev[L.Key]=Entry.second;Open.push({C,L.Key});}}}
    int32 GoalIndex=Cost[E.X]+FVector::Dist2D(Graph.Nodes[E.X],B)<Cost[E.Y]+FVector::Dist2D(Graph.Nodes[E.Y],B)?E.X:E.Y;
    if(Cost[GoalIndex]==TNumericLimits<double>::Max())return;TArray<FVector> Reverse;for(int32 I=GoalIndex;I>=0;I=Prev[I])Reverse.Add(Graph.Nodes[I]);
    Route={Start,A};if(StartEdge!=EndEdge)for(int32 I=Reverse.Num()-1;I>=0;--I)Route.Add(Reverse[I]);Route.Add(B);Route.Add(End);StreetRoute=true;LastSearchMs=(FPlatformTime::Seconds()-Started)*1000;
}
