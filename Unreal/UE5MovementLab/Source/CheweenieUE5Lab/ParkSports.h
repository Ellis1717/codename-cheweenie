#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ParkSports.generated.h"
class AMartyCharacter;
class AStaticMeshActor;
class UTextRenderComponent;
class UInstancedStaticMeshComponent;

// Small, replayable solo park games. Session scores never alter adventure saves.
UCLASS()
class UParkSports : public UActorComponent
{
    GENERATED_BODY()
public:
    UParkSports();
    void Build(FVector Park);
    FString Prompt(const AMartyCharacter* Pawn) const;
    bool Interact(AMartyCharacter* Pawn);
    FString Location(FVector P) const;
    virtual void TickComponent(float Delta,ELevelTick Type,FActorComponentTickFunction* Function) override;
    FVector Soccer, Diamond, Court;
    FVector Base(int32 Index) const;
    FVector ResetPoint(int32 Sport) const;
    // Uses the same triangles as the colliding terrain, including on raised approaches.
    float GroundHeight(FVector Position) const;
    FVector GroundPoint(FVector Position) const;
    int32 Goals[2]={0,0}, Runs=0, Baskets=0, NextBase=-1;
    UPROPERTY() TObjectPtr<AStaticMeshActor> SoccerBall;
    UPROPERTY() TObjectPtr<AStaticMeshActor> Baseball;
    UPROPERTY() TObjectPtr<AStaticMeshActor> Basketball;
private:
    FVector TerrainOrigin;
    float TerrainVertex(float X,float Y) const;
    void BuildTerrain();
    void Surface(FString Name,FVector Center,FVector2D Size,FLinearColor Color,float Yaw=0,float Lift=0);
    FVector SoccerVelocity, BatVelocity, ShotStart;
    float ShotTime=-1, BatTime=0, KickCooldown=0;
    UPROPERTY() TObjectPtr<UTextRenderComponent> Score;
    UPROPERTY() TObjectPtr<UInstancedStaticMeshComponent> FieldMarkings;
    AStaticMeshActor* Shape(FString Name,FVector P,FVector Size,FLinearColor Color,bool Solid=false,bool Ball=false);
    UTextRenderComponent* Label(FString Text,FVector P,float Size=30);
    void Line(FVector A,FVector B,float Width=5);
    void Reset(int32 Sport);
    void Tell(FString Text);
    void UpdateScore();
};
