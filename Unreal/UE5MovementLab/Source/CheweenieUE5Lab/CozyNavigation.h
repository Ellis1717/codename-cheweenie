#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CozyNavigation.generated.h"

struct FCozyLandmark { FString Label; FVector Position; bool Outdoors=false; };

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
private:
    float RouteTime=0;
};
