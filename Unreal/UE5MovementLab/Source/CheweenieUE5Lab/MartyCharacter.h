#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/HUD.h"
#include "MartyCharacter.generated.h"

class UInputAction;
class UInputMappingContext;
class USpringArmComponent;
class UStaticMeshComponent;
class UCozyAdventure;
class UTexture2D;
class UMartyGait;
class UPoseableMeshComponent;
class UCheweenieNeighbourhood;
class UDogLife;
class UCozyNavigation;
class UCatCompanions;
class UHalloweenCostume;
struct FInputActionValue;

UCLASS()
class AMartyCharacter : public ACharacter
{
    GENERATED_BODY()
public:
    AMartyCharacter();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void PossessedBy(AController* NewController) override;
    virtual void SetupPlayerInputComponent(UInputComponent* Input) override;
    void CycleCamera();
    virtual void ReturnHome();
    virtual bool UsesAdventureHome() const { return true; }
    virtual float TakeDamage(float Amount,const FDamageEvent& Event,AController* EventInstigator,AActor* Causer) override;
    int32 CameraPreset = 0;
    UPROPERTY(VisibleAnywhere) TObjectPtr<UHalloweenCostume> Costume;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<UMartyGait> Gait;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<UPoseableMeshComponent> Skin;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<UDogLife> Life;
private:
    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void LookStick(const FInputActionValue& Value);
    void Recenter();
    void SetSprint(bool Active);
    bool CanPlay() const;
    UCozyAdventure* Adventure() const;
    void MenuConfirm();
    void MenuBack();
    void MenuPause();
    void MenuUp();
    void MenuDown();
    void MenuLeft();
    void MenuRight();
    void Interact();
    void PetJump();
    void Dig();
    void DropToy();
    void Bark();
    void Backpack();
    void ClearBodyStick();
    void Map();
    void PadUp();
    void PadDown();
    void Crew();
    UPROPERTY() TObjectPtr<USpringArmComponent> Boom;
    UPROPERTY() TObjectPtr<UStaticMeshComponent> Visual;
    UPROPERTY() TObjectPtr<UInputMappingContext> Mapping;
    UPROPERTY() TObjectPtr<UInputAction> MoveAction;
    UPROPERTY() TObjectPtr<UInputAction> MouseAction;
    UPROPERTY() TObjectPtr<UInputAction> StickAction;
    UPROPERTY() TObjectPtr<UInputAction> SprintAction;
    UPROPERTY() TObjectPtr<UInputAction> CameraAction;
    UPROPERTY() TObjectPtr<UInputAction> RecenterAction;
    UPROPERTY() TObjectPtr<UInputAction> HomeAction;
    UPROPERTY() TArray<TObjectPtr<UInputAction>> MenuActions;
    int32 LastDetailed=-1;
    FVector Home;
};

// Deterministic collision setup for the retained room, independently on each peer.
void PrepareCheweenieRoom(UWorld* World, bool HideAllPets = false);

UCLASS()
class AMartyPlayMode : public AGameModeBase
{
    GENERATED_BODY()
public:
    AMartyPlayMode();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<UCozyAdventure> Adventure;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<UCheweenieNeighbourhood> Neighbourhood;
    UPROPERTY(VisibleAnywhere) TObjectPtr<UCozyNavigation> Navigation;
    UPROPERTY(VisibleAnywhere) TObjectPtr<UCatCompanions> Companions;
private:
    void SmokeCheck(bool Passed, const FString& Name);
    void TickAdventureSmoke(float Delta);
    void TickGaitSmoke(float Delta);
    void TickOutsideSmoke(float Delta);
    void TickLifeSmoke(float Delta);
    void TickAudioSmoke(float Delta);
    void TickTricksSmoke(float Delta);
    void TickNavigationSmoke(float Delta);
    void TickCompanionSmoke(float Delta);
    void TickWorldSmoke(float Delta);
    void TickCostumeSmoke(float Delta);
    void TickRatchetSmoke(float Delta);
    int32 SmokeStage = 0;
    double SmokeTime = 0;
    float SmokeYaw = 0;
    float SmokePitch = 0;
    bool SmokeMoving = false;
    FVector SmokePosition;
    TArray<FString> SmokePassed;
    TArray<FString> SmokeFailed;
};

UCLASS()
class AMartyHUD : public AHUD
{
    GENERATED_BODY()
public:
    virtual void DrawHUD() override;
    virtual void NotifyHitBoxClick(FName BoxName) override;
private:
    bool DrawAdventureHUD();
    void DrawNavigation(bool FullMap);
    UPROPERTY() TObjectPtr<UTexture2D> Cover;
};
