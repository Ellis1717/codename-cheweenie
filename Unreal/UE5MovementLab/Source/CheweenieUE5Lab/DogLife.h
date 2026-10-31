#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DogLife.generated.h"
class AMartyCharacter;
class UCozyAdventure;
class UCozyAdventureSave;
class AStaticMeshActor;
class UMaterialInstanceDynamic;
class ACameraActor;
enum class ECozyFeature:uint8;

UENUM(BlueprintType)
enum class EDogActivity:uint8 { Idle, Digging, Eating, WatchingTV, CouchHop, Lying, Rolling, Shaking };

// Pet needs and cozy verbs are independent of mission completion and the movement motor.
UCLASS(ClassGroup=(Cheweenie),meta=(BlueprintSpawnableComponent))
class UDogLife:public UActorComponent
{
    GENERATED_BODY()
public:
    UDogLife();
    virtual void BeginPlay() override;
    virtual void TickComponent(float Delta,ELevelTick Type,FActorComponentTickFunction* Function) override;
    UFUNCTION(BlueprintCallable) void Dig();
    UFUNCTION(BlueprintCallable) void Drop();
    UFUNCTION(BlueprintCallable) void Bark();
    int32 BarkCount=0;
    void LieDown();
    void StandUp();
    void RollOver();
    void ShakePaw();
    void Stow();
    void Retrieve();
    void CyclePace();
    FString PaceName() const;
    FString BackpackText() const;
    float WalkSpeed() const;
    float Health() const;
    float ActionProgress() const;
    UFUNCTION(BlueprintCallable) void AdjustHealth(float Amount);
    void ToggleBodyControls();
    bool UsesBodyControls() const;
    void SetBodyStick(FVector2D Value);
    void ClearBodyStick();
    float BodyRoll=0,BodyPitch=0,BodyYawSpeed=0;
    UFUNCTION(BlueprintCallable) bool Interact();
    UFUNCTION(BlueprintCallable) bool PayJump();
    UFUNCTION(BlueprintCallable) void StopWatching();
    void CancelAction();
    UFUNCTION(BlueprintPure) FString Mood() const;
    FString Status() const;
    FString Prompt() const;
    bool Busy() const;
    bool OnCouch() const;
    float Energy() const;
    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Needs") float WalkingDrain=.04f;
    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Needs") float HurryDrain=.65f;
    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Needs") float IdleRecovery=.65f;
    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Needs") float TVRecovery=1.5f;
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly) EDogActivity Activity=EDogActivity::Idle;
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly) float Excitement=0;
    FVector Bowl=FVector(-180,320,0);
    TArray<FVector> Patches;
    bool SprintRequested=false;
private:
    AMartyCharacter* Pawn() const;
    UCozyAdventure* Session() const;
    UCozyAdventureSave* Data() const;
    bool Playing() const;
    bool Allows(ECozyFeature Id) const;
    int32 NearPatch() const;
    void Tell(const FString& Text,const FString& Sound=TEXT("Menu"));
    void Save();
    void Build();
    void Initialize();
    void SyncItems();
    void SyncHoles();
    void FinishAction();
    AStaticMeshActor* Prop(FString Name,FVector P,FVector Size,int32 Color,const TCHAR* Shape=TEXT("/Engine/BasicShapes/Sphere.Sphere"),bool Solid=false);
    void Label(FString Text,FVector P,float Yaw=0);
    UPROPERTY() TArray<TObjectPtr<UMaterialInstanceDynamic>> Materials;
    UPROPERTY() TArray<TObjectPtr<AStaticMeshActor>> Items;
    UPROPERTY() TArray<TObjectPtr<AStaticMeshActor>> Holes;
    UPROPERTY() TObjectPtr<AStaticMeshActor> TVBall;
    UPROPERTY() TObjectPtr<ACameraActor> TVCamera;
    TArray<FVector> LooseLocations;
    float ActionTime=0,SaveTime=0,FoodCooldown=0,TVTime=0,HopDelay=0;
    int32 PendingPatch=INDEX_NONE;
    bool Initialized=false;
    float BarkCooldown=0,StepClock=0;
    float TrickDuration=1;
    FVector2D BodyStick=FVector2D::ZeroVector,BodyVelocity=FVector2D::ZeroVector;
    void TickBodyControls(float Delta);
    bool WasAirborne=false;
};
