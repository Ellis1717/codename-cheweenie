#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MartyGait.generated.h"
class UPoseableMeshComponent;

struct FMartyLeg
{
    FName Upper, Lower, Hock, Paw;
    FVector Hip, Knee, Ankle, Toe, Contact, LiftOff;
    bool Planted=false;
};

// A true skin-weighted skeletal mesh, driven by analytic IK rather than animation clips.
UCLASS(ClassGroup=(Cheweenie), meta=(BlueprintSpawnableComponent))
class UMartyGait : public UActorComponent
{
    GENERATED_BODY()
public:
    UMartyGait();
    virtual void BeginPlay() override;
    virtual void TickComponent(float Delta, ELevelTick Type, FActorComponentTickFunction* Function) override;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gait") float WalkStride=27;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gait") float TrotStride=43;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gait") float PawLift=5;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Gait") float Cycle=0;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Gait") float Travel=0;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Gait") float MaxJointBend=0;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Gait") int32 PlantedPaws=0;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Gait") bool Ready=false;
    UPROPERTY() TObjectPtr<UPoseableMeshComponent> Skin;
    static FVector SolveKnee(FVector Hip,FVector Foot,float Upper,float Lower,float BendDirection);
    void ResetContacts();
    FVector MouthWorld() const;
private:
    float RestLowering=0;
    TArray<FMartyLeg> Legs;
    TMap<FName,FTransform> Rest;
    FVector LastPosition;
    float Time=0, SmoothedSpeed=0, Trot=0;
    void Aim(FName Bone,FVector Origin,FVector RestEnd,FVector End);
    FVector Ground(FVector WorldPoint) const;
};
