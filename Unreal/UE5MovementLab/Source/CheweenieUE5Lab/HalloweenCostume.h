#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HalloweenCostume.generated.h"

class USceneComponent;
class UStaticMeshComponent;

// Visual overlays only: no collision, stat bonuses or replacement pet geometry.
UCLASS(ClassGroup=(Cheweenie))
class UHalloweenCostume : public UActorComponent
{
    GENERATED_BODY()
public:
    UHalloweenCostume();
    virtual void TickComponent(float Delta, ELevelTick Type, FActorComponentTickFunction* Function) override;
    void SetOutfit(USceneComponent* PetMesh, int32 Pet, bool Enabled);
    int32 VisiblePieces() const;
    bool CollisionFree() const;
    bool MaterialsReady() const;
    int32 OutfitPet = -1;
private:
    void Build();
    void Piece(const TCHAR* Shape, const TCHAR* Material, FVector Position, FVector Size, FRotator Rotation = FRotator::ZeroRotator);
    UPROPERTY() TObjectPtr<USceneComponent> Anchor;
    UPROPERTY() TObjectPtr<USceneComponent> Target;
    UPROPERTY() TArray<TObjectPtr<UStaticMeshComponent>> Pieces;
    bool Wearing = false;
};
