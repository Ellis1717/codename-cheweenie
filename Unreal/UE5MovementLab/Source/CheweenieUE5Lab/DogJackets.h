#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DogJackets.generated.h"
class USceneComponent;
class UProceduralMeshComponent;
class UMaterialInstanceDynamic;

// Original sleeve-free clothing. Visual only; no collision or stat changes.
UCLASS(ClassGroup=(Cheweenie))
class UDogJackets : public UActorComponent
{
    GENERATED_BODY()
public:
    UDogJackets();
    void SetJacket(USceneComponent* PetMesh,int32 Choice);
    virtual void TickComponent(float Delta,ELevelTick Type,FActorComponentTickFunction* Function) override;
    int32 VisiblePieces() const;
    bool CollisionFree() const;
    bool MaterialsReady() const;
    bool SurfaceFacesOutward() const;
    FTransform JacketTransform() const;
    int32 Style=0;
    static FString Name(int32 Choice);
private:
    void Build();
    void Shell(float Start,float End,float Width,float Height,float Z,int32 Material,float MinAngle=-108,float MaxAngle=108,bool Puffy=false,float RearCurve=0,float FrontCurve=0,float NeckRise=0);
    UPROPERTY() TObjectPtr<USceneComponent> Anchor;
    UPROPERTY() TObjectPtr<USceneComponent> Target;
    UPROPERTY() TArray<TObjectPtr<UProceduralMeshComponent>> Pieces;
    UPROPERTY() TArray<TObjectPtr<UMaterialInstanceDynamic>> Palette;
};
