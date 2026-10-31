#pragma once
#include "CoreMinimal.h"
#include "MartyCharacter.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerController.h"
#include "CozyCoop.generated.h"

class UStaticMeshComponent;
class USoundBase;

// Independent, save-free LAN room. Adventure/companion state stays in the solo mode.
UCLASS()
class ACozyCoopState : public AGameStateBase
{
    GENERATED_BODY()
public:
    virtual void BeginPlay() override;
};

UCLASS()
class ACozyCoopMode : public AGameModeBase
{
    GENERATED_BODY()
public:
    ACozyCoopMode();
    virtual void BeginPlay() override;
    virtual void PreLogin(const FString& Options,const FString& Address,const FUniqueNetIdRepl& Id,FString& Error) override;
    virtual void RestartPlayer(AController* Player) override;
};

UCLASS()
class ACozyCoopPet : public AMartyCharacter
{
    GENERATED_BODY()
public:
    ACozyCoopPet();
    virtual bool UsesAdventureHome() const override { return false; }
    virtual void BeginPlay() override;
    virtual void Tick(float Delta) override;
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;
    virtual void SetupPlayerInputComponent(UInputComponent* Input) override;
    virtual void ReturnHome() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& Out) const override;
    UPROPERTY(ReplicatedUsing=ApplyPet) int32 PetId=-1;
    UPROPERTY(Replicated) int32 VoiceCount=0;
    UPROPERTY(Replicated) int32 LastVoicePet=-1;
    // Opt-in runtime acceptance handshake; dormant at -1 during ordinary play.
    UPROPERTY(Replicated) int32 AcceptanceStage=-1;
    UFUNCTION(Server,Reliable) void ServerAcceptanceStage(int32 Value);
    UFUNCTION() void ApplyPet();
    UFUNCTION(Server,Reliable) void ServerChoosePet(int32 Id);
    UFUNCTION(Server,Reliable) void ServerHurry(bool Active);
    UFUNCTION(Server,Reliable) void ServerHome();
    UFUNCTION(Server,Reliable) void ServerVoice();
    UFUNCTION(NetMulticast,Unreliable) void PlayVoice(int32 Id);
    UFUNCTION(Server,Reliable) void ServerToy(bool DropOnly);
    FString PetName() const;
    bool PetAvailable(int32 Id) const;
private:
    void MoveCoop(const FInputActionValue& Value);
    void MouseCoop(const FInputActionValue& Value);
    void StickCoop(const FInputActionValue& Value);
    void HurryOn();
    void HurryOff();
    void NextPet();
    void PreviousPet();
    void Marty();
    void Lakitu();
    void Rio();
    void JumpCoop();
    void Voice();
    void UseToy();
    void DropToy();
    void RecenterCoop();
    void ToggleMenu();
    UPROPERTY() TObjectPtr<UInputMappingContext> CoopMapping;
    UPROPERTY() TArray<TObjectPtr<UInputAction>> CoopActions;
    UPROPERTY() TObjectPtr<UStaticMesh> LakituMesh;
    UPROPERTY() TObjectPtr<UStaticMesh> RioMesh;
    UPROPERTY() TArray<TObjectPtr<USoundBase>> Voices;
    double NextVoice=0;
    int32 PosePolicy=-1;
};

UCLASS()
class ACozyCoopToy : public AActor
{
    GENERATED_BODY()
public:
    ACozyCoopToy();
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& Out) const override;
    virtual void Tick(float Delta) override;
    UPROPERTY(VisibleAnywhere) TObjectPtr<UStaticMeshComponent> Ball;
    UPROPERTY(ReplicatedUsing=SyncCarrier) TObjectPtr<ACozyCoopPet> Carrier;
    UFUNCTION() void SyncCarrier();
    void Release();
};

UCLASS()
class ACozyCoopController : public APlayerController
{
    GENERATED_BODY()
public:
    ACozyCoopController();
    virtual void BeginPlay() override;
    virtual void Tick(float Delta) override;
    UFUNCTION(Client,Reliable) void Notice(const FString& Text);
    FString Message;
    bool MenuOpen=false;
private:
    void TickAcceptance(float Delta);
    double Started=0;
    int32 Stage=0;
    float StageTime=0;
    FVector OwnStart,OtherStart;
    bool SawRemoteMove=false,SawRemoteRun=false,SawVoice=false;
    TArray<FString> Passed,Failed;
};

UCLASS()
class ACozyCoopHUD : public AHUD
{
    GENERATED_BODY()
public:
    virtual void DrawHUD() override;
};
