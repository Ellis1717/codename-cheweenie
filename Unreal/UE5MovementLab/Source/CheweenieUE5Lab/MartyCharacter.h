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
struct FInputActionValue;

UCLASS()
class AMartyCharacter : public ACharacter
{
    GENERATED_BODY()
public:
    AMartyCharacter();
    virtual void BeginPlay() override;
    virtual void PossessedBy(AController* NewController) override;
    virtual void SetupPlayerInputComponent(UInputComponent* Input) override;
    void CycleCamera();
    void ReturnHome();
    int32 CameraPreset = 0;
private:
    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void LookStick(const FInputActionValue& Value);
    void Recenter();
    void SetSprint(bool Active);
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
    FVector Home;
};

UCLASS()
class AMartyPlayMode : public AGameModeBase
{
    GENERATED_BODY()
public:
    AMartyPlayMode();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
private:
    void SmokeCheck(bool Passed, const FString& Name);
    int32 SmokeStage = 0;
    double SmokeTime = 0;
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
};
