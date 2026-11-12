#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AnimalGait.generated.h"
class UPoseableMeshComponent;
struct FAnimalLeg
{
    FName Upper,Lower,Hock,Paw;
    FVector Hip,Knee,Ankle,Toe,Contact,LiftOff;
    bool Planted=false;
};
// Shared skin/IK driver; canine and feline rhythms remain distinct. No movement authority.
UCLASS(ClassGroup=(Cheweenie))
class UAnimalGait:public UActorComponent
{
    GENERATED_BODY()
public:
    UAnimalGait();
    bool Initialize(const FString& Profile,bool Cat,USceneComponent* Parent,FTransform Local=FTransform::Identity);
    virtual void TickComponent(float Delta,ELevelTick Type,FActorComponentTickFunction* Function) override;
    void ResetContacts();
    UPROPERTY() TObjectPtr<UPoseableMeshComponent> Skin;
    bool Ready=false,Feline=false,Happy=false,Sniff=false;
    float Cycle=0,Travel=0,MaxJointBend=0,BodyMotion=0,Speed=0;
    int32 PlantedPaws=0,ContactResets=0;
    const TArray<FAnimalLeg>& LegStates() const{return Legs;}
private:
    TArray<FAnimalLeg> Legs;
    TMap<FName,FTransform> Rest;
    FVector Last=FVector::ZeroVector;
    float Time=0;
    FString BoundProfile;
    FVector Ground(FVector Point) const;
    void Aim(FName Bone,FVector Start,FVector RestEnd,FVector End);
};
