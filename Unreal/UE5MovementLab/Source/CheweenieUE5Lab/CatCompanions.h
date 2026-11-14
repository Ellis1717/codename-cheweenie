#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CatCompanions.generated.h"
class AStaticMeshActor;
class AMartyCharacter;
class UAnimalGait;
struct FCozyCatRuntime
{
    TWeakObjectPtr<AStaticMeshActor> Actor;
    TWeakObjectPtr<UAnimalGait> Gait;
    FTransform Home;
    FVector Position=FVector::ZeroVector;
    float Speed=0,Phase=0,Happy=0,PetCooldown=0;
    bool Outdoors=false,Waiting=false;
};

// Companion behavior retains actor identity; a separate skinned feline rig supplies articulation.
UCLASS(ClassGroup=(Cheweenie),meta=(BlueprintSpawnableComponent))
class UCatCompanions:public UActorComponent
{
    GENERATED_BODY()
public:
    UCatCompanions();
    virtual void TickComponent(float Delta,ELevelTick Type,FActorComponentTickFunction* Function) override;
    void Initialize();
    void ToggleFollow(int32 Index);
    void CallBoth();
    void SendHome();
    bool Pet(int32 Index);
    bool PetNearby();
    FString Prompt() const;
    FString Status() const;
    FString State(int32 Index) const;
    bool Enabled() const;
    bool Following(int32 Index) const;
    FVector Position(int32 Index) const;
    bool Outdoors(int32 Index) const;
    void OnPlayerTravel(bool Outside);
    void ApplyFeature();
    int32 CatForStop(FName Id) const;
    FCozyCatRuntime Cats[2];
    UPROPERTY(EditAnywhere,Category="Companions") float LakituSpeed=105;
    UPROPERTY(EditAnywhere,Category="Companions") float RioSpeed=90;
    UPROPERTY(EditAnywhere,Category="Companions") float Acceleration=180;
    UPROPERTY(EditAnywhere,Category="Companions") float TurnSpeed=220;
    UPROPERTY(EditAnywhere,Category="Companions") float CatchupSpeed=245;
private:
    AMartyCharacter* Marty() const;
    bool Ready=false;
    void Tell(const FString& Text,int32 Sound=-1);
    bool PlaceNear(int32 Index,bool Outside);
};
