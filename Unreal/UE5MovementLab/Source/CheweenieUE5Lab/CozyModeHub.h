#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "Engine/EngineBaseTypes.h"
#include "CozyModeHub.generated.h"

class UCozyAdventure;
class UNetDriver;
class UTexture2D;
enum class ECozyHubPage : uint8 { Modes, Host, Join, Archives, Graphics, Help, Leave, Overlays, ControllerSetup, Crew, Biography };
// Append-only presentation IDs. These never change gameplay feature flags.
enum class ECozyOverlay : uint8 { All, Objective, Hints, Status, Health, Energy, Backpack, Prompts, WorldLabels, LocalMap, Compass, Location, Guidance, Festival, TrickGuide, Messages, RoomStatus, PetNames, HomeStats, Reactions, Count };
struct FCozyArchiveChoice { FString Id, Label; };
FString CozyHubProfileSlot();

// One menu survives map travel; presentation preferences remain local to this PC.
UCLASS()
class UCozyModeHub : public UGameInstanceSubsystem, public FTickableGameObject
{
    GENERATED_BODY()
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    virtual UWorld* GetWorld() const override;
    virtual void Tick(float Delta) override;
    virtual bool IsTickable() const override { return !IsTemplate()&&GetWorld()!=nullptr; }
    virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UCozyModeHub,STATGROUP_Tickables); }
    virtual UWorld* GetTickableGameObjectWorld() const override { return GetWorld(); }
    void Open();
    void Back();
    void Navigate(int32 Direction);
    void Adjust(int32 Direction);
    void Confirm();
    void OpenCrew();
    UTexture2D* MenuBackdrop();
    UTexture2D* CrewPortrait();
    int32 CrewIndex=0;
    TArray<FString> Rows() const;
    FString Heading() const;
    FString Hint() const;
    int32 FurDetail();
    void AdjustFur(int32 Direction);
    void SoloReady(UCozyAdventure* Session);
    void RoomReady();
    bool CanAdjust(int32 Row) const;
    bool OverlayVisible(ECozyOverlay Id) const;
    void ToggleOverlay(ECozyOverlay Id);
    void ResetOverlays();
    void LoadOverlays();
    void SaveOverlays();
    int32 OverlayStyle() const { return HUDStyle; }
    FString OverlayStyleName() const;
    void AdjustOverlayStyle(int32 Direction);
    void Request(const FString& Mode);
    void TravelSolo(const FString& Action=TEXT("hub"));
    bool RatchetAvailable() const;
    FString Address() const;
    void SaveNetworkPreferences();
    ECozyHubPage Page=ECozyHubPage::Modes;
    int32 Selection=0, Port=7777;
    int32 Octets[4]={192,168,1,20};
    FString Status;
    TArray<FCozyArchiveChoice> Archives;
private:
    UPROPERTY() TMap<FString,TObjectPtr<UTexture2D>> MenuArtwork;
    UCozyAdventure* Session() const;
    void NewPage(ECozyHubPage Next);
    void Execute(const FString& Mode);
    void LoadArchives();
    TArray<FString> HostAddresses() const;
    void NetworkFailed(UWorld* World,UNetDriver* Driver,ENetworkFailure::Type Type,const FString& Error);
    FString PendingAction, PendingLeave;
    bool Returning=false,ReturnQueued=false,Connecting=false,Hosting=false;
    int32 CachedFur=2;
    bool FurLoaded=false;
    uint32 HiddenOverlays=0;
    int32 HUDStyle=0;
    // Acceptance state is dormant outside explicit test arguments.
    void TickAcceptance(float Delta);
    void TickOverlayAcceptance(float Delta);
    void TickStyleAcceptance(float Delta);
    int32 TestStage=0;
    double TestTime=0,TestTotal=0;
    TArray<FString> TestPassed,TestFailed;
};
