#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataAsset.h"
#include "GameFramework/SaveGame.h"
#include "CozyAdventure.generated.h"

class AStaticMeshActor;
class UAudioComponent;
class USoundBase;
class AMartyCharacter;

UENUM(BlueprintType)
enum class ECozyScreen : uint8 { Title, Playing, Pause, Controls, Settings, Restart, Complete, Backpack, Tricks, Features, Map, Companions, Wardrobe };

UENUM()
enum class ECozyFeature : uint8 { Map, Compass, Waypoint, Route, WorldLabels, Location, Energy, Health, Backpack, Tricks, Jump, Dig, Carry, Food, TV, Bark, Hurry, Orbit, Cameras, ActionViews, Hints, DetailedCoat, Music, Effects, Companions, Count };

USTRUCT(BlueprintType)
struct FCozyStop
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FName Id;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FString Label;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector Position = FVector::ZeroVector;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float Radius = 55;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FString Dialogue;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FString SoundName = TEXT("Menu");
};

// Author a Data Asset of this class to replace the default patrol without changing the pawn.
UCLASS(BlueprintType)
class UCozyMissionDefinition : public UDataAsset
{
    GENERATED_BODY()
public:
    UCozyMissionDefinition();
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FString MissionId = TEXT("first-sniff-patrol-v1");
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FString Title = TEXT("The Great Cozy Patrol");
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FCozyStop> Snacks;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FCozyStop> Visits;
};

UCLASS()
class UCozyAdventureSave : public USaveGame
{
    GENERATED_BODY()
public:
    UPROPERTY(SaveGame) int32 Version = 1;
    UPROPERTY(SaveGame) FString MissionId;
    UPROPERTY(SaveGame) int32 SnackMask = 0;
    UPROPERTY(SaveGame) int32 VisitIndex = 0;
    UPROPERTY(SaveGame) bool Complete = false;
    UPROPERTY(SaveGame) int32 NeighbourDogMask=0;
    UPROPERTY(SaveGame) float Music = .25f;
    UPROPERTY(SaveGame) float Effects = .6f;
    UPROPERTY(SaveGame) float PetEnergy=80;
    UPROPERTY(SaveGame) int32 CarryItem=-1;
    UPROPERTY(SaveGame) TArray<int32> HoleDepths={0,0,0};
    UPROPERTY(SaveGame) TArray<int32> ItemHoles={-1,-1};
    UPROPERTY(SaveGame) bool PetTVOn=false;
    UPROPERTY(SaveGame) float PetHealth=100;
    UPROPERTY(SaveGame) TArray<int32> BagItems;
    UPROPERTY(SaveGame) int32 PetPace=0;
    UPROPERTY(SaveGame) bool BodyControls=false;
    UPROPERTY(SaveGame) int32 DisabledFeatures=0;
    UPROPERTY(SaveGame) int32 NavTarget=0;
    UPROPERTY(SaveGame) int32 CatFollowMask=0;
    UPROPERTY(SaveGame) int32 CostumeMask=0;
    UPROPERTY(SaveGame) bool RatchetUnlocked=false;
    UPROPERTY(SaveGame) int32 LakituBond=0;
    UPROPERTY(SaveGame) int32 RioBond=0;
};

UCLASS(ClassGroup=(Cheweenie), meta=(BlueprintSpawnableComponent))
class UCozyAdventure : public UActorComponent
{
    GENERATED_BODY()
public:
    UCozyAdventure();
    virtual void BeginPlay() override;
    virtual void TickComponent(float Delta, ELevelTick TickType, FActorComponentTickFunction* ThisTick) override;
    UFUNCTION(BlueprintCallable,Category="Cheweenie|Adventure") void Confirm();
    UFUNCTION(BlueprintCallable,Category="Cheweenie|Adventure") void Back();
    UFUNCTION(BlueprintCallable,Category="Cheweenie|Adventure") void Pause();
    UFUNCTION(BlueprintCallable,Category="Cheweenie|Adventure") void Navigate(int32 Direction);
    UFUNCTION(BlueprintCallable,Category="Cheweenie|Adventure") void Adjust(int32 Direction);
    UFUNCTION(BlueprintCallable,Category="Cheweenie|Adventure") void Interact();
    UFUNCTION(BlueprintCallable,Category="Cheweenie|Adventure") void Start(bool Fresh);
    UFUNCTION(BlueprintCallable,Category="Cheweenie|Adventure") void Save();
    UFUNCTION(BlueprintPure,Category="Cheweenie|Adventure") bool AllowsMovement() const { return !Enabled || Screen == ECozyScreen::Playing; }
    void HoldMessage(float Seconds) { MessageTime=Seconds; }
    void ActionCue(const FString& Name) { PlayCue(Name); }
    void CheckRatchetUnlock();
    void VisitRatchet();
    void OpenBackpack();
    void OpenTricks();
    void OpenMap();
    void OpenCompanions();
    FVector StopPosition(const FCozyStop& Stop) const;
    bool StopOutdoors(const FCozyStop& Stop) const;
    bool Feature(ECozyFeature Id) const;
    void ToggleFeature(ECozyFeature Id);
    TArray<ECozyFeature> FeatureGroup() const;
    int32 FeaturePage=0;
    FString Objective() const;
    FString Prompt() const;
    TArray<FString> Menu() const;
    FString Heading() const;
    UPROPERTY(BlueprintReadOnly) ECozyScreen Screen = ECozyScreen::Title;
    UPROPERTY(BlueprintReadOnly) int32 Selection = 0;
    UPROPERTY(BlueprintReadOnly) TObjectPtr<UCozyAdventureSave> Progress;
    UPROPERTY(BlueprintReadOnly) TObjectPtr<UCozyMissionDefinition> Mission;
    UPROPERTY(BlueprintReadOnly) FString Message;
    bool Enabled = true;
    FString Slot;
    int32 LoadedSoundCount() const { return Sounds.Num(); }
    void UpdateMusic();
    bool MusicPlaying() const;
    FString MusicTrack;
    FString LastCue;
    int32 AudibleCueCount=0;
private:
    AMartyCharacter* Marty() const;
    void SetScreen(ECozyScreen Next);
    void RebuildMarkers();
    void PlayCue(const FString& Name);
    void BuildFort();
    void CompleteRoom();
    UPROPERTY() TArray<TObjectPtr<AStaticMeshActor>> Markers;
    UPROPERTY() TMap<FString,TObjectPtr<USoundBase>> Sounds;
    UPROPERTY() TObjectPtr<UAudioComponent> MusicPlayer;
    ECozyScreen ReturnScreen = ECozyScreen::Title;
    float MessageTime = 0;
    float PulseTime = 0;
};
