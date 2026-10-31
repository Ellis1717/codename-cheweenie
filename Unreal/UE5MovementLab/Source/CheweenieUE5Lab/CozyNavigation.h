#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CozyNavigation.generated.h"

struct FCozyLandmark { FString Label; FVector Position; bool Outdoors=false; };
struct FCozyStreetGraph
{
    TArray<FVector> Nodes;
    TArray<TArray<TPair<int32,double>>> Links;
    TArray<FIntPoint> Ends;
    TArray<int32> ConnectedEdges;
    int32 EdgeCount=-1;
};

// Local geometry only. Private street data is never copied to public assets or reports.
UCLASS()
class UCozyNavigation : public UActorComponent
{
    GENERATED_BODY()
public:
    UCozyNavigation();
    virtual void TickComponent(float Delta,ELevelTick Type,FActorComponentTickFunction* Function) override;
    TArray<FCozyLandmark> Landmarks() const;
    FCozyLandmark Goal() const;
    FString Location() const;
    FString Guidance() const;
    static float Bearing(FVector Direction);
    static FString Cardinal(float Degrees);
    static float RelativeTurn(FVector Direction,float ActorYaw);
    void RefreshRoute();
    TArray<FVector> Route;
    bool StreetRoute=false;
    void InvalidateNetwork(){Graph.EdgeCount=-1;}
    int32 GraphBuilds=0,RouteSearches=0;
    double LastGraphMs=0,LastSearchMs=0;
private:
    float RouteTime=0;
    FCozyStreetGraph Graph;
    void BuildNetwork(const TArray<TPair<FVector,FVector>>& Edges);
    FVector LastStart=FVector::ZeroVector,LastGoal=FVector::ZeroVector;
    bool LastOutside=false;
};
