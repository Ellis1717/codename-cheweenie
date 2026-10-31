#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HalloweenSpecial.generated.h"
class AMartyCharacter;
class AActor;
class UTextRenderComponent;
class UPointLightComponent;
class UInstancedStaticMeshComponent;

struct FCozyPorch
{
    FVector Position=FVector::ZeroVector;
    FString Name;
};

// Optional solo event. Saved stickers and badge are independent of the main patrol.
UCLASS(ClassGroup=(Cheweenie))
class UHalloweenSpecial : public UActorComponent
{
    GENERATED_BODY()
public:
    UHalloweenSpecial();
    virtual void TickComponent(float Delta,ELevelTick Type,FActorComponentTickFunction* Function) override;
    void Prepare();
    void StartTrail();
    void Toggle();
    void Replay();
    bool Active() const;
    int32 StampCount() const;
    int32 NextPorch() const;
    FVector GoalPosition() const;
    FString Objective() const;
    FString Prompt() const;
    bool Interact(AMartyCharacter* Pawn);
    TArray<FCozyPorch> Porches;
    FVector Clubhouse() const;
    bool DecorationsVisible() const;
    bool DecorationsCollisionFree() const;
private:
    void Refresh();
    void BuildDecorations();
    void Instance(const TCHAR* Mesh,const TCHAR* Material,FVector P,FVector Size,FRotator R=FRotator::ZeroRotator);
    UPROPERTY() TObjectPtr<AActor> Decor;
    UPROPERTY() TMap<FString,TObjectPtr<UInstancedStaticMeshComponent>> Batches;
    UPROPERTY() TArray<TObjectPtr<UTextRenderComponent>> Labels;
    UPROPERTY() TArray<TObjectPtr<UPointLightComponent>> Lights;
    int32 LastVisibility=-1;
};
