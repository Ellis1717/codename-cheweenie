#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DigVisuals.generated.h"
class UDogLife;
class UInstancedStaticMeshComponent;
class UStaticMeshComponent;
class UTextRenderComponent;
class UMaterialInstanceDynamic;
class UProceduralMeshComponent;

// Local ground apertures and colliding pits descend below the surrounding surface.
UCLASS()
class UDigVisuals : public UActorComponent
{
    GENERATED_BODY()
public:
    UDigVisuals();
    void Initialize(UDogLife* InLife);
    void Refresh();
    virtual void TickComponent(float Delta,ELevelTick Type,FActorComponentTickFunction* Function) override;
    int32 Nearest() const;
    int32 VisibleDepth(int32 Patch) const;
    bool Covered(int32 Patch) const;
    bool WorldLabelVisible(int32 Patch) const;
    int32 FlyingClods() const;
    float Relief(int32 Patch) const;
    float BottomElevation(int32 Patch) const;
    float StrokeProgress() const;
    FString DepthText(int32 Patch) const;
private:
    UPROPERTY() TObjectPtr<UDogLife> Life;
    UPROPERTY() TArray<TObjectPtr<UMaterialInstanceDynamic>> Palette;
    UPROPERTY() TArray<TObjectPtr<AActor>> PatchActors;
    UPROPERTY() TArray<TObjectPtr<UStaticMeshComponent>> Centers;
    UPROPERTY() TArray<TObjectPtr<UInstancedStaticMeshComponent>> Rims;
    UPROPERTY() TArray<TObjectPtr<UInstancedStaticMeshComponent>> Covers;
    UPROPERTY() TArray<TObjectPtr<UTextRenderComponent>> Labels;
    UPROPERTY() TArray<TObjectPtr<UProceduralMeshComponent>> Pits;
    UPROPERTY() TObjectPtr<UInstancedStaticMeshComponent> Clods;
    TArray<int32> Depths,Buried;
    bool WasDigging=false;
    float DigElapsed=0,DigDuration=1;
    void OpenGround();
    void RebuildPit(int32 Patch,int32 Depth,bool Filled);
};
