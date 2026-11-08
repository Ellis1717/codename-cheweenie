#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HockeySurprise.generated.h"
class AStaticMeshActor;

// Replayable, transient story vignette; never replaces the solo checkpoint.
UCLASS()
class UHockeySurprise : public UActorComponent
{
    GENERATED_BODY()
public:
    UHockeySurprise();
    void Start();
    void Finish();
    bool Active() const { return Stage>=0; }
    FString Objective() const;
    FString Prompt() const;
    void Interact();
    virtual void TickComponent(float Delta,ELevelTick Type,FActorComponentTickFunction* Function) override;
    int32 Stage=-1;
    FVector Target() const;
private:
    void Build(bool DressingRoom);
    void Clear();
    void Tell(const FString& Text,const FString& Cue=TEXT("Menu"));
    float RevealTime=0;
    UPROPERTY() TArray<TObjectPtr<AActor>> Props;
    UPROPERTY() TObjectPtr<AStaticMeshActor> Bag;
    UPROPERTY() TObjectPtr<AStaticMeshActor> Cat;
};
