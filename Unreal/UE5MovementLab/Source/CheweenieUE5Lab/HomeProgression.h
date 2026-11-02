#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HomeProgression.generated.h"
class AMartyCharacter;
class AMartyPlayMode;
class UCozyAdventureSave;
class UMaterialInstanceDynamic;
class UTextRenderComponent;
class UStaticMeshComponent;

// Persistent gentle progression; station actions and scene presentation stay local to solo.
UCLASS(ClassGroup=(Cheweenie),meta=(BlueprintSpawnableComponent))
class UHomeProgression : public UActorComponent
{
    GENERATED_BODY()
public:
    UHomeProgression();
    virtual void TickComponent(float Delta,ELevelTick Type,FActorComponentTickFunction* Function) override;
    void Initialize();
    bool Enabled() const;
    bool InAnnex() const;
    static bool PointInAnnex(FVector P){return P.X>7400&&P.X<9200&&FMath::Abs(P.Y)<600;}
    bool Interact();
    FString Prompt() const;
    FString Summary() const;
    FString BuffSummary() const;
    FString TrainingObjective() const;
    FVector TrainingGoal() const;
    void OnSkill(int32 Skill);
    void OnMeal();
    void CancelActivities();
    float SpeedMultiplier() const;
    float JumpMultiplier() const;
    float DrainMultiplier() const;
    float DamageMultiplier() const;
    float DigDuration() const;
    float DigCost() const;
    void Travel(bool Enter);
    bool GearVisible() const;
    int32 UpgradeCost(int32 Type) const;
    int32 Level(int32 Type) const;
    UPROPERTY(EditAnywhere,Category="Home") float BedEnergyRecovery=4;
    UPROPERTY(EditAnywhere,Category="Home") float BedHealthRecovery=1.5f;
    UPROPERTY(EditAnywhere,Category="Home") float MealDuration=120;
    UPROPERTY(EditAnywhere,Category="Home") float RestDuration=180;
    UPROPERTY(EditAnywhere,Category="Home") float PraiseCooldown=8;
    FVector Bed=FVector(-350,-345,0),Portal=FVector(410,350,28),Exit=FVector(7800,-220,28);
    FVector ArmorBench=FVector(8000,-220,0),BumpTest=FVector(8110,180,0);
    FVector Gym[3]={FVector(8500,-220,0),FVector(8800,-220,0),FVector(8400,180,0)};
    FVector Finish=FVector(8820,180,28);
    bool Resting=false;
    int32 Training=-1,Reactions=0;
    float TrainingRemaining=0;
private:
    AMartyPlayMode* Mode() const;
    AMartyCharacter* Marty() const;
    UCozyAdventureSave* Data() const;
    bool Playing() const;
    void Tell(const FString& Text,const FString& Sound=TEXT("Menu"));
    void Build();
    void FinishTraining();
    void UpdateGear();
    void Praise(int32 Witness,int32 Skill);
    AActor* Shape(const TCHAR* Tag,FVector P,FVector Size,int32 Color,bool Solid=false,const TCHAR* Mesh=TEXT("Cube"));
    void Sign(const FString& Text,FVector P,float Size=20);
    UPROPERTY() TArray<TObjectPtr<UMaterialInstanceDynamic>> Palette;
    UPROPERTY() TArray<TObjectPtr<UTextRenderComponent>> ReactionLabels;
    UPROPERTY() TObjectPtr<AActor> Gear;
    UPROPERTY() TArray<TObjectPtr<UStaticMeshComponent>> GearPieces;
    bool Ready=false;
    float RestTime=0,TrainingSpeed=0,BumpCooldown=0;
    float ReactionTime[2]={0,0};
    FVector TrainingStart;
};
UHomeProgression* CheweenieHome(const UObject* Context);
